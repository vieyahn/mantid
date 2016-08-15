#ifndef MANTIDQTCUSTOMINTERFACES_ENGGDIFF_ENGGDIFFPEAKDIAGPRESENTER_H_
#define MANTIDQTCUSTOMINTERFACES_ENGGDIFF_ENGGDIFFPEAKDIAGPRESENTER_H_

#include "MantidQtCustomInterfaces/EnggDiffraction/IEnggDiffPeakDiagPresenter.h"
#include "MantidQtCustomInterfaces/EnggDiffraction/IEnggDiffPeakDiagView.h"
#include "MantidQtCustomInterfaces/DllConfig.h"

#include <Qobject>

namespace MantidQt {
namespace CustomInterfaces {

/**
Presenter as part of the MVP pattern for the peak diagnostics window and widgets 
found within the Engineering Diffraction custom interface

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

class MANTIDQT_CUSTOMINTERFACES_DLL EnggDiffPeakDiagPresenter :
		public QObject,
		public IEnggDiffPeakDiagPresenter {

public:

	EnggDiffPeakDiagPresenter(IEnggDiffPeakDiagView *view);

	~EnggDiffPeakDiagPresenter() override;

	void notify(const IEnggDiffPeakDiagPresenter::Notification notification) override;


private:
	/// The view for this presenter (MVP)
	IEnggDiffPeakDiagView *const m_view;

};

} // namespace MantidQt
} // namespace CustomInterfaces


#endif // MANTIDQTCUSTOMINTERFACES_ENGGDIFF_ENGGDIFFPEAKDIAGPRESENTER_H_