#include "MantidAlgorithms/SumSpectra.h"
#include "MantidAPI/CommonBinsValidator.h"
#include "MantidAPI/SpectrumInfo.h"
#include "MantidAPI/WorkspaceFactory.h"
#include "MantidDataObjects/RebinnedOutput.h"
#include "MantidGeometry/IDetector.h"
#include "MantidKernel/ArrayProperty.h"
#include "MantidKernel/BoundedValidator.h"

namespace Mantid {
namespace Algorithms {

// Register the class into the algorithm factory
DECLARE_ALGORITHM(SumSpectra)

using namespace Kernel;
using namespace API;
using namespace DataObjects;

SumSpectra::SumSpectra()
    : API::Algorithm(), m_outSpecNum(0), m_minWsInd(0), m_maxWsInd(0),
      m_keepMonitors(false), m_numberOfSpectra(0), m_yLength(0), m_indices(),
      m_calculateWeightedSum(false) {}

/** Initialisation method.
 *
 */
void SumSpectra::init() {
  declareProperty(make_unique<WorkspaceProperty<>>(
                      "InputWorkspace", "", Direction::Input,
                      boost::make_shared<CommonBinsValidator>()),
                  "The workspace containing the spectra to be summed.");
  declareProperty(
      make_unique<WorkspaceProperty<>>("OutputWorkspace", "",
                                       Direction::Output),
      "The name of the workspace to be created as the output of the algorithm. "
      " A workspace of this name will be created and stored in the Analysis "
      "Data Service.");

  auto mustBePositive = boost::make_shared<BoundedValidator<int>>();
  mustBePositive->setLower(0);
  declareProperty("StartWorkspaceIndex", 0, mustBePositive,
                  "The first Workspace index to be included in the summing");
  declareProperty("EndWorkspaceIndex", EMPTY_INT(), mustBePositive,
                  "The last Workspace index to be included in the summing");

  declareProperty(
      make_unique<Kernel::ArrayProperty<int>>("ListOfWorkspaceIndices"),
      "A list of workspace indices as a string with ranges, for "
      "example: 5-10,15,20-23. \n"
      "Optional: if not specified, then the "
      "Start/EndWorkspaceIndex fields are used alone. "
      "If specified, the range and the list are combined (without "
      "duplicating indices). For example, a range of 10 to 20 and "
      "a list '12,15,26,28' gives '10-20,26,28'.");

  declareProperty("IncludeMonitors", true,
                  "Whether to include monitor spectra in the summation.");

  declareProperty("WeightedSum", false,
                  "Instead of the usual spectra sum, calculate the weighted "
                  "sum. This has the form: \n"
                  ":math:`nSpectra "
                  "\\times\\Sigma(Signal_i/Error_i^2)/\\Sigma(1/Error_i^2)`\n "
                  "This property is ignored for event workspace.\n"
                  "The sums are defined for :math:`Error_i != 0` only, so the "
                  "values with zero error are dropped from the summation. To "
                  "estimate the number of dropped values see the "
                  "description. ");
}

/** Executes the algorithm
 *
 */
void SumSpectra::exec() {
  // Try and retrieve the optional properties
  m_minWsInd = getProperty("StartWorkspaceIndex");
  m_maxWsInd = getProperty("EndWorkspaceIndex");
  const std::vector<int> indices_list = getProperty("ListOfWorkspaceIndices");

  m_keepMonitors = getProperty("IncludeMonitors");

  // Get the input workspace
  MatrixWorkspace_const_sptr localworkspace = getProperty("InputWorkspace");

  m_numberOfSpectra = static_cast<int>(localworkspace->getNumberHistograms());
  this->m_yLength = static_cast<int>(localworkspace->blocksize());

  // Check 'StartSpectrum' is in range 0-m_numberOfSpectra
  if (m_minWsInd > m_numberOfSpectra) {
    g_log.warning("StartWorkspaceIndex out of range! Set to 0.");
    m_minWsInd = 0;
  }

  if (indices_list.empty()) {
    // If no list was given and no max, just do all.
    if (isEmpty(m_maxWsInd))
      m_maxWsInd = m_numberOfSpectra - 1;
  }

  // Something for m_maxWsIndex was given but it is out of range?
  if (!isEmpty(m_maxWsInd) &&
      (m_maxWsInd > m_numberOfSpectra - 1 || m_maxWsInd < m_minWsInd)) {
    g_log.warning("EndWorkspaceIndex out of range! Set to max Workspace Index");
    m_maxWsInd = m_numberOfSpectra;
  }

  // Make the set of indices to sum up from the list
  this->m_indices.insert(indices_list.begin(), indices_list.end());

  // And add the range too, if any
  if (!isEmpty(m_maxWsInd)) {
    for (int i = m_minWsInd; i <= m_maxWsInd; i++)
      this->m_indices.insert(i);
  }

  // determine the output spectrum number
  m_outSpecNum = this->getOutputSpecNo(localworkspace);
  g_log.information()
      << "Spectra remapping gives single spectra with spectra number: "
      << m_outSpecNum << "\n";

  m_calculateWeightedSum = getProperty("WeightedSum");

  EventWorkspace_const_sptr eventW =
      boost::dynamic_pointer_cast<const EventWorkspace>(localworkspace);
  if (eventW) {
    m_calculateWeightedSum = false;
    this->execEvent(eventW, this->m_indices);
  } else {
    //-------Workspace 2D mode -----

    // Create the 2D workspace for the output
    MatrixWorkspace_sptr outputWorkspace =
        API::WorkspaceFactory::Instance().create(
            localworkspace, 1, localworkspace->readX(m_minWsInd).size(),
            this->m_yLength);
    size_t numSpectra(0); // total number of processed spectra
    size_t numMasked(0);  // total number of the masked and skipped spectra
    size_t numZeros(0);   // number of spectra which have 0 value in the first
    // column (used in special cases of evaluating how good
    // Puasonian statistics is)

    Progress progress(this, 0, 1, this->m_indices.size());

    // This is the (only) output spectrum
    auto &outSpec = outputWorkspace->getSpectrum(0);

    // Copy over the bin boundaries
    outSpec.dataX() = localworkspace->readX(0);

    // Build a new spectra map
    outSpec.setSpectrumNo(m_outSpecNum);
    outSpec.clearDetectorIDs();

    if (localworkspace->id() == "RebinnedOutput") {
      this->doRebinnedOutput(outputWorkspace, progress, numSpectra, numMasked,
                             numZeros);
    } else {
      this->doWorkspace2D(localworkspace, outSpec, progress, numSpectra,
                          numMasked, numZeros);
    }

    // Pointer to sqrt function
    MantidVec &YError = outSpec.dataE();
    typedef double (*uf)(double);
    uf rs = std::sqrt;
    // take the square root of all the accumulated squared errors - Assumes
    // Gaussian errors
    std::transform(YError.begin(), YError.end(), YError.begin(), rs);

    // set up the summing statistics
    outputWorkspace->mutableRun().addProperty("NumAllSpectra", int(numSpectra),
                                              "", true);
    outputWorkspace->mutableRun().addProperty("NumMaskSpectra", int(numMasked),
                                              "", true);
    outputWorkspace->mutableRun().addProperty("NumZeroSpectra", int(numZeros),
                                              "", true);

    // Assign it to the output workspace property
    setProperty("OutputWorkspace", outputWorkspace);
  }
}

/**
 * Determine the minimum spectrum No for summing. This requires that
 * SumSpectra::indices has already been set.
 * @param localworkspace The workspace to use.
 * @return The minimum spectrum No for all the spectra being summed.
 */
specnum_t
SumSpectra::getOutputSpecNo(MatrixWorkspace_const_sptr localworkspace) {
  // initial value
  specnum_t specId =
      localworkspace->getSpectrum(*(this->m_indices.begin())).getSpectrumNo();

  // the total number of spectra
  int totalSpec = static_cast<int>(localworkspace->getNumberHistograms());

  specnum_t temp;
  for (const auto index : this->m_indices) {
    if (index < totalSpec) {
      temp = localworkspace->getSpectrum(index).getSpectrumNo();
      if (temp < specId)
        specId = temp;
    }
  }

  return specId;
}

/**
 * This function deals with the logic necessary for summing a Workspace2D.
 * @param localworkspace The input workspace for summing.
 * @param outSpec The spectrum for the summed output.
 * @param progress The progress indicator.
 * @param numSpectra The number of spectra contributed to the sum.
 * @param numMasked The spectra dropped from the summations because they are
 * masked.
 * @param numZeros The number of zero bins in histogram workspace or empty
 * spectra for event workspace.
 */
void SumSpectra::doWorkspace2D(MatrixWorkspace_const_sptr localworkspace,
                               ISpectrum &outSpec, Progress &progress,
                               size_t &numSpectra, size_t &numMasked,
                               size_t &numZeros) {
  // Get references to the output workspaces's data vectors
  MantidVec &YSum = outSpec.dataY();
  MantidVec &YError = outSpec.dataE();

  MantidVec Weight;
  std::vector<size_t> nZeros;
  if (m_calculateWeightedSum) {
    Weight.assign(YSum.size(), 0);
    nZeros.assign(YSum.size(), 0);
  }
  numSpectra = 0;
  numMasked = 0;
  numZeros = 0;

  const auto &spectrumInfo =
      localworkspace->spectrumInfo(ThreadedContextCheck::Skip);
  // Loop over spectra
  for (const auto i : this->m_indices) {
    // Don't go outside the range.
    if ((i >= this->m_numberOfSpectra) || (i < 0)) {
      g_log.error() << "Invalid index " << i
                    << " was specified. Sum was aborted.\n";
      break;
    }

    if (spectrumInfo.hasDetectors(i)) {
      // Skip monitors, if the property is set to do so
      if (!m_keepMonitors && spectrumInfo.isMonitor(i))
        continue;
      // Skip masked detectors
      if (spectrumInfo.isMasked(i)) {
        numMasked++;
        continue;
      }
    }
    numSpectra++;

    // Retrieve the spectrum into a vector
    const MantidVec &YValues = localworkspace->readY(i);
    const MantidVec &YErrors = localworkspace->readE(i);
    if (m_calculateWeightedSum) {
      for (int k = 0; k < this->m_yLength; ++k) {
        if (YErrors[k] != 0) {
          double errsq = YErrors[k] * YErrors[k];
          YError[k] += errsq;
          Weight[k] += 1. / errsq;
          YSum[k] += YValues[k] / errsq;
        } else {
          nZeros[k]++;
        }
      }
    } else {
      for (int k = 0; k < this->m_yLength; ++k) {
        YSum[k] += YValues[k];
        YError[k] += YErrors[k] * YErrors[k];
      }
    }

    // Map all the detectors onto the spectrum of the output
    outSpec.addDetectorIDs(localworkspace->getSpectrum(i).getDetectorIDs());

    progress.report();
  }

  if (m_calculateWeightedSum) {
    numZeros = 0;
    for (size_t i = 0; i < Weight.size(); i++) {
      if (numSpectra > nZeros[i])
        YSum[i] *= double(numSpectra - nZeros[i]) / Weight[i];
      if (nZeros[i] != 0)
        numZeros += nZeros[i];
    }
  }
}

/**
 * This function handles the logic for summing RebinnedOutput workspaces.
 * @param outputWorkspace the workspace to hold the summed input
 * @param progress the progress indicator
 * @param numSpectra
 * @param numMasked
 * @param numZeros
 */
void SumSpectra::doRebinnedOutput(MatrixWorkspace_sptr outputWorkspace,
                                  Progress &progress, size_t &numSpectra,
                                  size_t &numMasked, size_t &numZeros) {
  // Get a copy of the input workspace
  MatrixWorkspace_sptr temp = getProperty("InputWorkspace");

  // First, we need to clean the input workspace for nan's and inf's in order
  // to treat the data correctly later. This will create a new private
  // workspace that will be retrieved as mutable.
  IAlgorithm_sptr alg = this->createChildAlgorithm("ReplaceSpecialValues");
  alg->setProperty<MatrixWorkspace_sptr>("InputWorkspace", temp);
  std::string outName = "_" + temp->getName() + "_clean";
  alg->setProperty("OutputWorkspace", outName);
  alg->setProperty("NaNValue", 0.0);
  alg->setProperty("NaNError", 0.0);
  alg->setProperty("InfinityValue", 0.0);
  alg->setProperty("InfinityError", 0.0);
  alg->executeAsChildAlg();
  MatrixWorkspace_sptr localworkspace = alg->getProperty("OutputWorkspace");

  // Transform to real workspace types
  RebinnedOutput_sptr inWS =
      boost::dynamic_pointer_cast<RebinnedOutput>(localworkspace);
  RebinnedOutput_sptr outWS =
      boost::dynamic_pointer_cast<RebinnedOutput>(outputWorkspace);

  // Get references to the output workspaces's data vectors
  auto &outSpec = outputWorkspace->getSpectrum(0);
  MantidVec &YSum = outSpec.dataY();
  MantidVec &YError = outSpec.dataE();
  MantidVec &FracSum = outWS->dataF(0);
  MantidVec Weight;
  std::vector<size_t> nZeros;
  if (m_calculateWeightedSum) {
    Weight.assign(YSum.size(), 0);
    nZeros.assign(YSum.size(), 0);
  }
  numSpectra = 0;
  numMasked = 0;
  numZeros = 0;

  // Careful: SumSpectra is called in an OpenMP by other algorithms (on distinct
  // workspaces) so we manually disable the thread context check here. This will
  // go wrong if clients call SumSpectra on the same workspace in different
  // threads at the same time.
  const auto &spectrumInfo =
      localworkspace->spectrumInfo(ThreadedContextCheck::Skip);
  // Loop over spectra
  for (const auto i : m_indices) {
    // Don't go outside the range.
    if ((i >= m_numberOfSpectra) || (i < 0)) {
      g_log.error() << "Invalid index " << i
                    << " was specified. Sum was aborted.\n";
      break;
    }

    if (spectrumInfo.hasDetectors(i)) {
      // Skip monitors, if the property is set to do so
      if (!m_keepMonitors && spectrumInfo.isMonitor(i))
        continue;
      // Skip masked detectors
      if (spectrumInfo.isMasked(i)) {
        numMasked++;
        continue;
      }
    }
    numSpectra++;

    // Retrieve the spectrum into a vector
    const MantidVec &YValues = localworkspace->readY(i);
    const MantidVec &YErrors = localworkspace->readE(i);
    const MantidVec &FracArea = inWS->readF(i);

    if (m_calculateWeightedSum) {
      for (int k = 0; k < this->m_yLength; ++k) {
        if (YErrors[k] != 0) {
          double errsq = YErrors[k] * YErrors[k] * FracArea[k] * FracArea[k];
          YError[k] += errsq;
          Weight[k] += 1. / errsq;
          YSum[k] += YValues[k] * FracArea[k] / errsq;
          FracSum[k] += FracArea[k];
        } else {
          nZeros[k]++;
          FracSum[k] += FracArea[k];
        }
      }
    } else {
      for (int k = 0; k < this->m_yLength; ++k) {
        YSum[k] += YValues[k] * FracArea[k];
        YError[k] += YErrors[k] * YErrors[k] * FracArea[k] * FracArea[k];
        FracSum[k] += FracArea[k];
      }
    }

    // Map all the detectors onto the spectrum of the output
    outSpec.addDetectorIDs(localworkspace->getSpectrum(i).getDetectorIDs());

    progress.report();
  }

  if (m_calculateWeightedSum) {
    numZeros = 0;
    for (size_t i = 0; i < Weight.size(); i++) {
      if (numSpectra > nZeros[i])
        YSum[i] *= double(numSpectra - nZeros[i]) / Weight[i];
      if (nZeros[i] != 0)
        numZeros += nZeros[i];
    }
  }

  // Create the correct representation
  outWS->finalize();
}

/** Executes the algorithm
 *@param localworkspace :: the input workspace
 *@param indices :: set of indices to sum up
 */
void SumSpectra::execEvent(EventWorkspace_const_sptr localworkspace,
                           std::set<int> &indices) {
  // Make a brand new EventWorkspace
  EventWorkspace_sptr outputWorkspace =
      boost::dynamic_pointer_cast<EventWorkspace>(
          API::WorkspaceFactory::Instance().create("EventWorkspace", 1, 2, 1));
  // Copy geometry over.
  API::WorkspaceFactory::Instance().initializeFromParent(localworkspace,
                                                         outputWorkspace, true);

  Progress progress(this, 0, 1, indices.size());

  // Get the pointer to the output event list
  EventList &outEL = outputWorkspace->getSpectrum(0);
  outEL.setSpectrumNo(m_outSpecNum);
  outEL.clearDetectorIDs();

  const auto &spectrumInfo =
      localworkspace->spectrumInfo(ThreadedContextCheck::Skip);
  // Loop over spectra
  size_t numSpectra(0);
  size_t numMasked(0);
  size_t numZeros(0);
  for (const auto i : indices) {
    // Don't go outside the range.
    if ((i >= m_numberOfSpectra) || (i < 0)) {
      g_log.error() << "Invalid index " << i
                    << " was specified. Sum was aborted.\n";
      break;
    }

    if (spectrumInfo.hasDetectors(i)) {
      // Skip monitors, if the property is set to do so
      if (!m_keepMonitors && spectrumInfo.isMonitor(i))
        continue;
      // Skip masked detectors
      if (spectrumInfo.isMasked(i)) {
        numMasked++;
        continue;
      }
    }
    numSpectra++;

    // Add the event lists with the operator
    const EventList &tOutEL = localworkspace->getSpectrum(i);
    if (tOutEL.empty()) {
      ++numZeros;
    }
    outEL += tOutEL;

    progress.report();
  }

  // Set all X bins on the output
  outputWorkspace->setAllX(HistogramData::BinEdges(localworkspace->refX(0)));

  outputWorkspace->mutableRun().addProperty("NumAllSpectra", int(numSpectra),
                                            "", true);
  outputWorkspace->mutableRun().addProperty("NumMaskSpectra", int(numMasked),
                                            "", true);
  outputWorkspace->mutableRun().addProperty("NumZeroSpectra", int(numZeros), "",
                                            true);

  // Assign it to the output workspace property
  setProperty("OutputWorkspace",
              boost::dynamic_pointer_cast<MatrixWorkspace>(outputWorkspace));
}

} // namespace Algorithms
} // namespace Mantid
