#ifndef MANTIDQTCUSTOMINTERFACES_ENGGDIFFRACTION_IENGGDIFFFITTINGVIEWQTWIDGET_H_
#define MANTIDQTCUSTOMINTERFACES_ENGGDIFFRACTION_IENGGDIFFFITTINGVIEWQTWIDGET_H_

#include "MantidAPI/IPeakFunction.h"
#include "MantidQtCustomInterfaces/DllConfig.h"
#include "MantidQtCustomInterfaces/EnggDiffraction/IEnggDiffFittingPresenter.h"
#include "MantidQtCustomInterfaces/EnggDiffraction/IEnggDiffFittingView.h"

#include "ui_EnggDiffractionQtTabFitting.h"

#include <boost/scoped_ptr.hpp>

// Qt classes forward declarations
class QMessageBox;
class QMutex;

class QwtPlotCurve;
class QwtPlotZoomer;

namespace MantidQt {

namespace MantidWidgets {
class PeakPicker;
}

namespace CustomInterfaces {

/**
Qt-based view of the Engineering Diffraction (EnggDiff) fitting
widget/tab. Provides a concrete view that is Qt-based and is probably
the only one that will be implemented in a foreseeable horizon. The
interface of this class is given by IEnggDiffFittingView so that it
fits in the MVP (Model-View-Presenter) design of this GUI.

Copyright &copy; 2016 ISIS Rutherford Appleton Laboratory, NScD
Oak Ridge National Laboratory & European Spallation Source

This file is part of Mantid.

Mantid is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.

Mantid is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

File change history is stored at: <https://github.com/mantidproject/mantid>
Code Documentation is available at: <http://doxygen.mantidproject.org>
*/
class MANTIDQT_CUSTOMINTERFACES_DLL EnggDiffFittingViewQtWidget
    : public QWidget,
      public IEnggDiffFittingView {
  Q_OBJECT

public:
  EnggDiffFittingViewQtWidget(
      QWidget *parent, boost::shared_ptr<IEnggDiffractionUserMsg> mainMsg,
      boost::shared_ptr<IEnggDiffractionSettings> mainSettings,
      boost::shared_ptr<IEnggDiffractionCalibration> mainCalib,
      boost::shared_ptr<IEnggDiffractionParam> mainParam,
      boost::shared_ptr<IEnggDiffractionPythonRunner> mainPyhonRunner);
  ~EnggDiffFittingViewQtWidget() override;

  /// From the IEnggDiffractionUserMsg interface
  //@{
  void showStatus(const std::string &sts) override;

  void userWarning(const std::string &warn,
                   const std::string &description) override;

  void userError(const std::string &err,
                 const std::string &description) override;
  void enableCalibrateFocusFitUserActions(bool enable) override;
  //@}

  /// From the IEnggDiffractionSettings interface
  //@{
  EnggDiffCalibSettings currentCalibSettings() const override;

  std::string focusingDir() const override;
  //@}

  /// From the IEnggDiffractionPythonRunner interface
  //@{
  virtual std::string enggRunPythonCode(const std::string &pyCode) override;
  //@}

  void enable(bool enable);

  std::vector<std::string> logMsgs() const override { return m_logMsgs; }

  void setFittingRunNo(const std::string &path) override;

  std::string getFittingRunNo() const override;

  void enableFitAllButton(bool enable) const override;

  void clearFittingComboBox() const override;

  void enableFittingComboBox(bool enable) const override;

  int getFittingComboIdx(std::string bank) const override;

  void clearFittingListWidget() const override;

  void enableFittingListWidget(bool enable) const override;

  int getFittingListWidgetCurrentRow() const override;

  void setFittingListWidgetCurrentRow(int idx) const override;

  std::string fittingPeaksData() const override;

  void setPeakList(const std::string &peakList) const override;

  void setBankEmit() override;

  void resetCanvas() override;

  void setDataVector(std::vector<boost::shared_ptr<QwtData>> &data,
                     bool focused, bool plotSinglePeaks) override;

  void addBankItem(std::string bankID) override;

  void addRunNoItem(std::string runNo) override;

  std::vector<std::string> getFittingRunNumVec() override;

  void setFittingRunNumVec(std::vector<std::string> assignVec) override;

  bool getFittingMultiRunMode() override;

  void setFittingMultiRunMode(bool mode) override;

  bool getFittingSingleRunMode() override;

  void setFittingSingleRunMode(bool mode) override;

  double getPeakCentre() const override;

  bool peakPickerEnabled() const override;

  std::string getPreviousDir() const override;

  void setPreviousDir(const std::string &path) override;

  std::string getOpenFile(const std::string &prevPath) override;

  std::string getSaveFile(const std::string &prevPath) override;

  void dataCurvesFactory(std::vector<boost::shared_ptr<QwtData>> &data,
                         std::vector<QwtPlotCurve *> &dataVector, bool focused);

  void setPeakPickerEnabled(bool enabled);

  void setPeakPicker(const Mantid::API::IPeakFunction_const_sptr &peak);

  void setZoomTool(bool enabled);

  void resetView();

protected:
  void initLayout();

signals:
  void getBanks();
  void setBank();

private slots:
  // slot of the fitting peaks per part of the interface
  void browseFitFocusedRun();
  void resetFittingMode();
  void setBankIdComboBox(int idx) override;
  void setPeakPick();
  void clearPeakList();
  void loadClicked();
  void fitClicked();
  void fitAllClicked();
  void FittingRunNo();
  void addClicked();
  void browseClicked();
  void saveClicked();
  void plotSeparateWindow();
  void showToolTipHelp();
  void setBankDir(int idx);
  void listViewFittingRun();

private:
  /// Setup the interface (tab UI)
  void doSetup();

  /// Load saved/default interface settings
  void readSettings();
  /// save settings (before closing)
  void saveSettings() const override;

  /// converts QList to a vector
  std::vector<std::string> qListToVector(QStringList list,
                                         bool validator) const;

  // path/name for the persistent settings group of this interface
  static const std::string g_settingsGroup;

  static const std::string g_peaksListExt;

  /// indentifier for fitting multi-run or single run input
  static bool m_fittingMutliRunMode;

  /// indentifier for fitting multi-run or single run input
  static bool m_fittingSingleRunMode;

  // vector holding directory of focused bank file
  static std::vector<std::string> m_fitting_runno_dir_vec;

  Ui::EnggDiffractionQtTabFitting m_ui;

  // here the view puts messages before notifying the presenter to show them
  std::vector<std::string> m_logMsgs;

  /// Loaded focused workspace
  std::vector<QwtPlotCurve *> m_focusedDataVector;

  /// Loaded data curves
  std::vector<QwtPlotCurve *> m_fittedDataVector;

  /// Peak picker tool for fitting - only one on the plot at any given moment
  MantidWidgets::PeakPicker *m_peakPicker = nullptr;

  /// zoom-in/zoom-out tool for fitting
  QwtPlotZoomer *m_zoomTool = nullptr;

  /// user messages interface provided by a main view/widget
  boost::shared_ptr<IEnggDiffractionUserMsg> m_mainMsgProvider;

  /// settings from the user
  boost::shared_ptr<IEnggDiffractionSettings> m_mainSettings;

  /// interface for the Python runner
  boost::shared_ptr<IEnggDiffractionPythonRunner> m_mainPythonRunner;

  /// presenter as in the model-view-presenter
  boost::scoped_ptr<IEnggDiffFittingPresenter> m_presenter;
};

} // namespace CustomInterfaces
} // namespace MantidQt

#endif // MANTIDQTCUSTOMINTERFACES_ENGGDIFFRACTION_IENGGDIFFFITTINGVIEWQTWIDGET_H_
