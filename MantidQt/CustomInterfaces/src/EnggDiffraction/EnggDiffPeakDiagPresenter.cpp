#include "MantidQtCustomInterfaces/EnggDiffraction/EnggDiffPeakDiagPresenter.h"

namespace MantidQt {
namespace CustomInterfaces {

namespace {
Mantid::Kernel::Logger g_log("EngineeringDiffPeakDiagGUI");
}

/**
* Default constructor for the Peak Diagnostics presenter
*
* @param view The view to associate with this presenter
*/
EnggDiffPeakDiagPresenter::EnggDiffPeakDiagPresenter(
    IEnggDiffPeakDiagView *view)
    : m_view(view) {}

EnggDiffPeakDiagPresenter::~EnggDiffPeakDiagPresenter(){};

/**
 * Called from the view to notify presenter that something
 * has happened and call based on the user action
 *
 * @param notification The action performed by the user
 */
void EnggDiffPeakDiagPresenter::notify(
    const IEnggDiffPeakDiagPresenter::Notification notification) {
  // Typedef the notification
  using notif = IEnggDiffPeakDiagPresenter::Notification;

  switch (notification) {
  case notif::Start:
	  //TODO
    break;
  case notif::ShutDown:
	  //TODO
    break;
  }
}

} // namespace MantidQt
} // namespace CustomInterfaces