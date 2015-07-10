#pylint: disable=invalid-name,attribute-defined-outside-init,line-too-long,too-many-instance-attributes,too-many-statements
# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'ui/inelastic/dgs_data_corrections.ui'
#
# Created: Mon Oct 22 16:27:15 2012
#      by: PyQt4 UI code generator 4.9.1
#
# WARNING! All changes made in this file will be lost!

from PyQt4 import QtCore, QtGui

try:
    _fromUtf8 = QtCore.QString.fromUtf8
except AttributeError:
    _fromUtf8 = lambda s: s

class Ui_DataCorrsFrame(object):
    def setupUi(self, DataCorrsFrame):
        DataCorrsFrame.setObjectName(_fromUtf8("DataCorrsFrame"))
        DataCorrsFrame.resize(834, 591)
        DataCorrsFrame.setFrameShape(QtGui.QFrame.StyledPanel)
        DataCorrsFrame.setFrameShadow(QtGui.QFrame.Raised)
        self.verticalLayout_2 = QtGui.QVBoxLayout(DataCorrsFrame)
        self.verticalLayout_2.setObjectName(_fromUtf8("verticalLayout_2"))
        self.horizontalLayout = QtGui.QHBoxLayout()
        self.horizontalLayout.setObjectName(_fromUtf8("horizontalLayout"))
        self.filter_bad_pulses_chkbox = QtGui.QCheckBox(DataCorrsFrame)
        self.filter_bad_pulses_chkbox.setEnabled(False)
        self.filter_bad_pulses_chkbox.setObjectName(_fromUtf8("filter_bad_pulses_chkbox"))
        self.horizontalLayout.addWidget(self.filter_bad_pulses_chkbox)
        spacerItem = QtGui.QSpacerItem(40, 20, QtGui.QSizePolicy.Expanding, QtGui.QSizePolicy.Minimum)
        self.horizontalLayout.addItem(spacerItem)
        self.verticalLayout_2.addLayout(self.horizontalLayout)
        self.horizontalLayout_2 = QtGui.QHBoxLayout()
        self.horizontalLayout_2.setObjectName(_fromUtf8("horizontalLayout_2"))
        self.incident_beam_norm_gb = QtGui.QGroupBox(DataCorrsFrame)
        self.incident_beam_norm_gb.setObjectName(_fromUtf8("incident_beam_norm_gb"))
        self.gridLayout = QtGui.QGridLayout(self.incident_beam_norm_gb)
        self.gridLayout.setObjectName(_fromUtf8("gridLayout"))
        self.none_rb = QtGui.QRadioButton(self.incident_beam_norm_gb)
        self.none_rb.setMinimumSize(QtCore.QSize(100, 0))
        self.none_rb.setChecked(True)
        self.none_rb.setObjectName(_fromUtf8("none_rb"))
        self.gridLayout.addWidget(self.none_rb, 0, 0, 1, 1)
        self.current_rb = QtGui.QRadioButton(self.incident_beam_norm_gb)
        self.current_rb.setObjectName(_fromUtf8("current_rb"))
        self.gridLayout.addWidget(self.current_rb, 0, 1, 1, 2)
        self.monint_label = QtGui.QLabel(self.incident_beam_norm_gb)
        self.monint_label.setMinimumSize(QtCore.QSize(130, 0))
        self.monint_label.setAlignment(QtCore.Qt.AlignRight|QtCore.Qt.AlignTrailing|QtCore.Qt.AlignVCenter)
        self.monint_label.setObjectName(_fromUtf8("monint_label"))
        self.gridLayout.addWidget(self.monint_label, 1, 1, 1, 1)
        self.monint_low_edit = QtGui.QLineEdit(self.incident_beam_norm_gb)
        self.monint_low_edit.setObjectName(_fromUtf8("monint_low_edit"))
        self.gridLayout.addWidget(self.monint_low_edit, 1, 2, 1, 1)
        self.monint_high_edit = QtGui.QLineEdit(self.incident_beam_norm_gb)
        self.monint_high_edit.setObjectName(_fromUtf8("monint_high_edit"))
        self.gridLayout.addWidget(self.monint_high_edit, 1, 4, 1, 1)
        self.monitor1_rb = QtGui.QRadioButton(self.incident_beam_norm_gb)
        self.monitor1_rb.setMinimumSize(QtCore.QSize(100, 0))
        self.monitor1_rb.setObjectName(_fromUtf8("monitor1_rb"))
        self.gridLayout.addWidget(self.monitor1_rb, 1, 0, 1, 1)
        self.label = QtGui.QLabel(self.incident_beam_norm_gb)
        self.label.setMinimumSize(QtCore.QSize(60, 0))
        self.label.setAlignment(QtCore.Qt.AlignCenter)
        self.label.setObjectName(_fromUtf8("label"))
        self.gridLayout.addWidget(self.label, 1, 3, 1, 1)
        self.horizontalLayout_2.addWidget(self.incident_beam_norm_gb)
        spacerItem1 = QtGui.QSpacerItem(40, 20, QtGui.QSizePolicy.Expanding, QtGui.QSizePolicy.Minimum)
        self.horizontalLayout_2.addItem(spacerItem1)
        self.verticalLayout_2.addLayout(self.horizontalLayout_2)
        self.horizontalLayout_4 = QtGui.QHBoxLayout()
        self.horizontalLayout_4.setObjectName(_fromUtf8("horizontalLayout_4"))
        self.background_sub_gb = QtGui.QGroupBox(DataCorrsFrame)
        self.background_sub_gb.setCheckable(True)
        self.background_sub_gb.setChecked(False)
        self.background_sub_gb.setObjectName(_fromUtf8("background_sub_gb"))
        self.horizontalLayout_3 = QtGui.QHBoxLayout(self.background_sub_gb)
        self.horizontalLayout_3.setObjectName(_fromUtf8("horizontalLayout_3"))
        self.tof_start_label = QtGui.QLabel(self.background_sub_gb)
        self.tof_start_label.setMinimumSize(QtCore.QSize(236, 0))
        self.tof_start_label.setAlignment(QtCore.Qt.AlignRight|QtCore.Qt.AlignTrailing|QtCore.Qt.AlignVCenter)
        self.tof_start_label.setObjectName(_fromUtf8("tof_start_label"))
        self.horizontalLayout_3.addWidget(self.tof_start_label)
        self.tof_start_edit = QtGui.QLineEdit(self.background_sub_gb)
        self.tof_start_edit.setObjectName(_fromUtf8("tof_start_edit"))
        self.horizontalLayout_3.addWidget(self.tof_start_edit)
        self.tof_end_label = QtGui.QLabel(self.background_sub_gb)
        self.tof_end_label.setMinimumSize(QtCore.QSize(60, 0))
        self.tof_end_label.setObjectName(_fromUtf8("tof_end_label"))
        self.horizontalLayout_3.addWidget(self.tof_end_label)
        self.tof_end_edit = QtGui.QLineEdit(self.background_sub_gb)
        self.tof_end_edit.setObjectName(_fromUtf8("tof_end_edit"))
        self.horizontalLayout_3.addWidget(self.tof_end_edit)
        self.horizontalLayout_4.addWidget(self.background_sub_gb)
        spacerItem2 = QtGui.QSpacerItem(40, 20, QtGui.QSizePolicy.Expanding, QtGui.QSizePolicy.Minimum)
        self.horizontalLayout_4.addItem(spacerItem2)
        self.verticalLayout_2.addLayout(self.horizontalLayout_4)
        self.correct_kikf_cb = QtGui.QCheckBox(DataCorrsFrame)
        self.correct_kikf_cb.setChecked(True)
        self.correct_kikf_cb.setObjectName(_fromUtf8("correct_kikf_cb"))
        self.verticalLayout_2.addWidget(self.correct_kikf_cb)
        self.det_van_gb = QtGui.QGroupBox(DataCorrsFrame)
        self.det_van_gb.setObjectName(_fromUtf8("det_van_gb"))
        self.verticalLayout = QtGui.QVBoxLayout(self.det_van_gb)
        self.verticalLayout.setObjectName(_fromUtf8("verticalLayout"))
        self.horizontalLayout_5 = QtGui.QHBoxLayout()
        self.horizontalLayout_5.setObjectName(_fromUtf8("horizontalLayout_5"))
        self.van_input_label = QtGui.QLabel(self.det_van_gb)
        self.van_input_label.setObjectName(_fromUtf8("van_input_label"))
        self.horizontalLayout_5.addWidget(self.van_input_label)
        self.van_input_edit = QtGui.QLineEdit(self.det_van_gb)
        self.van_input_edit.setObjectName(_fromUtf8("van_input_edit"))
        self.horizontalLayout_5.addWidget(self.van_input_edit)
        self.van_input_browse = QtGui.QPushButton(self.det_van_gb)
        self.van_input_browse.setObjectName(_fromUtf8("van_input_browse"))
        self.horizontalLayout_5.addWidget(self.van_input_browse)
        spacerItem3 = QtGui.QSpacerItem(19, 20, QtGui.QSizePolicy.Fixed, QtGui.QSizePolicy.Minimum)
        self.horizontalLayout_5.addItem(spacerItem3)
        self.verticalLayout.addLayout(self.horizontalLayout_5)
        self.horizontalLayout_8 = QtGui.QHBoxLayout()
        self.horizontalLayout_8.setObjectName(_fromUtf8("horizontalLayout_8"))
        self.use_procdetvan_cb = QtGui.QCheckBox(self.det_van_gb)
        self.use_procdetvan_cb.setObjectName(_fromUtf8("use_procdetvan_cb"))
        self.horizontalLayout_8.addWidget(self.use_procdetvan_cb)
        spacerItem4 = QtGui.QSpacerItem(40, 20, QtGui.QSizePolicy.Expanding, QtGui.QSizePolicy.Minimum)
        self.horizontalLayout_8.addItem(spacerItem4)
        self.verticalLayout.addLayout(self.horizontalLayout_8)
        self.horizontalLayout_6 = QtGui.QHBoxLayout()
        self.horizontalLayout_6.setObjectName(_fromUtf8("horizontalLayout_6"))
        self.van_int_cb = QtGui.QCheckBox(self.det_van_gb)
        self.van_int_cb.setMinimumSize(QtCore.QSize(138, 0))
        self.van_int_cb.setObjectName(_fromUtf8("van_int_cb"))
        self.horizontalLayout_6.addWidget(self.van_int_cb)
        self.van_int_range_label = QtGui.QLabel(self.det_van_gb)
        self.van_int_range_label.setMinimumSize(QtCore.QSize(78, 0))
        self.van_int_range_label.setAlignment(QtCore.Qt.AlignRight|QtCore.Qt.AlignTrailing|QtCore.Qt.AlignVCenter)
        self.van_int_range_label.setObjectName(_fromUtf8("van_int_range_label"))
        self.horizontalLayout_6.addWidget(self.van_int_range_label)
        self.van_int_range_low_edit = QtGui.QLineEdit(self.det_van_gb)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Fixed, QtGui.QSizePolicy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.van_int_range_low_edit.sizePolicy().hasHeightForWidth())
        self.van_int_range_low_edit.setSizePolicy(sizePolicy)
        self.van_int_range_low_edit.setObjectName(_fromUtf8("van_int_range_low_edit"))
        self.horizontalLayout_6.addWidget(self.van_int_range_low_edit)
        self.label_3 = QtGui.QLabel(self.det_van_gb)
        self.label_3.setMinimumSize(QtCore.QSize(60, 0))
        self.label_3.setAlignment(QtCore.Qt.AlignCenter)
        self.label_3.setObjectName(_fromUtf8("label_3"))
        self.horizontalLayout_6.addWidget(self.label_3)
        self.van_int_range_high_edit = QtGui.QLineEdit(self.det_van_gb)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Fixed, QtGui.QSizePolicy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.van_int_range_high_edit.sizePolicy().hasHeightForWidth())
        self.van_int_range_high_edit.setSizePolicy(sizePolicy)
        self.van_int_range_high_edit.setObjectName(_fromUtf8("van_int_range_high_edit"))
        self.horizontalLayout_6.addWidget(self.van_int_range_high_edit)
        self.van_int_range_units_cb = QtGui.QComboBox(self.det_van_gb)
        self.van_int_range_units_cb.setMinimumSize(QtCore.QSize(110, 0))
        self.van_int_range_units_cb.setObjectName(_fromUtf8("van_int_range_units_cb"))
        self.van_int_range_units_cb.addItem(_fromUtf8(""))
        self.van_int_range_units_cb.addItem(_fromUtf8(""))
        self.van_int_range_units_cb.addItem(_fromUtf8(""))
        self.horizontalLayout_6.addWidget(self.van_int_range_units_cb)
        spacerItem5 = QtGui.QSpacerItem(40, 20, QtGui.QSizePolicy.Expanding, QtGui.QSizePolicy.Minimum)
        self.horizontalLayout_6.addItem(spacerItem5)
        self.verticalLayout.addLayout(self.horizontalLayout_6)
        self.horizontalLayout_7 = QtGui.QHBoxLayout()
        self.horizontalLayout_7.setObjectName(_fromUtf8("horizontalLayout_7"))
        self.save_procdetvan_cb = QtGui.QCheckBox(self.det_van_gb)
        self.save_procdetvan_cb.setMinimumSize(QtCore.QSize(138, 0))
        self.save_procdetvan_cb.setObjectName(_fromUtf8("save_procdetvan_cb"))
        self.horizontalLayout_7.addWidget(self.save_procdetvan_cb)
        self.save_procdetvan_label = QtGui.QLabel(self.det_van_gb)
        self.save_procdetvan_label.setMinimumSize(QtCore.QSize(78, 0))
        self.save_procdetvan_label.setObjectName(_fromUtf8("save_procdetvan_label"))
        self.horizontalLayout_7.addWidget(self.save_procdetvan_label)
        self.save_procdetvan_edit = QtGui.QLineEdit(self.det_van_gb)
        self.save_procdetvan_edit.setObjectName(_fromUtf8("save_procdetvan_edit"))
        self.horizontalLayout_7.addWidget(self.save_procdetvan_edit)
        self.save_procdetvan_save = QtGui.QPushButton(self.det_van_gb)
        self.save_procdetvan_save.setObjectName(_fromUtf8("save_procdetvan_save"))
        self.horizontalLayout_7.addWidget(self.save_procdetvan_save)
        spacerItem6 = QtGui.QSpacerItem(40, 20, QtGui.QSizePolicy.Expanding, QtGui.QSizePolicy.Minimum)
        self.horizontalLayout_7.addItem(spacerItem6)
        self.verticalLayout.addLayout(self.horizontalLayout_7)
        self.verticalLayout_2.addWidget(self.det_van_gb)
        spacerItem7 = QtGui.QSpacerItem(20, 124, QtGui.QSizePolicy.Minimum, QtGui.QSizePolicy.Expanding)
        self.verticalLayout_2.addItem(spacerItem7)

        self.retranslateUi(DataCorrsFrame)
        QtCore.QMetaObject.connectSlotsByName(DataCorrsFrame)

    def retranslateUi(self, DataCorrsFrame):
        DataCorrsFrame.setWindowTitle(QtGui.QApplication.translate("DataCorrsFrame", "Frame", None, QtGui.QApplication.UnicodeUTF8))
        self.filter_bad_pulses_chkbox.setText(QtGui.QApplication.translate("DataCorrsFrame", "Filter Bad Pulses", None, QtGui.QApplication.UnicodeUTF8))
        self.incident_beam_norm_gb.setTitle(QtGui.QApplication.translate("DataCorrsFrame", "Incident Beam Normalisation", None, QtGui.QApplication.UnicodeUTF8))
        self.none_rb.setText(QtGui.QApplication.translate("DataCorrsFrame", "None", None, QtGui.QApplication.UnicodeUTF8))
        self.current_rb.setToolTip(QtGui.QApplication.translate("DataCorrsFrame", "Current (aka Proton Charge) Normalisation", None, QtGui.QApplication.UnicodeUTF8))
        self.current_rb.setText(QtGui.QApplication.translate("DataCorrsFrame", "Current", None, QtGui.QApplication.UnicodeUTF8))
        self.monint_label.setText(QtGui.QApplication.translate("DataCorrsFrame", "Integration Range:", None, QtGui.QApplication.UnicodeUTF8))
        self.monitor1_rb.setText(QtGui.QApplication.translate("DataCorrsFrame", "Monitor 1", None, QtGui.QApplication.UnicodeUTF8))
        self.label.setText(QtGui.QApplication.translate("DataCorrsFrame", "to", None, QtGui.QApplication.UnicodeUTF8))
        self.background_sub_gb.setTitle(QtGui.QApplication.translate("DataCorrsFrame", "Time-Independent Background Subtraction", None, QtGui.QApplication.UnicodeUTF8))
        self.tof_start_label.setText(QtGui.QApplication.translate("DataCorrsFrame", "TOF Start", None, QtGui.QApplication.UnicodeUTF8))
        self.tof_end_label.setText(QtGui.QApplication.translate("DataCorrsFrame", "TOF End", None, QtGui.QApplication.UnicodeUTF8))
        self.correct_kikf_cb.setText(QtGui.QApplication.translate("DataCorrsFrame", "Correct Ki/Kf", None, QtGui.QApplication.UnicodeUTF8))
        self.det_van_gb.setTitle(QtGui.QApplication.translate("DataCorrsFrame", "Detector Vanadium", None, QtGui.QApplication.UnicodeUTF8))
        self.van_input_label.setText(QtGui.QApplication.translate("DataCorrsFrame", "Input", None, QtGui.QApplication.UnicodeUTF8))
        self.van_input_browse.setText(QtGui.QApplication.translate("DataCorrsFrame", "Browse", None, QtGui.QApplication.UnicodeUTF8))
        self.use_procdetvan_cb.setText(QtGui.QApplication.translate("DataCorrsFrame", "Use Processed", None, QtGui.QApplication.UnicodeUTF8))
        self.van_int_cb.setText(QtGui.QApplication.translate("DataCorrsFrame", "Use Bounds?", None, QtGui.QApplication.UnicodeUTF8))
        self.van_int_range_label.setText(QtGui.QApplication.translate("DataCorrsFrame", "Range:", None, QtGui.QApplication.UnicodeUTF8))
        self.label_3.setText(QtGui.QApplication.translate("DataCorrsFrame", "to", None, QtGui.QApplication.UnicodeUTF8))
        self.van_int_range_units_cb.setItemText(0, QtGui.QApplication.translate("DataCorrsFrame", "Energy", None, QtGui.QApplication.UnicodeUTF8))
        self.van_int_range_units_cb.setItemText(1, QtGui.QApplication.translate("DataCorrsFrame", "Wavelength", None, QtGui.QApplication.UnicodeUTF8))
        self.van_int_range_units_cb.setItemText(2, QtGui.QApplication.translate("DataCorrsFrame", "TOF", None, QtGui.QApplication.UnicodeUTF8))
        self.save_procdetvan_cb.setText(QtGui.QApplication.translate("DataCorrsFrame", "Save Processed", None, QtGui.QApplication.UnicodeUTF8))
        self.save_procdetvan_label.setText(QtGui.QApplication.translate("DataCorrsFrame", "    Filename:", None, QtGui.QApplication.UnicodeUTF8))
        self.save_procdetvan_save.setText(QtGui.QApplication.translate("DataCorrsFrame", "Save", None, QtGui.QApplication.UnicodeUTF8))
