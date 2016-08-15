import mantid.api as mantidApi
import mantid.simpleapi as mantidSapi

def convertEventWsToHistoGss(vanFile, cerFile, inputFileList[]):
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
    """
    
    #Load to check all data exists
    #First vanadium and cerium calibration runs
    vanWs = loadVanCalib(vanFilePath)
    cerWs = loadCerCalib(cerFilePath)
    #Then all run data
    inputWorkspaces[] = loadRunData(fileList[])
    
    #Next perform the calibration
    calWs = calibrate(vanWs, cerWs)
    
    #Save GSS param file out
    saveCalibration(calWs)
    
    #Rebin all event workspaces to histogram workspaces
    rebinnedWorkspaces[] = rebinWorkspaces(inputWorkspaces[])
    
    #Focus each workspaces
    focusedWorkspaces[] = focusWorkspaces(rebinnedWorkspaces[])
    
    #Finally save each run
    saveGssFiles(focusedWorkspaces[])
    
def loadVanCalib(vanFilePath):
    """
    Attempts to load a vanadium calibration file at 
    the file path input
    @param :: vanFilePath The location of the vanadium run
    @returns :: The vanadium run workspace
    """    
    
def loadCerCalib(cerFilePath):
    """
    Attempts to load a cerium oxide calibration file at
    the file path input
    @param :: cerFilePath The location of the cerium run
    @returns :: The cerium oxide run workspace
    """
    
def loadRunData(fileList[]):
    """
    Attempts to load all runs specified in the run list into
    workspaces for later processing
    @param :: fileList The list of event workspaces to load
    @returns :: A list of loaded workspaces
    """
    
def calibrate(vanWs, cerWs):
    """
    Runs the calibration using the vanadium and cerium
    workspaces entered
    @param :: vanWs The workspace containing a vanadium run
    @param :: cerWs The workspace containing a cerium run
    @returns :: The calibrated workspace
    """
    
def saveCalibration(calWs):
    """
    Saves the calibration workspace as a parametrised .gss file
    @param :: calWs The calibrated workspace to save
    """
    
def rebinWorkspaces(inputWorkspaces[]):
    """
    Rebins the list of event workspaces into histogram workspaces
    @param :: inputWorkspaces The list of event workspaces to convertEventWsToHistoGss
    @returns :: A list of event workspaces 
    """
    
def focusWorkspaces(rebinnedWorkspaces[]):
    """
    Focuses the list of histogram workspaces using 'EnggFocus' 
    @param :: rebinnedWorkspaces The list of workspaces to focus
    @returns :: A list of focused workspaces 
    """
    
def saveGssFiles(focusedWorkspaces[]):
    """
    Takes a list of focused workspaces and saves them to Gss files
    @param :: focusWorkspaces The list of workspaces to save out
    """
