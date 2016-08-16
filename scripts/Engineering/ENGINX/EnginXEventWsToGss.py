import mantid.api as mantidApi
import mantid.simpleapi as mantidSapi

def convertEventWsToHistoGss(vanFile, cerFile, inputFileList, bank="", detectorIndicies=""):
    """
    Runs a calibration using the input vanadium and cerium oxide 
    runs then saves this as a .prm file. Using this calibration
    it then converts the list of event workspaces to histogram
    workspaces and saves them out as .gss files.
    
    @param :: vanFile The name of the file containing the 
    vanadium calibration run
    
    @param :: cerFile The name of the file containing the 
    cerium oxide calibration run
    
    @param :: inputFileList The list of files containing event
    workspaces to process and save
    
    @param :: bank Optional: The banks to use for calibration and focusing
    defaults to 'all' if not specified
    
    @param :: detectorIndicies Optional: The detectors to use for calibration and focusing
    """
    
    #Load to check all data exists
    #First vanadium and cerium calibration runs
    vanWs = loadVanCalib(vanFile)
    cerWs = loadCerCalib(cerFile)
    
    #Next perform the calibration
    calWs = calibrate(vanWs, cerWs, bank, detectorIndicies)
    
    #Save GSS param file out
    saveCalibration(calWs)

    #Load run data
    inputWorkspaces = loadRunData(inputFileList)
    
    #Rebin all event workspaces to histogram workspace
    rebinnedWorkspaces = rebinWorkspaces(inputWorkspaces)
    
    #Focus each workspace
    focusedWorkspaces = focusWorkspaces(rebinnedWorkspaces)
    
    #Finally save each run
    saveGssFiles(focusedWorkspaces)
    
def loadVanCalib(vanFilePath):
    """
    Attempts to load a vanadium calibration file at 
    the file path input
    @param :: vanFilePath The location of the vanadium run
    @returns :: The vanadium run workspace
    """    
    vanWs = mantidSapi.Load(vanFilePath)
    return vanWs
    
def loadCerCalib(cerFilePath):
    """
    Attempts to load a cerium oxide calibration file at
    the file path input
    @param :: cerFilePath The location of the cerium run
    @returns :: The cerium oxide run workspace
    """
    cerWs = mantidSapi.Load(cerFilePath)
    return cerWs
    
def loadRunData(fileList):
    """
    Attempts to load all runs specified in the run list into
    workspaces for later processing
    @param :: fileList The list of event workspaces to load
    @returns :: A list of loaded workspaces
    """
    loadedList = []
    for filePath in fileList:
        #Have to assign to WS then append that to list
        tempWs = mantidSapi.Load(filePath)
        loadedList.append(tempWs)
        
    return loadedList
    
def calibrate(vanWs, cerWs, bank, detectorIndicies):
    """
    Runs the calibration using the vanadium and cerium
    workspaces entered
    @param :: vanWs The workspace containing a vanadium run
    @param :: cerWs The workspace containing a cerium run
    @param :: bank The bank to run the calibration on
    @param :: detectorIndicies List/Range of detectors to calibrate
    @returns :: The calibrated workspace(s)
    """
    
    vanIntegralWs = "vanIntegralWs"
    vanCurvesWs = "vanCurvesWs"
       
    difc = []
    zero = []
       
    mantidSapi.EnggVanadiumCorrections(VanadiumWorkspace=vanWs, OutIntegrationWorkspace=vanIntegralWs, OutCurvesWorkspace=vanCurvesWs)
    
    calWsList = []
    
    if bank == "" and detectorIndicies == "":
        print ("Calibrating both")
        tempDifC = 0.0
        
        #Calibration for bank one
        calWsOne = mantidSapi.EnggCalibrate(
        InputWorkspace=cerWs, VanIntegrationWorkspace=vanIntegralWs, \
        VanCurvesWorkspace=vanCurvesWs, Bank='1', Difc=tempDifC)
        difc.append(tempDifc)
        calWsList.append(calWsOne)
        
        #Calibration for bank two
        calWsTwo = mantidSapi.EnggCalibrate(
        InputWorkspace=cerWs, VanIntegrationWorkspace=vanIntegralWs, \
        VanCurvesWorkspace=vanCurvesWs, Bank='2', Difc=tempDifC)
        difc.append(tempDifc)
        calWsList.append(calWsTwo)
        
    elif detectorIndicies == "":
        print ("Calibrating specified bank")
        calWs = mantidSapi.EnggCalibrate(InputWorkspace=cerWs, VanadiumWorkspace=vanWs, Bank=bank)
        calWsList.append(calWs)
        
    else:
        calDetList = mantidSapi.EnggCalibrate(InputWorkspace=cerWs, VanadiumWorkspace=vanWs, SpectrumNumbers=detectorIndicies)
        calWsList.append(calDetList)
    
    return calWsList
    
def saveCalibration(calWs):
    """
    Saves the calibration workspace as a parametrised .gss file
    @param :: calWs The calibrated workspace to save
    """
    
def rebinWorkspaces(inputWorkspaces):
    """
    Rebins the list of event workspaces into histogram workspaces
    @param :: inputWorkspaces The list of event workspaces to convertEventWsToHistoGss
    @returns :: A list of event workspaces 
    """
    
def focusWorkspaces(rebinnedWorkspaces):
    """
    Focuses the list of histogram workspaces using 'EnggFocus' 
    @param :: rebinnedWorkspaces The list of workspaces to focus
    @returns :: A list of focused workspaces 
    """
    
def saveGssFiles(focusedWorkspaces):
    """
    Takes a list of focused workspaces and saves them to Gss files
    @param :: focusWorkspaces The list of workspaces to save out
    """

#TODO remove hardcoded file paths used during testing 

import os

inList = []
for file in os.listdir("D:\\ENGINX Test Data\\SplitEvent"):
    inList.append("D:\\ENGINX Test Data\\SplitEvent\\" + file)
    print file
    
for list in inList:
    print(list)


convertEventWsToHistoGss(vanFile="ENGINX00246043.nxs",cerFile="ENGINX00249918.nxs", inputFileList=inList)