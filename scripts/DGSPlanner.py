#pylint: disable=invalid-name,unused-import
import sys
from PyQt4 import QtGui
from DGSPlanner import DGSPlannerGUI

def qapp():
    if QtGui.QApplication.instance():
        _app = QtGui.QApplication.instance()
    else:
        _app = QtGui.QApplication(sys.argv)
    return _app

if __name__ == '__main__':
    app = qapp()
    planner = DGSPlannerGUI.DGSPlannerGUI()
    planner.show()
    try: #check if started from within mantidplot
        import mantidplot
    except ImportError:
        sys.exit(app.exec_())

