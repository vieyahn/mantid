#include "MantidQtMantidWidgets/InstrumentView/InstrumentWidgetTreeTab.h"
#include "MantidQtAPI/TSVSerialiser.h"
#include "MantidQtMantidWidgets/InstrumentView/GLActorVisitor.h"
#include "MantidQtMantidWidgets/InstrumentView/InstrumentActor.h"
#include "MantidQtMantidWidgets/InstrumentView/InstrumentTreeWidget.h"
#include "MantidQtMantidWidgets/InstrumentView/InstrumentWidget.h"
#include "MantidQtMantidWidgets/InstrumentView/ProjectionSurface.h"

#include <QVBoxLayout>
#include <QMessageBox>

namespace MantidQt {
namespace MantidWidgets {

InstrumentWidgetTreeTab::InstrumentWidgetTreeTab(InstrumentWidget *instrWidget)
    : InstrumentWidgetTab(instrWidget) {
  QVBoxLayout *layout = new QVBoxLayout(this);
  // Tree Controls
  m_instrumentTree = new InstrumentTreeWidget(0);
  layout->addWidget(m_instrumentTree);
  connect(m_instrumentTree,
          SIGNAL(componentSelected(Mantid::Geometry::ComponentID)),
          m_instrWidget,
          SLOT(componentSelected(Mantid::Geometry::ComponentID)));
  connect(m_instrWidget, SIGNAL(requestSelectComponent(QString)), this,
          SLOT(selectComponentByName(QString)));
}

void InstrumentWidgetTreeTab::initSurface() {
  m_instrumentTree->setInstrumentActor(m_instrWidget->getInstrumentActor());
}

/**
        * Find an instrument component by its name. This is used from the
        * scripting API and errors (component not found) are shown as a
        * message box in the GUI.
        *
        * @param name :: Name of an instrument component.
        */
void InstrumentWidgetTreeTab::selectComponentByName(const QString &name) {
  QModelIndex component = m_instrumentTree->findComponentByName(name);
  if (!component.isValid()) {
    QMessageBox::warning(this, "Instrument Window - Tree Tab - Error",
                         "No component named '" + name +
                             "' was found. "
                             "Please use a valid component name ");
    return;
  }

  m_instrumentTree->clearSelection();
  m_instrumentTree->scrollTo(component, QAbstractItemView::EnsureVisible);
  m_instrumentTree->selectionModel()->select(component,
                                             QItemSelectionModel::Select);
  m_instrumentTree->sendComponentSelectedSignal(component);
}

/**
        * Update surface when tab becomes visible.
        */
void InstrumentWidgetTreeTab::showEvent(QShowEvent *) {
  getSurface()->setInteractionMode(ProjectionSurface::MoveMode);
}

/** Load tree tab state from a Mantid project file
 * @param lines :: lines from the project file to load state from
 */
void InstrumentWidgetTreeTab::loadFromProject(const std::string &lines) {
  TSVSerialiser tsv(lines);

  if (!tsv.selectSection("treetab"))
    return;

  std::string tabLines;
  tsv >> tabLines;
  TSVSerialiser tab(tabLines);

  std::string componentName;
  if (tab.selectLine("SelectedComponent")) {
    tab >> componentName;
    selectComponentByName(QString::fromStdString(componentName));
  }
}

/** Save the state of the tree tab to a Mantid project file
 * @return a string representing the state of the tree tab
 */
std::string InstrumentWidgetTreeTab::saveToProject() const {
  TSVSerialiser tsv;
  TSVSerialiser tab;

  auto index = m_instrumentTree->currentIndex();
  auto model = index.model();

  if (model) {
    auto item = model->data(index);
    auto name = item.value<QString>();
    tab.writeLine("SelectedComponent") << name;
  }

  tsv.writeSection("treetab", tab.outputLines());
  return tsv.outputLines();
}

} // MantidWidgets
} // MantidQt
