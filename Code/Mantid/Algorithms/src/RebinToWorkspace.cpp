//--------------------------------
// Includes
//------------------------------
#include "MantidAlgorithms/RebinToWorkspace.h"
#include "MantidAPI/MatrixWorkspace.h"

using namespace Mantid::API;
using namespace Mantid::Algorithms;

// Register the algorithm into the AlgorithmFactory
DECLARE_ALGORITHM(RebinToWorkspace)

// Get a reference to the logger. It is used to print out information, warning and error messages
Mantid::Kernel::Logger& RebinToWorkspace::g_log = Mantid::Kernel::Logger::get("RebinToWorkspace");

//---------------------------
// Private Methods
//---------------------------
/**
 * Initialise the algorithm
 */
void RebinToWorkspace::init()
{
  //  using namespace Mantid::DataObjects;
  declareProperty(new WorkspaceProperty<>("WorkspaceToRebin","", Kernel::Direction::Input));
  declareProperty(new WorkspaceProperty<>("WorkspaceToMatch","",Kernel::Direction::Input));
  declareProperty(new WorkspaceProperty<>("OutputWorkspace","",Kernel::Direction::Output));
}

/**
 * Execute the algorithm
 */
void RebinToWorkspace::exec()
{
  //The input workspaces ...
  MatrixWorkspace_sptr toRebin = getProperty("WorkspaceToRebin");
  MatrixWorkspace_sptr toMatch = getProperty("WorkspaceToMatch");

  //First we need to create the parameter vector from the workspace with which we are matching
  std::vector<double> rb_params;
  createRebinParameters(toMatch, rb_params);

  IAlgorithm_sptr runRebin = createSubAlgorithm("Rebin");
  
  runRebin->setProperty<MatrixWorkspace_sptr>("InputWorkspace", toRebin);
  runRebin->setPropertyValue("OutputWorkspace", "rebin_out");
  runRebin->setProperty("params", rb_params);
  
  try
  {
    runRebin->execute();
  }
  catch (std::runtime_error&)
  {
    g_log.information("Unable to successfully run Rebin sub-algorithm");
    throw std::runtime_error("Error while executing Rebin as a sub algorithm.");
  }
  
  MatrixWorkspace_sptr ws = runRebin->getProperty("OutputWorkspace");

  setProperty("OutputWorkspace", ws);
}

/**
 * Create the vector of rebin parameters
 * @param toMatch A shared pointer to the workspace with the desired binning 
 * @param rb_params A vector to hold the rebin parameters once they have been calculated
 */
void RebinToWorkspace::createRebinParameters(MatrixWorkspace_sptr toMatch, std::vector<double> & rb_params)
{
  using namespace Mantid::API;

  const std::vector<double> & matchXdata = toMatch->readX(0);
  //params vector should have the form [x_1, delta_1,x_2, ... ,x_n-1,delta_n-1,x_n), see SimpleRebin.cpp
  rb_params.clear();
  int xsize = (int)matchXdata.size();
  for( int i = 0; i < xsize; ++i )
  {
    //bin bound
    rb_params.push_back(matchXdata[i]);
    //Bin width
    if( i < xsize - 1) rb_params.push_back(matchXdata[i + 1] - matchXdata[i]);
  }
}
