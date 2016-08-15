#ifndef MANTIDQTCUSTOMINTERFACES_ENGGDIFFRACTION_ENGGDIFFPEAKDIAGVIEW_H_
#define MANTIDQTCUSTOMINTERFACES_ENGGDIFFRACTION_ENGGDIFFPEAKDIAGVIEW_H_

// Interfaces
#include "MantidQtCustomInterfaces/EnggDiffraction/IEnggDiffPeakDiagView.h"
#include "MantidQtCustomInterfaces/EnggDiffraction/IEnggDiffPeakDiagPresenter.h"

// External headers
#include "MantidQtCustomInterfaces/DllConfig.h"

// UI files
#include "ui_EnggDiffPeakDiag.h"

// Std headers
#include <string>
#include <memory>

namespace MantidQt {
namespace CustomInterfaces {
/**
Qt-based view of the Peak Diagnostics GUI as part of the
Engineering Diffraction interface. Provides a concrete
implementation for the view interface(s) MVP pattern.

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
class MANTIDQT_CUSTOMINTERFACES_DLL EnggDiffPeakDiagView
	: public QWidget, public IEnggDiffPeakDiagView {

	//Qt macro for to allow it to generate MOC files
	Q_OBJECT

public:
  /// Constructor
  EnggDiffPeakDiagView(QWidget *parent = 0);

  /// Destructor
  ~EnggDiffPeakDiagView() override;

  /// Interface name
  static std::string name() { return "Peak Diagnostics"; };



private:

  /**
   * Initializes the window to be displayed
   */
  void initLayout() override;


  
private:
	/// Interface definition
	Ui::EnggDiffPeakDiag m_ui;

	/// Holds shared pointer to presenter
	std::shared_ptr<IEnggDiffPeakDiagPresenter> m_presenter;

	/// Sets up any signals and slots used by the interface
	void setupSignalsSlots();
};

} // namespace MantidQt
} // namespace customInterfaces

#endif // !MANTIDQTCUSTOMINTERFACES_ENGGDIFFRACTION_ENGGDIFFPEAKDIAGVIEW_H_
