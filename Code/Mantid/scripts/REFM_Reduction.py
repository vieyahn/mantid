"""
    Script used to start the REFL reduction gui from Mantidplot
"""
from Interface.reduction_application import ReductionGUI
from PyQt4 import QtCore, uic

reducer = ReductionGUI(instrument="REFM", instrument_list=["REFM"])
reducer.setup_layout(load_last=True)
reducer.show()