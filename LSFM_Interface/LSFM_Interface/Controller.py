# from time import time


from ctypes import *
import os
import numpy as np
from datetime import datetime
import time
from PIL import Image
import io
import binascii
import matplotlib.pyplot as plt
import tifffile as tf
from PIL import Image
from io import StringIO
from FLIM_live import FLIM_rgb

#os.chdir(os.path.join('C:\\', 'LSFM_Software', 'TreiberImplementierung', 'bin64'))


dirdll = os.path.join('C:\\', 'LSFM_Software', 'TreiberImplementierung', 'bin64')
os.chdir(dirdll)
mydll = WinDLL('LSFMLibrary.dll')


Operating_Mode = {0: "ACC",
                  1: "APC"}

Pulse_Modulation = {0: "Emission Standby",
                    1: "ACC Mode- No Modulation Active",
                    2:  "APC Mode- No Modulation Active",
                    3:  "ACC Mode- Analog Modulation Active"
                    }

Controllers = {"X Controller": 0,
               "Y Controller": 1,
               "Z Controller": 2,
               "Focus Controller": 3}

waveLengthList = { 638: 1,
               488:2,
               455:3
}

filterPositions = {}
class DataPack(Structure):
    _fields_ = [("width", c_int),
                ("height", c_int),
                ("buffersize", c_int),
                ("bufferAddress", POINTER(c_uint16))]





def Connect():
    mydll.Connect()
    MoveAbsolut(0, 39.88,0)
    MoveAbsolut(1, 67.5,0)



def GetConnectedCamera():
    mydll.GetConnectedCamera.argtypes = [np.ctypeslib.ndpointer(dtype=np.int32)]
    array = np.zeros(2, np.int32)
    mydll.GetConnectedCamera(array)

    listOfAvailableCamera = []

    for value in array:
        if value == 1:
            listOfAvailableCamera.append("LSFM_Camera")
        if value == 2:
            listOfAvailableCamera.append("LSFM_FLIM_Camera")

    return listOfAvailableCamera, array


def Disconnect():
    try:
        mydll.Disconnect()
    except OSError:
        print("Error with the Disconnect method")


def SetPower(wavelength, power):
    wavelength = waveLengthList[wavelength]
    mydll.SetPower(c_int(wavelength), c_float(power))


def GetActualPulseModulation(wavelength):
    mydll.GetActualModulation.restype = c_int
    pulse_modulation = mydll.GetActualModulation(c_int(wavelength))
    return Pulse_Modulation[pulse_modulation]


def GetModulation(wavelength):
    mydll.GetModulation.restype = c_int
    modulation = mydll.GetModulation(c_int(wavelength))
    return Operating_Mode[modulation]


def GetWaveLength():
    waveLength = mydll.ReadWaveLength()
    if waveLength == 1:
        return 638
    elif waveLength == 2:
        return 488
    else:
        return 455


def MoveAbsolut(stage, position, acquisitionRunning):
    mydll.MoveAbsolut.argtypes = [c_int, c_double,c_bool]
    mydll.MoveAbsolut(stage, position, acquisitionRunning)


def MoveRelativ(stage, position):
    mydll.MoveRelativ.argtypes = [c_int, c_double, c_bool]
    mydll.MoveRelativ(stage, position ,False)


def GetLaserPower(wavelength):
    mydll.GetPowerInPercentage.restype = c_double
    power = mydll.GetPowerInPercentage(c_int(wavelength))
    return power


def LaserOn(wavelength):
    mydll.LaserOn(c_int(wavelength))
    return True



def SetModulation(wavelength, modulation):
    mydll.SetModulation(c_int(wavelength),c_int(modulation))
    return True


def LaserOff(wavelength):
    mydll.LaserOff(c_int(wavelength))
    return True


def StartSequenceAquisition(cameraType, path, zUp, zDown, xValue, yValue, focusUp, focusDown, N, dZ , exposureParameter, ActualLaserParameter,
                            filterparameter, phaseNumber):

    N = int(N)
    now = datetime.now()
    s = "Sequence_Acquisition_" + now.strftime("%d") + "_" + now.strftime("%m") + "_" + now.strftime("%y") + "_" + now.strftime(
        "%H") + "h" + now.strftime("%M") + "m" + now.strftime("%S") + "s"
    path = os.path.join(path, s)
    os.mkdir(path)

    path2 = os.path.join(path,"AcquisitionParameter.txt")



    f = open(path2, "w+")

    f.write("Ordnername =  %s  \r\n" % (path))
    f.write("Focus_UP Value =  %f  \r\n" % (focusUp))
    f.write("Focus_Down Value =  %f  \r\n" % (focusDown))
    f.write("Z_UP Value =  %f  \r\n" % (zUp))
    f.write("Z_Down Value =  %f  \r\n" % (zDown))
    f.write("X Value =  %f  \r\n" % (xValue))
    f.write("Y Value =  %f  \r\n" % (yValue))
    f.write("dZ Value =  %f  micrometer \r\n" % (dZ*1000))
    f.write("Image Number =  %d  \r\n" % (N))
    f.write("ExposureTime =  %d  ms \r\n" % (exposureParameter["ExposureTime"]))
    f.write("DelayTime =  %d ms \r\n" % (exposureParameter["DelayTime"]))
    f.write("LaserTyp =  %s  \r\n" % (ActualLaserParameter["LaserTyp"]))
    f.write("LaserPower =  %d  \r\n" % (ActualLaserParameter["LaserPower"]))
    f.write("FilterWheel =  %s  \r\n" % (filterparameter))


    f.close()


    b_string1 = path.encode('utf-8')


    mydll.Image_Sequence(c_int(cameraType), c_char_p(b_string1), c_double(zUp), c_double(zDown), c_double(xValue),
                             c_double(yValue),
                             c_double(focusUp), c_double(focusDown), c_int(N), c_double(dZ) , c_int(phaseNumber))


    return path


def SingleImageAquisition(camera, filePath):

    filePathsplit = filePath.split('\\')
    fP = filePathsplit[0]
    for j,f in enumerate(filePathsplit):
        if j == 0:
            continue
        else:
            fP += '\\\\'+f
    b_string1 = fP.encode('utf-8')


    response =  mydll.SingleImageAquisition(c_int(camera), c_char_p(b_string1))

    return


def SetFlimParamter(cameraType, phaseNumber, frequency):
    mydll.SetFlimParamter(c_int(cameraType),   c_int(phaseNumber), c_int(frequency))

def GetAllPositionsName():
    string_buffers = [create_string_buffer(400)]
    pointers = c_wchar_p(*map(addressof, string_buffers))
    mydll.GetAllFiltersPositionsName(pointers)

    list = str(string_buffers[0].value).split("'")

    response = list[1]

    list2 = response.split("//")

    filterPositions[1] = list2[0]
    filterPositions[2] = list2[1]
    filterPositions[3] = list2[2]
    filterPositions[4] = list2[3]
    filterPositions[5] = list2[4]

    return filterPositions


def GetListofNotConnectedHardware():
    string_buffers = [create_string_buffer(600)]
    pointers = c_wchar_p(*map(addressof, string_buffers))
    mydll.ListNotConnectedHardware(pointers)

    list = str(string_buffers[0].value)
    list = str(string_buffers[0].value).split("'")

    list  = list[1].split("//")
    finalList = []

    for i in range(len(list) -1):
            finalList.append(list[i])

    print(finalList)

    return finalList


def MoveFilterToPosition(position):
    actPosition = mydll.MoveFilterToPosition(c_int(position))
    return actPosition


def GetActualFilterPosition():
   actualFilterPosition = mydll.GetFilterCurrentPosition()

   return actualFilterPosition

def GetStagesPosition():
    listOfPositions = {}
    xPosition = (c_double*1)()
    yPosition = (c_double*1)()
    zPosition = (c_double*1)()
    focusPosition = (c_double*1)()

    xPointer = cast(xPosition,POINTER(c_double))
    yPointer = cast(yPosition, POINTER(c_double))
    zPointer = cast(zPosition, POINTER(c_double))
    focusPointer = cast(focusPosition, POINTER(c_double))

    value = mydll.GetActualPositions(xPointer, yPointer, zPointer, focusPointer)



    listOfPositions["X Controller"] = xPosition[0]
    listOfPositions["Y Controller"] = yPosition[0]
    listOfPositions["Z Controller"] = zPosition[0]
    listOfPositions["Focus Controller"] = focusPosition[0]

    return listOfPositions


def SetExposure( camera , exposureTime , delay):
        mydll.SetExposureTime(c_int(camera),c_int(exposureTime), c_int(delay))



def GetExposureTime(camera):
    exposuretime = (c_int*1)()
    delay = (c_int*1)()

    exposurePointer = cast(exposuretime, POINTER(c_int))
    delayPointer = cast(delay,POINTER(c_int))


    mydll.GetExposureTime(c_int(camera), exposurePointer, delayPointer)

    exposureParameter = [exposuretime[0], delay[0]]

    return exposureParameter


def Live_View(camera):

    if (camera == 1):
         mydll.LiveView.restype = POINTER(DataPack)
         data = mydll.LiveView(c_int(camera))
         dbuffer = (c_uint16 * data.contents.buffersize).from_address(addressof(data.contents.bufferAddress.contents))
         img = np.frombuffer(dbuffer, dtype=np.uint16).reshape([data.contents.height,data.contents.width])
    else:
        mydll.LiveView.restype = POINTER(DataPack)
        try:
            data = mydll.LiveView(c_int(camera))
            dbuffer = (c_uint16 * data.contents.buffersize).from_address(
                addressof(data.contents.bufferAddress.contents))
            img = np.frombuffer(dbuffer, dtype=np.uint16).reshape([data.contents.height, data.contents.width])
        except:
            img = None


        """
        mydll.LiveView.restype = POINTER(DataPack)
        data = mydll.LiveView()
        f_tmp = os.path.join('C:\\', 'LSFM_Software', 'LSFM_Interface', 'LSFM_Interface', 'tmp', 'image_live_000.tif')
        img = tf.imread(f_tmp)
        img_rgb = []
        
        d_tmp = os.path.join('C:\\','LSFM_Software','LSFM_Interface','LSFM_Interface','tmp')
        f_seq = [os.path.join(d_tmp,'image_live_00'+str(j)+'.tif') for j in range(4)]

        im = tf.imread(f_seq)
        img_rgb = FLIM_rgb(*im)
        im_gs = np.mean(im,axis=0).astype('uint16')
        """

    return img
