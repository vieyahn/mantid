#ifndef MANTIDQTCUSTOMINTERFACES_ENGGDIFFRACTION_IENGGDIFFPEAKDIAGPRESENTER_H_
#define MANTIDQTCUSTOMINTERFACES_ENGGDIFFRACTION_IENGGDIFFPEAKDIAGPRESENTER_H_

namespace MantidQt {
namespace CustomInterfaces {

/** 
Implements an abstract presenter interface as part of the MVP pattern
for the peak diagnostics window. This is part of the Engineering Diffraction custom interface. 

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

class IEnggDiffPeakDiagPresenter {

public:
	virtual ~IEnggDiffPeakDiagPresenter() = default;

	/// Contains all accepted notification flags triggered by the view
	/// for the presenter to act upon
	enum class Notification {
		// TODO fill with any and all window notifcations
		Start,
		ShutDown
	};

	/**
	 * Interface for the view to notify the presenter about any changes
	 * such as the user clicking on something.
	 * 
	 * @param notification Flag indicating what action has been performed
	 */
	virtual void notify(IEnggDiffPeakDiagPresenter::Notification notification) = 0;

};


} // namespace CustomInterfaces
} // namespace MantidQt


#endif // MANTIDQTCUSTOMINTERFACES_ENGGDIFFRACTION_IENGGDIFFPEAKDIAGPRESENTER_H_
