#pylint: disable=invalid-name,attribute-defined-outside-init,too-many-statements,too-many-instance-attributes,line-too-long
# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'ui/reflectometer/refl_stitching.ui'
#
# Created: Mon Jul 16 11:31:20 2012
#      by: PyQt4 UI code generator 4.7.4
#
# WARNING! All changes made in this file will be lost!

from PyQt4 import QtCore, QtGui

class Ui_Frame(object):
    def setupUi(self, Frame):
        Frame.setObjectName("Frame")
        Frame.resize(1373, 1239)
        Frame.setFrameShape(QtGui.QFrame.NoFrame)
        Frame.setFrameShadow(QtGui.QFrame.Plain)
        Frame.setLineWidth(0)
        self.verticalLayout = QtGui.QVBoxLayout(Frame)
        self.verticalLayout.setObjectName("verticalLayout")
        self.scrollArea = QtGui.QScrollArea(Frame)
        self.scrollArea.setFrameShape(QtGui.QFrame.NoFrame)
        self.scrollArea.setFrameShadow(QtGui.QFrame.Plain)
        self.scrollArea.setLineWidth(0)
        self.scrollArea.setWidgetResizable(True)
        self.scrollArea.setObjectName("scrollArea")
        self.scrollAreaWidgetContents = QtGui.QWidget(self.scrollArea)
        self.scrollAreaWidgetContents.setGeometry(QtCore.QRect(0, 0, 1355, 1221))
        self.scrollAreaWidgetContents.setObjectName("scrollAreaWidgetContents")
        self.verticalLayout_2 = QtGui.QVBoxLayout(self.scrollAreaWidgetContents)
        self.verticalLayout_2.setObjectName("verticalLayout_2")
        self.horizontalLayout_6 = QtGui.QHBoxLayout()
        self.horizontalLayout_6.setObjectName("horizontalLayout_6")
        self.verticalLayout_3 = QtGui.QVBoxLayout()
        self.verticalLayout_3.setObjectName("verticalLayout_3")
        self.horizontalLayout_3 = QtGui.QHBoxLayout()
        self.horizontalLayout_3.setObjectName("horizontalLayout_3")
        self.groupBox_3 = QtGui.QGroupBox(self.scrollAreaWidgetContents)
        self.groupBox_3.setObjectName("groupBox_3")
        self.verticalLayout_4 = QtGui.QVBoxLayout(self.groupBox_3)
        self.verticalLayout_4.setObjectName("verticalLayout_4")
        self.label = QtGui.QLabel(self.groupBox_3)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Minimum, QtGui.QSizePolicy.Preferred)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.label.sizePolicy().hasHeightForWidth())
        self.label.setSizePolicy(sizePolicy)
        self.label.setObjectName("label")
        self.verticalLayout_4.addWidget(self.label)
        self.horizontalLayout_4 = QtGui.QHBoxLayout()
        self.horizontalLayout_4.setObjectName("horizontalLayout_4")
        self.label_2 = QtGui.QLabel(self.groupBox_3)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Fixed, QtGui.QSizePolicy.Preferred)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.label_2.sizePolicy().hasHeightForWidth())
        self.label_2.setSizePolicy(sizePolicy)
        self.label_2.setMinimumSize(QtCore.QSize(316, 0))
        self.label_2.setMaximumSize(QtCore.QSize(316, 16777215))
        self.label_2.setObjectName("label_2")
        self.horizontalLayout_4.addWidget(self.label_2)
        self.label_4 = QtGui.QLabel(self.groupBox_3)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Fixed, QtGui.QSizePolicy.Preferred)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.label_4.sizePolicy().hasHeightForWidth())
        self.label_4.setSizePolicy(sizePolicy)
        self.label_4.setMinimumSize(QtCore.QSize(80, 0))
        self.label_4.setMaximumSize(QtCore.QSize(80, 16777215))
        self.label_4.setObjectName("label_4")
        self.horizontalLayout_4.addWidget(self.label_4)
        self.label_3 = QtGui.QLabel(self.groupBox_3)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Fixed, QtGui.QSizePolicy.Preferred)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.label_3.sizePolicy().hasHeightForWidth())
        self.label_3.setSizePolicy(sizePolicy)
        self.label_3.setMinimumSize(QtCore.QSize(80, 0))
        self.label_3.setMaximumSize(QtCore.QSize(80, 16777215))
        self.label_3.setObjectName("label_3")
        self.horizontalLayout_4.addWidget(self.label_3)
        self.label_5 = QtGui.QLabel(self.groupBox_3)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Fixed, QtGui.QSizePolicy.Preferred)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.label_5.sizePolicy().hasHeightForWidth())
        self.label_5.setSizePolicy(sizePolicy)
        self.label_5.setMinimumSize(QtCore.QSize(80, 0))
        self.label_5.setMaximumSize(QtCore.QSize(80, 16777215))
        self.label_5.setObjectName("label_5")
        self.horizontalLayout_4.addWidget(self.label_5)
        spacerItem = QtGui.QSpacerItem(40, 20, QtGui.QSizePolicy.Expanding, QtGui.QSizePolicy.Minimum)
        self.horizontalLayout_4.addItem(spacerItem)
        self.verticalLayout_4.addLayout(self.horizontalLayout_4)
        self.horizontalLayout = QtGui.QHBoxLayout()
        self.horizontalLayout.setObjectName("horizontalLayout")
        self.angle_list_layout = QtGui.QVBoxLayout()
        self.angle_list_layout.setObjectName("angle_list_layout")
        self.horizontalLayout.addLayout(self.angle_list_layout)
        spacerItem1 = QtGui.QSpacerItem(40, 20, QtGui.QSizePolicy.Expanding, QtGui.QSizePolicy.Minimum)
        self.horizontalLayout.addItem(spacerItem1)
        self.verticalLayout_4.addLayout(self.horizontalLayout)
        self.horizontalLayout_3.addWidget(self.groupBox_3)
        self.verticalLayout_3.addLayout(self.horizontalLayout_3)
        self.line = QtGui.QFrame(self.scrollAreaWidgetContents)
        self.line.setFrameShape(QtGui.QFrame.HLine)
        self.line.setFrameShadow(QtGui.QFrame.Sunken)
        self.line.setObjectName("line")
        self.verticalLayout_3.addWidget(self.line)
        self.horizontalLayout_2 = QtGui.QHBoxLayout()
        self.horizontalLayout_2.setObjectName("horizontalLayout_2")
        spacerItem2 = QtGui.QSpacerItem(20, 20, QtGui.QSizePolicy.Fixed, QtGui.QSizePolicy.Minimum)
        self.horizontalLayout_2.addItem(spacerItem2)
        self.scale_to_one_chk = QtGui.QCheckBox(self.scrollAreaWidgetContents)
        self.scale_to_one_chk.setObjectName("scale_to_one_chk")
        self.horizontalLayout_2.addWidget(self.scale_to_one_chk)
        spacerItem3 = QtGui.QSpacerItem(60, 20, QtGui.QSizePolicy.Fixed, QtGui.QSizePolicy.Minimum)
        self.horizontalLayout_2.addItem(spacerItem3)
        self.max_q_unity_label = QtGui.QLabel(self.scrollAreaWidgetContents)
        self.max_q_unity_label.setObjectName("max_q_unity_label")
        self.horizontalLayout_2.addWidget(self.max_q_unity_label)
        self.min_q_unity_edit = QtGui.QLineEdit(self.scrollAreaWidgetContents)
        self.min_q_unity_edit.setMinimumSize(QtCore.QSize(80, 0))
        self.min_q_unity_edit.setMaximumSize(QtCore.QSize(80, 16777215))
        self.min_q_unity_edit.setObjectName("min_q_unity_edit")
        self.horizontalLayout_2.addWidget(self.min_q_unity_edit)
        self.label_6 = QtGui.QLabel(self.scrollAreaWidgetContents)
        self.label_6.setObjectName("label_6")
        self.horizontalLayout_2.addWidget(self.label_6)
        self.max_q_unity_edit = QtGui.QLineEdit(self.scrollAreaWidgetContents)
        self.max_q_unity_edit.setMinimumSize(QtCore.QSize(80, 0))
        self.max_q_unity_edit.setMaximumSize(QtCore.QSize(80, 16777215))
        self.max_q_unity_edit.setObjectName("max_q_unity_edit")
        self.horizontalLayout_2.addWidget(self.max_q_unity_edit)
        self.label_7 = QtGui.QLabel(self.scrollAreaWidgetContents)
        self.label_7.setObjectName("label_7")
        self.horizontalLayout_2.addWidget(self.label_7)
        self.pick_unity_range_btn = QtGui.QPushButton(self.scrollAreaWidgetContents)
        self.pick_unity_range_btn.setObjectName("pick_unity_range_btn")
        self.horizontalLayout_2.addWidget(self.pick_unity_range_btn)
        spacerItem4 = QtGui.QSpacerItem(40, 20, QtGui.QSizePolicy.Expanding, QtGui.QSizePolicy.Minimum)
        self.horizontalLayout_2.addItem(spacerItem4)
        self.auto_scale_btn = QtGui.QPushButton(self.scrollAreaWidgetContents)
        self.auto_scale_btn.setMaximumSize(QtCore.QSize(85, 16777215))
        self.auto_scale_btn.setObjectName("auto_scale_btn")
        self.horizontalLayout_2.addWidget(self.auto_scale_btn)
        self.save_btn = QtGui.QPushButton(self.scrollAreaWidgetContents)
        self.save_btn.setObjectName("save_btn")
        self.horizontalLayout_2.addWidget(self.save_btn)
        self.verticalLayout_3.addLayout(self.horizontalLayout_2)
        self.horizontalLayout_5 = QtGui.QHBoxLayout()
        self.horizontalLayout_5.setObjectName("horizontalLayout_5")
        spacerItem5 = QtGui.QSpacerItem(20, 20, QtGui.QSizePolicy.Fixed, QtGui.QSizePolicy.Minimum)
        self.horizontalLayout_5.addItem(spacerItem5)
        self.ref_pol_label = QtGui.QLabel(self.scrollAreaWidgetContents)
        self.ref_pol_label.setObjectName("ref_pol_label")
        self.horizontalLayout_5.addWidget(self.ref_pol_label)
        self.off_off_radio = QtGui.QRadioButton(self.scrollAreaWidgetContents)
        self.off_off_radio.setMinimumSize(QtCore.QSize(100, 0))
        self.off_off_radio.setChecked(True)
        self.off_off_radio.setObjectName("off_off_radio")
        self.horizontalLayout_5.addWidget(self.off_off_radio)
        self.on_off_radio = QtGui.QRadioButton(self.scrollAreaWidgetContents)
        self.on_off_radio.setMinimumSize(QtCore.QSize(100, 0))
        self.on_off_radio.setObjectName("on_off_radio")
        self.horizontalLayout_5.addWidget(self.on_off_radio)
        self.off_on_radio = QtGui.QRadioButton(self.scrollAreaWidgetContents)
        self.off_on_radio.setMinimumSize(QtCore.QSize(100, 0))
        self.off_on_radio.setObjectName("off_on_radio")
        self.horizontalLayout_5.addWidget(self.off_on_radio)
        self.on_on_radio = QtGui.QRadioButton(self.scrollAreaWidgetContents)
        self.on_on_radio.setMinimumSize(QtCore.QSize(100, 0))
        self.on_on_radio.setObjectName("on_on_radio")
        self.horizontalLayout_5.addWidget(self.on_on_radio)
        spacerItem6 = QtGui.QSpacerItem(40, 20, QtGui.QSizePolicy.Expanding, QtGui.QSizePolicy.Minimum)
        self.horizontalLayout_5.addItem(spacerItem6)
        self.verticalLayout_3.addLayout(self.horizontalLayout_5)
        self.line_2 = QtGui.QFrame(self.scrollAreaWidgetContents)
        self.line_2.setFrameShape(QtGui.QFrame.HLine)
        self.line_2.setFrameShadow(QtGui.QFrame.Sunken)
        self.line_2.setObjectName("line_2")
        self.verticalLayout_3.addWidget(self.line_2)
        self.horizontalLayout_7 = QtGui.QHBoxLayout()
        self.horizontalLayout_7.setObjectName("horizontalLayout_7")
        spacerItem7 = QtGui.QSpacerItem(20, 20, QtGui.QSizePolicy.Fixed, QtGui.QSizePolicy.Minimum)
        self.horizontalLayout_7.addItem(spacerItem7)
        self.send_email_chk = QtGui.QCheckBox(self.scrollAreaWidgetContents)
        self.send_email_chk.setObjectName("send_email_chk")
        self.horizontalLayout_7.addWidget(self.send_email_chk)
        spacerItem8 = QtGui.QSpacerItem(40, 20, QtGui.QSizePolicy.Expanding, QtGui.QSizePolicy.Minimum)
        self.horizontalLayout_7.addItem(spacerItem8)
        self.verticalLayout_3.addLayout(self.horizontalLayout_7)
        self.horizontalLayout_8 = QtGui.QHBoxLayout()
        self.horizontalLayout_8.setObjectName("horizontalLayout_8")
        spacerItem9 = QtGui.QSpacerItem(40, 20, QtGui.QSizePolicy.Fixed, QtGui.QSizePolicy.Minimum)
        self.horizontalLayout_8.addItem(spacerItem9)
        self.to_email_label = QtGui.QLabel(self.scrollAreaWidgetContents)
        self.to_email_label.setObjectName("to_email_label")
        self.horizontalLayout_8.addWidget(self.to_email_label)
        self.to_email_edit = QtGui.QLineEdit(self.scrollAreaWidgetContents)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Fixed, QtGui.QSizePolicy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.to_email_edit.sizePolicy().hasHeightForWidth())
        self.to_email_edit.setSizePolicy(sizePolicy)
        self.to_email_edit.setMaximumSize(QtCore.QSize(300, 16777215))
        self.to_email_edit.setObjectName("to_email_edit")
        self.horizontalLayout_8.addWidget(self.to_email_edit)
        self.from_email_label = QtGui.QLabel(self.scrollAreaWidgetContents)
        self.from_email_label.setObjectName("from_email_label")
        self.horizontalLayout_8.addWidget(self.from_email_label)
        self.from_email_edit = QtGui.QLineEdit(self.scrollAreaWidgetContents)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Fixed, QtGui.QSizePolicy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.from_email_edit.sizePolicy().hasHeightForWidth())
        self.from_email_edit.setSizePolicy(sizePolicy)
        self.from_email_edit.setMaximumSize(QtCore.QSize(300, 16777215))
        self.from_email_edit.setObjectName("from_email_edit")
        self.horizontalLayout_8.addWidget(self.from_email_edit)
        spacerItem10 = QtGui.QSpacerItem(40, 20, QtGui.QSizePolicy.Fixed, QtGui.QSizePolicy.Minimum)
        self.horizontalLayout_8.addItem(spacerItem10)
        self.send_btn = QtGui.QPushButton(self.scrollAreaWidgetContents)
        self.send_btn.setObjectName("send_btn")
        self.horizontalLayout_8.addWidget(self.send_btn)
        spacerItem11 = QtGui.QSpacerItem(40, 20, QtGui.QSizePolicy.Expanding, QtGui.QSizePolicy.Minimum)
        self.horizontalLayout_8.addItem(spacerItem11)
        self.verticalLayout_3.addLayout(self.horizontalLayout_8)
        spacerItem12 = QtGui.QSpacerItem(20, 40, QtGui.QSizePolicy.Minimum, QtGui.QSizePolicy.Expanding)
        self.verticalLayout_3.addItem(spacerItem12)
        self.horizontalLayout_6.addLayout(self.verticalLayout_3)
        self.verticalLayout_2.addLayout(self.horizontalLayout_6)
        self.scrollArea.setWidget(self.scrollAreaWidgetContents)
        self.verticalLayout.addWidget(self.scrollArea)

        self.retranslateUi(Frame)
        QtCore.QMetaObject.connectSlotsByName(Frame)

    def retranslateUi(self, Frame):
        Frame.setWindowTitle(QtGui.QApplication.translate("Frame", "Frame", None, QtGui.QApplication.UnicodeUTF8))
        self.groupBox_3.setTitle(QtGui.QApplication.translate("Frame", "Angle List", None, QtGui.QApplication.UnicodeUTF8))
        self.label.setText(QtGui.QApplication.translate("Frame", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"\
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:\'Ubuntu\'; font-size:11pt; font-weight:400; font-style:normal;\">\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-style:italic;\">The following is the list of data stetched that we stitched together, along with their computed scaling factor.</span></p>\n"
"<p style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; font-style:italic;\"></p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-style:italic;\">Use the radio button to select the data set to use as the reference and set its absolute scale. </span></p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-style:italic;\">Click &quot;Auto scale&quot; to computer the scaling factors of the other data sets.</span></p>\n"
"<p style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; font-style:italic;\"></p>\n"
"<p style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; font-style:italic;\"></p>\n"
"<p style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; font-style:italic;\"></p></body></html>", None, QtGui.QApplication.UnicodeUTF8))
        self.label_2.setText(QtGui.QApplication.translate("Frame", "Run", None, QtGui.QApplication.UnicodeUTF8))
        self.label_4.setText(QtGui.QApplication.translate("Frame", "Scaling\n"\
"factor", None, QtGui.QApplication.UnicodeUTF8))
        self.label_3.setText(QtGui.QApplication.translate("Frame", "Low-end\n"\
"points to\n"
"skip", None, QtGui.QApplication.UnicodeUTF8))
        self.label_5.setText(QtGui.QApplication.translate("Frame", "High-end\n"\
"points to\n"
"skip", None, QtGui.QApplication.UnicodeUTF8))
        self.scale_to_one_chk.setText(QtGui.QApplication.translate("Frame", "Scale to unity", None, QtGui.QApplication.UnicodeUTF8))
        self.max_q_unity_label.setText(QtGui.QApplication.translate("Frame", "Critical edge from", None, QtGui.QApplication.UnicodeUTF8))
        self.label_6.setText(QtGui.QApplication.translate("Frame", "1/Å  to  ", None, QtGui.QApplication.UnicodeUTF8))
        self.label_7.setText(QtGui.QApplication.translate("Frame", "1/Å  ", None, QtGui.QApplication.UnicodeUTF8))
        self.pick_unity_range_btn.setText(QtGui.QApplication.translate("Frame", "Pick range", None, QtGui.QApplication.UnicodeUTF8))
        self.auto_scale_btn.setText(QtGui.QApplication.translate("Frame", "Auto scale", None, QtGui.QApplication.UnicodeUTF8))
        self.save_btn.setText(QtGui.QApplication.translate("Frame", "Set scale", None, QtGui.QApplication.UnicodeUTF8))
        self.ref_pol_label.setText(QtGui.QApplication.translate("Frame", "Reference cross-section:", None, QtGui.QApplication.UnicodeUTF8))
        self.off_off_radio.setText(QtGui.QApplication.translate("Frame", "Off-Off", None, QtGui.QApplication.UnicodeUTF8))
        self.on_off_radio.setText(QtGui.QApplication.translate("Frame", "On-Off", None, QtGui.QApplication.UnicodeUTF8))
        self.off_on_radio.setText(QtGui.QApplication.translate("Frame", "Off-On", None, QtGui.QApplication.UnicodeUTF8))
        self.on_on_radio.setText(QtGui.QApplication.translate("Frame", "On-On", None, QtGui.QApplication.UnicodeUTF8))
        self.send_email_chk.setText(QtGui.QApplication.translate("Frame", "Send as email", None, QtGui.QApplication.UnicodeUTF8))
        self.to_email_label.setText(QtGui.QApplication.translate("Frame", "Recipient address:", None, QtGui.QApplication.UnicodeUTF8))
        self.from_email_label.setText(QtGui.QApplication.translate("Frame", "Your address:", None, QtGui.QApplication.UnicodeUTF8))
        self.send_btn.setText(QtGui.QApplication.translate("Frame", "Send", None, QtGui.QApplication.UnicodeUTF8))
