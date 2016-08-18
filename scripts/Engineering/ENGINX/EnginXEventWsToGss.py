import mantid.api as mantidApi
import mantid.simpleapi as mantidSapi

#pylint: disable=too-many-arguments
def convert_event_ws_to_histo_gss(van_file, cer_file, input_file_list, bank="", detector_indicies="",
                                  gsas_cal_fname="", out_fname_pattern="", out_f_dir="" ):
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

    @param :: out_fname_pattern Optional: Allows the user to specify a custom naming
     scheme for the focused files. This will have the suffix '_<fileNumber>' if a list
     of files is used where the 2nd file on the list would be '<out_fname_pattern>_2'

    @param :: out_f_dir Optional: Allows the user to select where to save output files
    """
    
    #Next perform the calibration
    van_curves, van_integral = van_calibrate(van_file_name=van_file, cer_file_name=cer_file, bank=bank, gsas_cal_fname=gsas_cal_fname)

    for input_file in input_file_list
        #Load run data
        input_workspace = load_run_data(input_file)
    
        #Rebin all event workspaces to histogram workspace
        rebinned_workspace = rebin_workspaces(input_workspace)
    
        #Focus each workspace
        focused_workspace = focus_workspaces(rebinned_workspace)
    
        #Finally save each run
        save_gss_files(focused_workspace)

def load_van_calib(van_file_name):
    """
    Attempts to load a vanadium calibration file at
     the file path input
    @param :: van_file_name The name of the vanadium run
    """
    mantidSapi.Load(Filename=van_file_name, OutputWorkspace=van_file_name)

def load_cer_calib(cer_file_name):
    """
    Attempts to load a cerium oxide calibration file at
     the file path input
    @param :: cer_file_name The name of the cerium run
    """
    mantidSapi.Load(Filename=cer_file_name, OutputWorkspace=cer_file_name)  
    
def calibrate(van_file_name, cer_file_name, bank, gsas_cal_fname):
    """
    Runs the calibration using the vanadium and cerium
    workspaces entered
    @param :: van_file_name The filename of the vanadium run
    @param :: cer_file_name The filename of the cerium run
    @param :: bank The bank to run the calibration on
    @param :: detector_indicies List/Range of detectors to calibrate
    @param :: gsas_cal_fname The filename to save the GSAS param file to
    @returns :: The vanadium curves and integration workspaces
    """

        #Load to check all data exists
    #First vanadium and cerium calibration runs
    load_van_calib(van_file_name)
    load_cer_calib(cer_file_name)

    van_integral_ws = "van_integral_ws"
    van_curves_ws = "van_curves_ws"

    #Apply vanadium correction before calibration
    mantidSapi.EnggVanadiumCorrections(
        VanadiumWorkspace=van_file_name, OutIntegrationWorkspace=van_integral_ws, OutCurvesWorkspace=van_curves_ws)

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
                InputWorkspace=cer_file_name, VanIntegrationWorkspace=van_integral_ws,
                VanCurvesWorkspace=van_curves_ws, Bank=str(i+1))

    else:
        print "Calibrating specified bank"
        tmp_a, tmp_c, tmp_zero, tmp_fitted = mantidSapi.EnggCalibrate(
                InputWorkspace=cer_file_name, VanIntegrationWorkspace=van_integral_ws,
                VanCurvesWorkspace=van_curves_ws, Bank=bank)
                
    # We only need difc and tZero so comment out the other lines if they need them
    difC_cal.append(tmp_c)
    tZero_cal.append(tmp_zero)
    #tmp_fitted.append(tmp_fitted)
    #difA_cal.append(tmp_a)
    
    #Strip the file names so they are run number only
    van_run_number = ''.join(i for i in van_file_name if i.isdigit())
    cer_run_number = ''.join(i for i in cer_file_name if i.isdigit())
    
    #Then strip any leading zeros
    van_run_number = van_run_number.lstrip("0")
    cer_run_number = cer_run_number.lstrip("0")
    
    #EnggUtils expects a bank list
    bank_list = []
    
    #If calibration save file name not specified generate it
    if gsas_cal_fname == "":
        if bank == "":
            gsas_cal_fname='ENGINX_' + van_run_number + '_' + cer_run_number + '_all_banks.prm'
            
        else:
            gsas_cal_fname='ENGINX_' + van_run_number + '_' + cer_run_number + '_bank_' + str(bank)
            # Cast bank to list for EnggUtils
            bank_list.append(bank)

    # Save out calibration
    import EnggUtils
    EnggUtils.write_ENGINX_GSAS_iparam_file(
        output_file=gsas_cal_fname, difc=difC_cal, tzero=tZero_cal, bank_names=bank_list,
        ceria_run=cer_file_name, vanadium_run=van_file_name)
        
    #To end tidy up any workspaces
    #First hard coded output from enggCalibrate
    mantidSapi.DeleteWorkspace("engg_fit_ws_dsp")
    mantidSapi.DeleteWorkspace("engg_van_ws_dsp")
    
    #tmp created workspaces
    mantidSapi.DeleteWorkspace(tmp_fitted)
   
    #Loaded calibration workspaces
    mantidSapi.DeleteWorkspace(van_file_name)
    mantidSapi.DeleteWorkspace(cer_file_name)
    
    return van_curves_ws, van_integral_ws
    
    
def load_run_data(file_list):
    """
    Attempts to load the run specified into a workspace 
    for later processing
    @param :: file_list The list of event workspaces to load
    @returns :: The loaded workspaces
    """
    mantidSapi.Load(InputWorkspace=file_path, OutputWorkspace=file_path)
    
    return file_path
    
def rebin_workspaces(input_workspace):
    """
    Rebins the event workspace into histogram workspace
    @param :: input_workspace The event workspaces to convertEventWsToHistoGss
    @returns :: Event workspaces
    """
    
    
def focus_workspaces(rebinned_workspace):
    """
    Focuses the histogram workspace using 'EnggFocus'
    @param :: rebinned_workspace Workspace to focus
    @returns :: The focused workspace
    """

def save_gss_files(focused_workspace):
    """
    Takes a focused workspace and saves it to a GS files
    @param :: focus_workspace The workspace to save out
    """

#TODO remove hardcoded file paths used during testing

import os

inList = []
for file in os.listdir("D:\\ENGINX Test Data\\SplitEvent"):
    inList.append(file)
    print file

for list in inList:
    print list


convert_event_ws_to_histo_gss(van_file="ENGINX00246043.nxs",cer_file="ENGINX00249918.nxs", input_file_list=inList, bank="North")
