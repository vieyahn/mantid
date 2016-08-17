import mantid.api as mantidApi
import mantid.simpleapi as mantidSapi

def convert_event_ws_to_histo_gss(van_file, cer_file, input_file_list, bank="", detector_indicies="", gsas_cal_fname="", out_fname_pattern="" ):
    """
    Runs a calibration using the input vanadium and cerium oxide
    runs then saves this as a .prm file. Using this calibration
    it then converts the list of event workspaces to histogram
    workspaces and saves them out as .gss files.

    @param :: van_file The name of the file containing the
     vanadium calibration run

    @param :: cer_file The name of the file containing the
     cerium oxide calibration run

    @param :: input_file_list The list of files containing event
     workspaces to process and save

    @param :: bank Optional: The banks to use for calibration and focusing
     defaults to 'all' if not specified

    @param :: detector_indicies Optional: The detectors to use for focusing
    
    @param :: gsas_cal_fname Optional: Allows the user to specify a filename and path 
    for the GSAS calibration file
    
    @param :: out_fname_pattern Optional:: Allows the user to specify a custom naming
    scheme for the focused files. This will have the suffix '_<fileNumber>' if a list
    of files is used where the 2nd file on the list would be '<out_fname_pattern>_2'
    """

    #Load to check all data exists
    #First vanadium and cerium calibration runs
    van_ws = load_van_calib(van_file)
    cer_ws = load_cer_calib(cer_file)

    #Next perform the calibration
    calibrate(van_ws=van_ws, cer_ws=cer_ws, bank=bank, gsas_cal_fname=gsas_cal_fname)

    #Load run data
    input_workspaces = load_run_data(input_file_list)

    #Rebin all event workspaces to histogram workspace
    rebinned_workspaces = rebin_workspaces(input_workspaces)

    #Focus each workspace
    focused_workspaces = focus_workspaces(rebinned_workspaces)

    #Finally save each run
    save_gss_files(focused_workspaces)

def load_van_calib(van_filePath):
    """
    Attempts to load a vanadium calibration file at
     the file path input
    @param :: van_filePath The location of the vanadium run
    @returns :: The vanadium run workspace
    """
    van_ws = mantidSapi.Load(van_filePath)
    return van_ws

def load_cer_calib(cer_filePath):
    """
    Attempts to load a cerium oxide calibration file at
     the file path input
    @param :: cer_filePath The location of the cerium run
    @returns :: The cerium oxide run workspace
    """
    cer_ws = mantidSapi.Load(cer_filePath)
    return cer_ws

def load_run_data(file_list):
    """
    Attempts to load all runs specified in the run list into
    workspaces for later processing
    @param :: file_list The list of event workspaces to load
    @returns :: A list of loaded workspaces
    """
    loadedList = []
    for filePath in file_list:
        #Have to assign to WS then append that to list
        tempWs = mantidSapi.Load(filePath)
        loadedList.append(tempWs)

    return loadedList

def calibrate(van_ws, cer_ws, bank, gsas_cal_fname):
    """
    Runs the calibration using the vanadium and cerium
    workspaces entered
    @param :: van_ws The workspace containing a vanadium run
    @param :: cer_ws The workspace containing a cerium run
    @param :: bank The bank to run the calibration on
    @param :: detector_indicies List/Range of detectors to calibrate
    @param :: gsas_cal_fname The filename to save the GSAS param file to
    """

    van_integral_ws = "van_integral_ws"
    van_curves_ws = "van_curves_ws"

    #Apply vanadium correction before calibration 
    mantidSapi.EnggVanadiumCorrections(
        VanadiumWorkspace=van_ws, OutIntegrationWorkspace=van_integral_ws, OutCurvesWorkspace=van_curves_ws)

    # Create lists for multiple return values which can be n length long

    difA_cal = []
    difC_cal = []
    tZero_cal = []
    fitted_peaks_cal = []
    
        
    if bank == "":
        print "Calibrating both"
        for i in range(2):
            # Calibration for bank one and two
            # We could use zip() however EnggCalibrate fails if we nest it so just use longer method
            tmp_a, tmp_c, tmp_zero, tmp_fitted = mantidSapi.EnggCalibrate(
                InputWorkspace=cer_ws, VanIntegrationWorkspace=van_integral_ws,
                VanCurvesWorkspace=van_curves_ws, Bank=str(i+1))
            
            
            # We only need difc and tZero so comment out the other lines if they need them
            difC_cal.append(tmp_c)
            tZero_cal.append(tmp_zero)
            #tmp_fitted.append(tmp_fitted)
            #difA_cal.append(tmp_a)
        
        # Next set bank for when we save to GSAS
        bank = ['North', 'South']
            
    else:
        print "Calibrating specified bank"
        difA_cal, difC_cal, tZero_cal, fitted_peaks_cal = mantidSapi.EnggCalibrate(InputWorkspace=cer_ws, VanadiumWorkspace=van_ws, Bank=bank)
        
        
    #Finally save out calibration
    import EnggUtils
    EnggUtils.write_ENGINX_GSAS_iparam_file(
        output_file=gsas_cal_fname, difc=difC_cal, tzero=tZero_cal, bank_names=bank, 
        ceria_run=cer_ws, vanadium_run=van_ws)
    
        

def rebin_workspaces(input_workspaces):
    """
    Rebins the list of event workspaces into histogram workspaces
    @param :: input_workspaces The list of event workspaces to convertEventWsToHistoGss
    @returns :: A list of event workspaces
    """

def focus_workspaces(rebinned_workspaces):
    """
    Focuses the list of histogram workspaces using 'EnggFocus'
    @param :: rebinned_workspaces The list of workspaces to focus
    @returns :: A list of focused workspaces
    """

def save_gss_files(focused_workspaces):
    """
    Takes a list of focused workspaces and saves them to Gss files
    @param :: focus_workspaces The list of workspaces to save out
    """

#TODO remove hardcoded file paths used during testing

import os

inList = []
for file in os.listdir("D:\\ENGINX Test Data\\SplitEvent"):
    inList.append("D:\\ENGINX Test Data\\SplitEvent\\" + file)
    print file

for list in inList:
    print list


convert_event_ws_to_histo_gss(van_file="ENGINX00246043.nxs",cer_file="ENGINX00249918.nxs", input_file_list=inList)
