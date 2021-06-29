# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'ListDarstellung.ui'
#
# Created by: PyQt5 UI code generator 5.9.2
#
# WARNING! All changes made in this file will be lost!

from PyQt5 import QtCore, QtGui, QtWidgets
import glob
import os
from PIL import Image
import numpy as np
import shutil



from PyQt5.QtGui import QImage, QPixmap
from PyQt5.QtWidgets import QDialog


filesPath1 = mainWindow1 = liveModus1 = fastForward1 = None
class CustomDialog(QDialog):
    def closeEvent(self, event):
        global filesPath1, mainWindow1, liveModus1
        try:
            if(fastForward1):
             shutil.rmtree(filesPath1)
        except OSError as e:
            print("Error: %s : %s" % (filesPath1, e.strerror))



class FastForward_Dialog(QDialog):

    def __init__(self, filePath, mainwindow, liveModus,fastForward):
        global filesPath1, mainWindow1, liveModus1,fastForward1
        super().__init__()
        self.filesPath = filePath
        self.mainWindow = mainwindow
        self.liveModus = liveModus
        filesPath1 = self.filesPath
        mainWindow1 = self.mainWindow
        liveModus1 = self.liveModus
        fastForward1 = fastForward


    def setupUi(self, Dialog):
        Dialog.setObjectName("Dialog")
        Dialog.resize(396, 316)
        self.verticalLayout_2 = QtWidgets.QVBoxLayout(Dialog)
        self.verticalLayout_2.setContentsMargins(0, 0, 0, 0)
        self.verticalLayout_2.setSpacing(0)
        self.verticalLayout_2.setObjectName("verticalLayout_2")
        self.frame = QtWidgets.QFrame(Dialog)
        self.frame.setFrameShape(QtWidgets.QFrame.StyledPanel)
        self.frame.setFrameShadow(QtWidgets.QFrame.Raised)
        self.frame.setObjectName("frame")
        self.verticalLayout = QtWidgets.QVBoxLayout(self.frame)
        self.verticalLayout.setContentsMargins(0, 0, 0, 0)
        self.verticalLayout.setSpacing(0)
        self.verticalLayout.setObjectName("verticalLayout")
        self.PictureListWidget = QtWidgets.QListWidget(self.frame)
        self.PictureListWidget.setObjectName("PictureListWidget")
        self.verticalLayout.addWidget(self.PictureListWidget)
        self.verticalLayout_2.addWidget(self.frame)

        self.retranslateUi(Dialog)
        QtCore.QMetaObject.connectSlotsByName(Dialog)

        self.filesPath1 = os.path.join(self.filesPath, "*.tif")
        files_abs = glob.glob(self.filesPath1)
        files = [os.path.basename(f) for f in files_abs]
        self.fullpath = {files[f]: files_abs[f] for f in range(len(files_abs))}

        self.PictureListWidget.addItems(files)

        self.PictureListWidget.currentItemChanged.connect(lambda:self.setParameter())

        self.liveModus = True
        self.mainWindow.StartLiveView(0)


    def retranslateUi(self, Dialog):
        _translate = QtCore.QCoreApplication.translate
        Dialog.setWindowTitle(_translate("Dialog", "Dialog"))


    def setParameter(self):
        file = self.PictureListWidget.currentItem().text()
        file_abs = self.fullpath[file]
        im = Image.open(file_abs)
        image = np.array(im)
        y, x = np.histogram(image, bins=np.linspace(0, 65535, 256))
        image = np.uint8(image*255./65535.)
        qImage = QImage(image, image.shape[0], image.shape[1], QImage.Format_Indexed8)
        pixmap = QPixmap(qImage)
        pixmap = pixmap.scaledToHeight(512)
        self.mainWindow.imageLive.setPixmap(pixmap)
        self.mainWindow.imageLive.repaint()
        self.mainWindow.graphicsView_2.clear()

        self.mainWindow.graphicsView_2.plot(x, y, stepMode=True, fillLevel=0, brush=(0, 0, 255, 150))




