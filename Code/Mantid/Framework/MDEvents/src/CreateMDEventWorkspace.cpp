#include "MantidAPI/FileProperty.h"
#include "MantidAPI/IMDEventWorkspace.h"
#include "MantidGeometry/MDGeometry/MDHistoDimension.h"
#include "MantidKernel/ArrayProperty.h"
#include "MantidKernel/System.h"
#include "MantidMDEvents/CreateMDEventWorkspace.h"
#include "MantidMDEvents/MDEventFactory.h"
#include "MantidKernel/Memory.h"
#include <math.h>

namespace Mantid
{
namespace MDEvents
{
  using namespace Mantid::Kernel;
  using namespace Mantid::API;
  using namespace Mantid::Geometry;

  // Register the algorithm into the AlgorithmFactory
  DECLARE_ALGORITHM(CreateMDEventWorkspace)
  
  /// Sets documentation strings for this algorithm
  void CreateMDEventWorkspace::initDocs()
  {
    this->setWikiSummary(" Creates an empty MDEventWorkspace with a given number of dimensions. ");
    this->setOptionalMessage("Creates an empty MDEventWorkspace with a given number of dimensions.");
  }

  //----------------------------------------------------------------------------------------------
  /** Constructor
   */
  CreateMDEventWorkspace::CreateMDEventWorkspace()
  {
  }
    
  //----------------------------------------------------------------------------------------------
  /** Destructor
   */
  CreateMDEventWorkspace::~CreateMDEventWorkspace()
  {
  }
  

  //----------------------------------------------------------------------------------------------
  /** Initialize the algorithm's properties.
   */
  void CreateMDEventWorkspace::init()
  {
    declareProperty(new PropertyWithValue<int>("Dimensions",1,Direction::Input), "Number of dimensions that the workspace will have.");

    std::vector<std::string> propOptions;
    propOptions.push_back("MDEvent");
    propOptions.push_back("MDLeanEvent");
    declareProperty("EventType", "MDLeanEvent",new ListValidator(propOptions),
      "Which underlying data type will event take.");

    declareProperty(
      new ArrayProperty<double>("Extents"),
      "A comma separated list of min, max for each dimension,\n"
      "specifying the extents of each dimension.");

    declareProperty(
      new ArrayProperty<std::string>("Names"),
      "A comma separated list of the name of each dimension.");

    declareProperty(
      new ArrayProperty<std::string>("Units"),
      "A comma separated list of the units of each dimension.");

    declareProperty(
      new ArrayProperty<int>("SplitInto", "5"),
      "A comma separated list of into how many sub-grid elements each dimension should split; \n"
      "or just one to split into the same number for all dimensions. Default 5.");

    declareProperty(
      new PropertyWithValue<int>("SplitThreshold", 1000),
      "How many events in a box before it should be split. Default 1000.");

    declareProperty(
      new PropertyWithValue<int>("MinRecursionDepth", 0),
      "Optional. If specified, then all the boxes will be split to this minimum recursion depth. 0 = no splitting, 1 = one level of splitting, etc.\n"
      "Be careful using this since it can quickly create a huge number of boxes = (SplitInto ^ (MinRercursionDepth * NumDimensions)).");

    declareProperty(
      new PropertyWithValue<int>("MaxRecursionDepth", 5),
      "How many levels of box splitting recursion are allowed. \n"
      "The smallest box will have each side length l = (extents) / (SplitInto ^ MaxRecursionDepth). Default 10.");

    declareProperty(new WorkspaceProperty<Workspace>("OutputWorkspace","",Direction::Output), "Name of the output MDEventWorkspace.");

    std::vector<std::string> exts;
    exts.push_back(".nxs");
    declareProperty(new FileProperty("Filename", "", FileProperty::OptionalSave, exts),
        "Optional: to use a file as the back end, give the path to the file to save.");

    declareProperty(new PropertyWithValue<int>("Memory", -1),
        "If Filename is specified to use a file back end:\n"
        "  The amount of memory (in MB) to allocate to the in-memory cache.\n"
        "  If not specified, a default of 40% of free physical memory is used.");

  }


  /** Finish initialisation
   *
   * @param ws :: MDEventWorkspace to finish
   */
  template<typename MDE, size_t nd>
  void CreateMDEventWorkspace::finish(typename MDEventWorkspace<MDE, nd>::sptr ws)
  {
    // ------------ Set up the box controller ----------------------------------
    BoxController_sptr bc = ws->getBoxController();
    int val;
    val = this->getProperty("SplitThreshold");
    bc->setSplitThreshold( val );
    val = this->getProperty("MaxRecursionDepth");
    bc->setMaxDepth( val );

    // Build MDGridBox
    std::vector<int> splits = getProperty("SplitInto");
    if (splits.size() == 1)
    {
      bc->setSplitInto(splits[0]);
    }
    else if (splits.size() == nd)
    {
      for (size_t d=0; d<nd; ++d)
        bc->setSplitInto(d, splits[0]);
    }
    else
      throw std::invalid_argument("SplitInto parameter must have 1 or ndims arguments.");
    bc->resetNumBoxes();

    // Split to level 1
    ws->splitBox();

    // Do we split more due to MinRecursionDepth?
    int minDepth = this->getProperty("MinRecursionDepth");
    double numBoxes = pow(double(bc->getNumSplit()), double(minDepth));
    double memoryToUse = numBoxes * double(sizeof(MDBox<MDE,nd>)) / 1024.0;
    MemoryStats stats;
    if (double(stats.availMem()) < memoryToUse)
    {
      g_log.error() << "MinRecursionDepth is set to " << minDepth << ", which would create " << numBoxes << " boxes using " <<  memoryToUse << " kB of memory."
          << " You have " << stats.availMem() << " kB available." << std::endl;
      throw std::runtime_error("Not enough memory available for the given MinRecursionDepth!");
    }

    for (int depth = 1; depth < minDepth; depth++)
    {
      // Get all the MDGridBoxes in the workspace
      std::vector<IMDBox<MDE,nd>*> boxes;
      boxes.clear();
      ws->getBox()->getBoxes(boxes, depth-1, false);
      for (size_t i=0; i<boxes.size(); i++)
      {
        IMDBox<MDE,nd> * box = boxes[i];
        MDGridBox<MDE,nd>* gbox = dynamic_cast<MDGridBox<MDE,nd>*>(box);
        if (gbox)
        {
          // Split ALL the contents.
          for (size_t j=0; j<gbox->getNumChildren(); j++)
            gbox->splitContents(j, NULL);
        }
      }
    }

  }


  //----------------------------------------------------------------------------------------------
  /** Execute the algorithm.
   */
  void CreateMDEventWorkspace::exec()
  {
    // Get the properties and validate them
    std::string eventType = getPropertyValue("EventType");
    int ndims_prop = getProperty("Dimensions");
    if (ndims_prop <= 0)
      throw std::invalid_argument("You must specify a number of dimensions >= 1.");
    int mind = this->getProperty("MinRecursionDepth");
    int maxd = this->getProperty("MaxRecursionDepth");
    if (mind > maxd)
      throw std::invalid_argument("MinRecursionDepth must be <= MaxRecursionDepth.");
    if (mind < 0 || maxd < 0)
      throw std::invalid_argument("MinRecursionDepth and MaxRecursionDepth must be positive.");

    size_t ndims = static_cast<size_t>(ndims_prop);

    std::vector<double> extents = getProperty("Extents");
    std::vector<std::string> names = getProperty("Names");
    std::vector<std::string> units = getProperty("Units");

    if (extents.size() != ndims*2)
      throw std::invalid_argument("You must specify twice as many extents (min,max) as there are dimensions.");
    if (names.size() != ndims)
      throw std::invalid_argument("You must specify as many names as there are dimensions.");
    if (units.size() != ndims)
      throw std::invalid_argument("You must specify as many units as there are dimensions.");

    // Have the factory create it
    IMDEventWorkspace_sptr out = MDEventFactory::CreateMDEventWorkspace(ndims, eventType);

    // Give all the dimensions
    for (size_t d=0; d<ndims; d++)
    {
      MDHistoDimension * dim = new MDHistoDimension(names[d], names[d], units[d], extents[d*2], extents[d*2+1], 1);
      out->addDimension(MDHistoDimension_sptr(dim));
    }

    // Initialize it using the dimension
    out->initialize();

    // Call the templated function to finish ints
    CALL_MDEVENT_FUNCTION(this->finish, out);

    // --- File back end ? ----------------
    std::string filename = getProperty("Filename");
    if (!filename.empty())
    {
      // First save to the NXS file
      g_log.notice() << "Running SaveMDEW" << std::endl;
      IAlgorithm_sptr alg = createSubAlgorithm("SaveMDEW");
      alg->setPropertyValue("Filename", filename);
      alg->setProperty("InputWorkspace", out);
      alg->executeAsSubAlg();
      // And now re-load it with this file as the backing.
      g_log.notice() << "Running LoadMDEW" << std::endl;
      alg = createSubAlgorithm("LoadMDEW");
      alg->setPropertyValue("Filename", filename);
      alg->setProperty("FileBackEnd", true);
      alg->setPropertyValue("Memory", getPropertyValue("Memory"));
      alg->executeAsSubAlg();
      // Replace the workspace with the loaded, file-backed one
      out = alg->getProperty("OutputWorkspace");
    }

    // Save it on the output.
    setProperty("OutputWorkspace", boost::dynamic_pointer_cast<Workspace>(out));
  }



} // namespace Mantid
} // namespace MDEvents

