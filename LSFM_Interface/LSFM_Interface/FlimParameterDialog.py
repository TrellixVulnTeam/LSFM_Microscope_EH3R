# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'FlimParameterDialog.ui'
#
# Created by: PyQt5 UI code generator 5.9.2
#
# WARNING! All changes made in this file will be lost!

from PyQt5 import QtCore, QtGui, QtWidgets

class FlimParameter_Dialog(object):
    def setupUi(self, Dialog):
        Dialog.setObjectName("Dialog")
        Dialog.resize(441, 138)
        Dialog.setMinimumSize(QtCore.QSize(441, 138))
        Dialog.setMaximumSize(QtCore.QSize(441, 138))
        self.verticalLayout = QtWidgets.QVBoxLayout(Dialog)
        self.verticalLayout.setContentsMargins(0, 20, 0, 0)
        self.verticalLayout.setSpacing(0)
        self.verticalLayout.setObjectName("verticalLayout")
        self.frame = QtWidgets.QFrame(Dialog)
        self.frame.setFrameShape(QtWidgets.QFrame.NoFrame)
        self.frame.setFrameShadow(QtWidgets.QFrame.Raised)
        self.frame.setLineWidth(0)
        self.frame.setObjectName("frame")
        self.verticalLayout_2 = QtWidgets.QVBoxLayout(self.frame)
        self.verticalLayout_2.setContentsMargins(0, 0, 0, 0)
        self.verticalLayout_2.setObjectName("verticalLayout_2")
        self.FrequencyFrame = QtWidgets.QFrame(self.frame)
        self.FrequencyFrame.setFrameShape(QtWidgets.QFrame.NoFrame)
        self.FrequencyFrame.setFrameShadow(QtWidgets.QFrame.Raised)
        self.FrequencyFrame.setObjectName("FrequencyFrame")
        self.horizontalLayout = QtWidgets.QHBoxLayout(self.FrequencyFrame)
        self.horizontalLayout.setContentsMargins(0, 0, 4, 4)
        self.horizontalLayout.setSpacing(0)
        self.horizontalLayout.setObjectName("horizontalLayout")
        self.frequencyLabel = QtWidgets.QLabel(self.FrequencyFrame)
        self.frequencyLabel.setObjectName("frequencyLabel")
        self.horizontalLayout.addWidget(self.frequencyLabel)
        self.frequencyList = QtWidgets.QComboBox(self.FrequencyFrame)
        self.frequencyList.setObjectName("frequencyList")
        self.horizontalLayout.addWidget(self.frequencyList)
        self.verticalLayout_2.addWidget(self.FrequencyFrame)
        self.phaseFrame = QtWidgets.QFrame(self.frame)
        self.phaseFrame.setFrameShape(QtWidgets.QFrame.NoFrame)
        self.phaseFrame.setFrameShadow(QtWidgets.QFrame.Raised)
        self.phaseFrame.setObjectName("phaseFrame")
        self.horizontalLayout_2 = QtWidgets.QHBoxLayout(self.phaseFrame)
        self.horizontalLayout_2.setContentsMargins(0, 0, 4, 4)
        self.horizontalLayout_2.setSpacing(0)
        self.horizontalLayout_2.setObjectName("horizontalLayout_2")
        self.phaseLabel = QtWidgets.QLabel(self.phaseFrame)
        self.phaseLabel.setObjectName("phaseLabel")
        self.horizontalLayout_2.addWidget(self.phaseLabel)
        self.phaseList = QtWidgets.QComboBox(self.phaseFrame)
        self.phaseList.setObjectName("phaseList")
        self.horizontalLayout_2.addWidget(self.phaseList)
        self.verticalLayout_2.addWidget(self.phaseFrame)
        self.verticalLayout.addWidget(self.frame)
        self.buttonBox = QtWidgets.QDialogButtonBox(Dialog)
        self.buttonBox.setOrientation(QtCore.Qt.Horizontal)
        self.buttonBox.setStandardButtons(QtWidgets.QDialogButtonBox.Cancel|QtWidgets.QDialogButtonBox.Ok)
        self.buttonBox.setObjectName("buttonBox")
        self.verticalLayout.addWidget(self.buttonBox, 0, QtCore.Qt.AlignHCenter)

        self.retranslateUi(Dialog)
        self.buttonBox.rejected.connect(Dialog.reject)
        self.buttonBox.accepted.connect(Dialog.accept)
        QtCore.QMetaObject.connectSlotsByName(Dialog)

    def retranslateUi(self, Dialog):
        _translate = QtCore.QCoreApplication.translate
        Dialog.setWindowTitle(_translate("Dialog", "Dialog"))
        self.frequencyLabel.setText(_translate("Dialog", "Modulation Frequency in MHZ"))
        self.phaseLabel.setText(_translate("Dialog", "Number of Phases"))


