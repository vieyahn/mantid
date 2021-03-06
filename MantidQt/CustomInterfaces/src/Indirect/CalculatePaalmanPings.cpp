#include "MantidQtCustomInterfaces/Indirect/CalculatePaalmanPings.h"
#include "MantidQtCustomInterfaces/UserInputValidator.h"
#include "MantidAPI/AnalysisDataService.h"
#include "MantidAPI/Axis.h"
#include "MantidAPI/MatrixWorkspace.h"
#include "MantidGeometry/Instrument.h"
#include "MantidKernel/Unit.h"
#include "MantidKernel/Material.h"
#include "MantidQtMantidWidgets/WorkspaceSelector.h"

#include <QLineEdit>
#include <QList>
#include <QValidator>
#include <QDoubleValidator>
#include <QRegExpValidator>

using namespace Mantid::API;

namespace {
Mantid::Kernel::Logger g_log("CalculatePaalmanPings");
}

namespace MantidQt {
namespace CustomInterfaces {
CalculatePaalmanPings::CalculatePaalmanPings(QWidget *parent)
    : CorrectionsTab(parent) {
  m_uiForm.setupUi(parent);

  connect(m_uiForm.dsSample, SIGNAL(dataReady(const QString &)), this,
          SLOT(getBeamWidthFromWorkspace(const QString &)));

  QRegExp regex("[A-Za-z0-9\\-\\(\\)]*");
  QValidator *formulaValidator = new QRegExpValidator(regex, this);
  m_uiForm.leSampleChemicalFormula->setValidator(formulaValidator);
  m_uiForm.leCanChemicalFormula->setValidator(formulaValidator);
  connect(m_uiForm.leSampleChemicalFormula, SIGNAL(editingFinished()), this,
          SLOT(validateChemical()));
  connect(m_uiForm.leCanChemicalFormula, SIGNAL(editingFinished()), this,
          SLOT(validateChemical()));
  // Connect slots for plot and save
  connect(m_uiForm.pbSave, SIGNAL(clicked()), this, SLOT(saveClicked()));
  connect(m_uiForm.pbPlot, SIGNAL(clicked()), this, SLOT(plotClicked()));

  UserInputValidator uiv;
  if (uiv.checkFieldIsNotEmpty("Can Chemical Formula",
                               m_uiForm.leCanChemicalFormula,
                               m_uiForm.valCanChemicalFormula)) {
    uiv.checkFieldIsValid("Can Chemical Formula", m_uiForm.leCanChemicalFormula,
                          m_uiForm.valCanChemicalFormula);
  }
}

void CalculatePaalmanPings::setup() { doValidation(true); }

void CalculatePaalmanPings::validateChemical() { doValidation(true); }

void CalculatePaalmanPings::run() {
  // Get correct corrections algorithm
  auto sampleShape = m_uiForm.cbSampleShape->currentText();
  auto algorithmName = sampleShape.replace(" ", "") + "PaalmanPingsCorrection";
  algorithmName = algorithmName.replace(
      "Annulus", "Cylinder"); // Use the cylinder algorithm for annulus

  API::BatchAlgorithmRunner::AlgorithmRuntimeProps absCorProps;
  auto absCorAlgo =
      AlgorithmManager::Instance().create(algorithmName.toStdString());
  absCorAlgo->initialize();

  // Sample details
  auto sampleWsName = m_uiForm.dsSample->getCurrentDataName();
  MatrixWorkspace_sptr sampleWs =
      AnalysisDataService::Instance().retrieveWS<MatrixWorkspace>(
          sampleWsName.toStdString());

  // If not in wavelength then do conversion
  const auto sampleXUnit = sampleWs->getAxis(0)->unit();
  if (sampleXUnit->caption() != "Wavelength") {
    g_log.information(
        "Sample workspace not in wavelength, need to convert to continue.");
    absCorProps["SampleWorkspace"] =
        addConvertUnitsStep(sampleWs, "Wavelength");
  } else {
    absCorProps["SampleWorkspace"] = sampleWsName.toStdString();
  }

  absCorAlgo->setProperty("SampleNumberDensity",
                          m_uiForm.spSampleNumberDensity->value());

  absCorAlgo->setProperty(
      "SampleChemicalFormula",
      m_uiForm.leSampleChemicalFormula->text().toStdString());

  addShapeSpecificSampleOptions(absCorAlgo, sampleShape);

  // Can details
  if (m_uiForm.ckUseCan->isChecked()) {
    const auto canWsName =
        m_uiForm.dsContainer->getCurrentDataName().toStdString();
    MatrixWorkspace_sptr canWs =
        AnalysisDataService::Instance().retrieveWS<MatrixWorkspace>(canWsName);

    // If not in wavelength then do conversion
    Mantid::Kernel::Unit_sptr canXUnit = canWs->getAxis(0)->unit();
    if (canXUnit->caption() != "Wavelength") {
      g_log.information("Container workspace not in wavelength, need to "
                        "convert to continue.");
      absCorProps["CanWorkspace"] = addConvertUnitsStep(canWs, "Wavelength");
    } else {
      absCorProps["CanWorkspace"] = canWsName;
    }

    absCorAlgo->setProperty("CanNumberDensity",
                            m_uiForm.spCanNumberDensity->value());

    const auto canChemicalFormula = m_uiForm.leCanChemicalFormula->text();
    absCorAlgo->setProperty("CanChemicalFormula",
                            canChemicalFormula.toStdString());

    addShapeSpecificCanOptions(absCorAlgo, sampleShape);
  }

  const auto eMode = getEMode(sampleWs);
  absCorAlgo->setProperty("EMode", eMode);
  if (eMode == "Indirect")
    absCorAlgo->setProperty("EFixed", getEFixed(sampleWs));

  // Generate workspace names
  auto nameCutIndex = sampleWsName.lastIndexOf("_");
  if (nameCutIndex == -1)
    nameCutIndex = sampleWsName.length();

  QString correctionType;
  switch (m_uiForm.cbSampleShape->currentIndex()) {
  case 0:
    correctionType = "flt";
    break;
  case 1:
    correctionType = "cyl";
    break;
  case 2:
    correctionType = "ann";
    break;
  }

  const auto outputWsName =
      sampleWsName.left(nameCutIndex) + "_" + correctionType + "_abs";

  absCorAlgo->setProperty("OutputWorkspace", outputWsName.toStdString());

  // Add corrections algorithm to queue
  m_batchAlgoRunner->addAlgorithm(absCorAlgo, absCorProps);

  // Run algorithm queue
  connect(m_batchAlgoRunner, SIGNAL(batchComplete(bool)), this,
          SLOT(absCorComplete(bool)));
  m_batchAlgoRunner->executeBatchAsync();

  // Set the result workspace for Python script export
  m_pythonExportWsName = outputWsName.toStdString();
}

bool CalculatePaalmanPings::validate() { return doValidation(); }

/**
 * Does validation on the user input.
 *
 * @param silent Set to true to avoid creating an error message
 * @return True if all user input is valid
 */
bool CalculatePaalmanPings::doValidation(bool silent) {
  UserInputValidator uiv;

  uiv.checkDataSelectorIsValid("Sample", m_uiForm.dsSample);

  // Validate chemical formula
  if (uiv.checkFieldIsNotEmpty("Sample Chemical Formula",
                               m_uiForm.leSampleChemicalFormula,
                               m_uiForm.valSampleChemicalFormula))
    uiv.checkFieldIsValid("Sample Chemical Formula",
                          m_uiForm.leSampleChemicalFormula,
                          m_uiForm.valSampleChemicalFormula);
  const auto sampleChem =
      m_uiForm.leSampleChemicalFormula->text().toStdString();
  try {
    Mantid::Kernel::Material::parseChemicalFormula(sampleChem);
  } catch (std::runtime_error &ex) {
    UNUSED_ARG(ex);
    uiv.addErrorMessage("Chemical Formula for Sample was not recognised.");
    uiv.setErrorLabel(m_uiForm.valSampleChemicalFormula, false);
  }

  if (m_uiForm.ckUseCan->isChecked()) {
    uiv.checkDataSelectorIsValid("Can", m_uiForm.dsContainer);

    // Validate chemical formula
    if (uiv.checkFieldIsNotEmpty("Can Chemical Formula",
                                 m_uiForm.leCanChemicalFormula,
                                 m_uiForm.valCanChemicalFormula))
      uiv.checkFieldIsValid("Can Chemical Formula",
                            m_uiForm.leCanChemicalFormula,
                            m_uiForm.valCanChemicalFormula);

    const auto containerChem =
        m_uiForm.leCanChemicalFormula->text().toStdString();
    try {
      Mantid::Kernel::Material::parseChemicalFormula(containerChem);
    } catch (std::runtime_error &ex) {
      UNUSED_ARG(ex);
      uiv.addErrorMessage("Chemical Formula for Container was not recognised.");
      uiv.setErrorLabel(m_uiForm.valCanChemicalFormula, false);
    }

    // Ensure sample and container are the same kind of data
    auto sampleWsName = m_uiForm.dsSample->getCurrentDataName();
    const auto sampleType = sampleWsName.right(sampleWsName.length() -
                                               sampleWsName.lastIndexOf("_"));
    auto containerWsName = m_uiForm.dsContainer->getCurrentDataName();
    const auto containerType = containerWsName.right(
        containerWsName.length() - containerWsName.lastIndexOf("_"));

    g_log.debug() << "Sample type is: " << sampleType.toStdString() << '\n';
    g_log.debug() << "Can type is: " << containerType.toStdString() << '\n';

    if (containerType != sampleType)
      uiv.addErrorMessage(
          "Sample and can workspaces must contain the same type of data.");
  }

  // Show error message if needed
  if (!uiv.isAllInputValid() && !silent)
    emit showMessageBox(uiv.generateErrorMessage());

  return uiv.isAllInputValid();
}

/**
 * Handles completion of the correction algorithm.
 *
 * @param error True of the algorithm failed
 */
void CalculatePaalmanPings::absCorComplete(bool error) {
  disconnect(m_batchAlgoRunner, SIGNAL(batchComplete(bool)), this,
             SLOT(absCorComplete(bool)));

  if (error) {
    emit showMessageBox("Absorption correction calculation failed.\nSee "
                        "Results Log for more details.");
    return;
  }

  // Convert the spectrum axis of correction factors to Q
  const auto sampleWsName =
      m_uiForm.dsSample->getCurrentDataName().toStdString();
  MatrixWorkspace_sptr sampleWs =
      AnalysisDataService::Instance().retrieveWS<MatrixWorkspace>(sampleWsName);
  WorkspaceGroup_sptr corrections =
      AnalysisDataService::Instance().retrieveWS<WorkspaceGroup>(
          m_pythonExportWsName);
  for (size_t i = 0; i < corrections->size(); i++) {
    MatrixWorkspace_sptr factorWs =
        boost::dynamic_pointer_cast<MatrixWorkspace>(corrections->getItem(i));
    if (!factorWs || !sampleWs)
      continue;

    if (getEMode(sampleWs) == "Indirect") {
      API::BatchAlgorithmRunner::AlgorithmRuntimeProps convertSpecProps;
      IAlgorithm_sptr convertSpecAlgo =
          AlgorithmManager::Instance().create("ConvertSpectrumAxis");
      convertSpecAlgo->initialize();
      convertSpecAlgo->setProperty("InputWorkspace", factorWs);
      convertSpecAlgo->setProperty("OutputWorkspace", factorWs->name());
      convertSpecAlgo->setProperty("Target", "ElasticQ");
      convertSpecAlgo->setProperty("EMode", "Indirect");

      try {
        convertSpecAlgo->setProperty("EFixed", getEFixed(factorWs));
      } catch (std::runtime_error &) {
      }

      m_batchAlgoRunner->addAlgorithm(convertSpecAlgo);
    }
  }

  // Run algorithm queue
  connect(m_batchAlgoRunner, SIGNAL(batchComplete(bool)), this,
          SLOT(postProcessComplete(bool)));
  m_batchAlgoRunner->executeBatchAsync();
}

/**
 * Handles completion of the post processing algorithms.
 *
 * @param error True of the algorithm failed
 */
void CalculatePaalmanPings::postProcessComplete(bool error) {
  disconnect(m_batchAlgoRunner, SIGNAL(batchComplete(bool)), this,
             SLOT(postProcessComplete(bool)));

  if (error) {
    emit showMessageBox("Correction factor post processing failed.\nSee "
                        "Results Log for more details.");
    return;
  }

  // Enable post processing plot and save
  m_uiForm.cbPlotOutput->setEnabled(true);
  m_uiForm.pbPlot->setEnabled(true);
  m_uiForm.pbSave->setEnabled(true);
}

void CalculatePaalmanPings::loadSettings(const QSettings &settings) {
  m_uiForm.dsSample->readSettings(settings.group());
  m_uiForm.dsContainer->readSettings(settings.group());
}

/**
 * Gets the beam width from the instrument parameters on a given workspace
 * and update the relevant options on the UI.
 *
 * @param wsName Name of the workspace
 */
void CalculatePaalmanPings::getBeamWidthFromWorkspace(const QString &wsName) {
  auto ws = AnalysisDataService::Instance().retrieveWS<MatrixWorkspace>(
      wsName.toStdString());

  if (!ws) {
    g_log.warning() << "Failed to find workspace " << wsName.toStdString()
                    << '\n';
    return;
  }

  auto instrument = ws->getInstrument();

  const std::string beamWidthParamName = "Workflow.beam-width";
  if (instrument->hasParameter(beamWidthParamName)) {
    const auto beamWidth = QString::fromStdString(
        instrument->getStringParameter(beamWidthParamName)[0]);
    const auto beamWidthValue = beamWidth.toDouble();

    m_uiForm.spCylBeamWidth->setValue(beamWidthValue);
    m_uiForm.spAnnBeamWidth->setValue(beamWidthValue);
  }

  const std::string beamHeightParamName = "Workflow.beam-height";
  if (instrument->hasParameter(beamHeightParamName)) {
    const auto beamHeight = QString::fromStdString(
        instrument->getStringParameter(beamHeightParamName)[0]);
    const auto beamHeightValue = beamHeight.toDouble();

    m_uiForm.spCylBeamHeight->setValue(beamHeightValue);
    m_uiForm.spAnnBeamHeight->setValue(beamHeightValue);
  }
}

/**
 * Sets algorithm properties specific to the sample for a given shape.
 *
 * @param alg Algorithm to set properties of
 * @param shape Sample shape
 */
void CalculatePaalmanPings::addShapeSpecificSampleOptions(IAlgorithm_sptr alg,
                                                          QString shape) {
  if (shape == "FlatPlate") {
    const auto sampleThickness = m_uiForm.spFlatSampleThickness->value();
    alg->setProperty("SampleThickness", sampleThickness);

    const auto sampleAngle = m_uiForm.spFlatSampleAngle->value();
    alg->setProperty("SampleAngle", sampleAngle);
  } else if (shape == "Cylinder") {
    alg->setProperty("SampleInnerRadius", 0.0);

    const auto sampleOuterRadius = m_uiForm.spCylSampleOuterRadius->value();
    alg->setProperty("SampleOuterRadius", sampleOuterRadius);

    const auto beamWidth = m_uiForm.spCylBeamWidth->value();
    alg->setProperty("BeamWidth", beamWidth);

    const auto beamHeight = m_uiForm.spCylBeamHeight->value();
    alg->setProperty("BeamHeight", beamHeight);

    const auto stepSize = m_uiForm.spCylStepSize->value();
    alg->setProperty("StepSize", stepSize);
  } else if (shape == "Annulus") {
    const auto sampleInnerRadius = m_uiForm.spAnnSampleInnerRadius->value();
    alg->setProperty("SampleInnerRadius", sampleInnerRadius);

    const auto sampleOuterRadius = m_uiForm.spAnnSampleOuterRadius->value();
    alg->setProperty("SampleOuterRadius", sampleOuterRadius);

    const auto beamWidth = m_uiForm.spAnnBeamWidth->value();
    alg->setProperty("BeamWidth", beamWidth);

    const auto beamHeight = m_uiForm.spAnnBeamHeight->value();
    alg->setProperty("BeamHeight", beamHeight);

    const auto stepSize = m_uiForm.spAnnStepSize->value();
    alg->setProperty("StepSize", stepSize);
  }
}

/**
 * Sets algorithm properties specific to the container for a given shape.
 *
 * @param alg Algorithm to set properties of
 * @param shape Sample shape
 */
void CalculatePaalmanPings::addShapeSpecificCanOptions(IAlgorithm_sptr alg,
                                                       QString shape) {
  if (shape == "FlatPlate") {
    const auto canFrontThickness = m_uiForm.spFlatCanFrontThickness->value();
    alg->setProperty("CanFrontThickness", canFrontThickness);

    const auto canBackThickness = m_uiForm.spFlatCanBackThickness->value();
    alg->setProperty("SampleThickness", canBackThickness);
  } else if (shape == "Cylinder") {
    const auto canOuterRadius = m_uiForm.spCylCanOuterRadius->value();
    alg->setProperty("CanOuterRadius", canOuterRadius);
  } else if (shape == "Annulus") {
    const auto canOuterRadius = m_uiForm.spAnnCanOuterRadius->value();
    alg->setProperty("CanOuterRadius", canOuterRadius);
  }
}

/**
 * Handles saving of workspace
 */
void CalculatePaalmanPings::saveClicked() {

  if (checkADSForPlotSaveWorkspace(m_pythonExportWsName, false))
    addSaveWorkspaceToQueue(QString::fromStdString(m_pythonExportWsName));
  m_batchAlgoRunner->executeBatchAsync();
}

/**
 * Handles mantid plotting of workspace
 */
void CalculatePaalmanPings::plotClicked() {

  QString plotType = m_uiForm.cbPlotOutput->currentText();

  if (checkADSForPlotSaveWorkspace(m_pythonExportWsName, true)) {

    if (plotType == "Both" || plotType == "Wavelength")
      plotSpectrum(QString::fromStdString(m_pythonExportWsName));

    if (plotType == "Both" || plotType == "Angle")
      plotTimeBin(QString::fromStdString(m_pythonExportWsName));
  }
}
} // namespace CustomInterfaces
} // namespace MantidQt
