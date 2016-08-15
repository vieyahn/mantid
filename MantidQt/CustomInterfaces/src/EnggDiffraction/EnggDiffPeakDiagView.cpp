#include "MantidQtCustomInterfaces/EnggDiffraction/EnggDiffPeakDiagView.h"
#include "MantidQtCustomInterfaces/EnggDiffraction/IEnggDiffFittingView.h"

namespace MantidQt {
namespace CustomInterfaces {

	EnggDiffPeakDiagView::EnggDiffPeakDiagView(QWidget *parent)
	: IEnggDiffPeakDiagView() {
		//Todo
	}

	EnggDiffPeakDiagView::~EnggDiffPeakDiagView() {}

	void EnggDiffPeakDiagView::initLayout() {
		// Call Qt to setup the Ui for us
		m_ui.setupUi(this);

		// Next create a presenter to handle view
		m_presenter = std::make_shared<IEnggDiffPeakDiagPresenter>();



	}

	void EnggDiffPeakDiagView::setupSignalsSlots()
	{
		//TODO setup signals and slots
	}



} // namespace MantidQt
} // namespace CustomInterfaces