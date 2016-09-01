from mantid import config
import mantid.simpleapi as mantidSapi

# pylint: disable=too-many-arguments
def convert_event_ws_to_histo_gss(van_file, cer_file, input_file_list, bank="", detector_indicies="",
                                  gsas_cal_fname="", out_fname_pattern="", out_f_dir="", rebin_param=""):
    """
    Runs a calibration using the input vanadium and cerium oxide
    runs then saves this as a .prm file. Using this calibration
    it then converts the list of event workspaces to histogram
    workspaces and saves them out as focused XYE files.

    @param :: van_file The name of the file containing the
     vanadium calibration run

    @param :: cer_file The name of the file containing the
     cerium oxide calibration run

    @param :: input_file_list The list of files containing event
     workspaces to process and save

    @param :: bank Optional: The banks to use for calibration and focusing
     defaults to 'all' if not specified - Not fully implemented, only affects calibration

    @param :: detector_indicies Optional: The detectors to use for focusing

    @param :: gsas_cal_fname Optional: Allows the user to specify a filename and path
     for the GSAS calibration file

    @param :: out_fname_pattern Optional: Allows the user to specify a custom naming
     scheme for the focused files. This will have the suffix '_<fileNumber>' if a list
     of files is used where the 2nd file on the list would be '<out_fname_pattern>_2' - Not implemented yet

    @param :: out_f_dir Optional: Allows the user to select where to save output files

    @param :: rebin_param Optional: Allows the user to override the rebin parameters
    """

    # Next perform the calibration
    van_curves, van_integral = _calibrate(van_file_name=van_file, cer_file_name=cer_file,\
    bank=bank, gsas_cal_fname=gsas_cal_fname)

    for input_file in input_file_list:
        # Load run data
        input_ws = _load_run_data(input_file)

        # Rebin all event workspaces to histogram workspace
        rebinned_ws = _convert_workspace(input_ws, rebin_param)

        # Focus each workspace
        focused_ws_list = _focus_workspace(rebinned_workspace=rebinned_ws,\
            van_integral_ws=van_integral, van_curves_ws=van_curves)

        # Finally save each run
        _save_focused_xye_file(focused_workspace=focused_ws_list, save_path=out_f_dir)

        # Tidy all to save memory space
        delete_list = [input_ws, rebinned_ws]
        delete_list.extend(focused_ws_list)
        _delete_ws_list(delete_list)

    # Remove the vanadium curves and integral
    delete_list = [van_curves, van_integral]
    _delete_ws_list(delete_list)

def _load_van_calib(van_file_name):
    """
    Attempts to load a vanadium calibration file at
     the file path input
    @param :: van_file_name The name of the vanadium run
    """
    mantidSapi.Load(Filename=van_file_name, OutputWorkspace=van_file_name)

def _load_cer_calib(cer_file_name):
    """
    Attempts to load a cerium oxide calibration file at the file path input
    @param :: cer_file_name The name of the cerium run
    """
    mantidSapi.Load(Filename=cer_file_name, OutputWorkspace=cer_file_name)

def _calibrate(van_file_name, cer_file_name, bank, gsas_cal_fname):
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

        # Load to check all data exists
    # First vanadium and cerium calibration runs
    _load_van_calib(van_file_name)
    _load_cer_calib(cer_file_name)

    van_integral_ws = "van_integral_ws"
    van_curves_ws = "van_curves_ws"

    # Apply vanadium correction before calibration
    mantidSapi.EnggVanadiumCorrections(
        VanadiumWorkspace=van_file_name, OutIntegrationWorkspace=van_integral_ws,\
        OutCurvesWorkspace=van_curves_ws)

    # Create lists for multiple return values which can be n length long

    difA_cal = []
    difC_cal = []
    tZero_cal = []
    fitted_peaks_cal = []


    if not bank:
        print "Calibrating both banks"
        for i in range(2):
            # TODO wrap in its own function

            # Calibration for bank one and two
            # We could use zip() however EnggCalibrate fails if we nest it so just use longer method
            tmp_a, tmp_c, tmp_zero, tmp_fitted = mantidSapi.EnggCalibrate(
                InputWorkspace=cer_file_name, VanIntegrationWorkspace=van_integral_ws,
                VanCurvesWorkspace=van_curves_ws, Bank=str(i+1))

            # We only need difc and tZero so comment out the other lines if they need them
            difC_cal.append(tmp_c)
            tZero_cal.append(tmp_zero)
            # tmp_fitted.append(tmp_fitted)
            # difA_cal.append(tmp_a)

    else:
        print "Calibrating specified bank"
        tmp_a, tmp_c, tmp_zero, tmp_fitted = mantidSapi.EnggCalibrate(
            InputWorkspace=cer_file_name, VanIntegrationWorkspace=van_integral_ws,
            VanCurvesWorkspace=van_curves_ws, Bank=bank)

        # We only need difc and tZero so comment out the other lines if they need them
        difC_cal.append(tmp_c)
        tZero_cal.append(tmp_zero)
        # tmp_fitted.append(tmp_fitted)
        # difA_cal.append(tmp_a)


    # Strip the file names so they are run number only
    van_run_number = ''.join(i for i in van_file_name if i.isdigit())
    cer_run_number = ''.join(i for i in cer_file_name if i.isdigit())

    # Then strip any leading zeros
    van_run_number = van_run_number.lstrip("0")
    cer_run_number = cer_run_number.lstrip("0")

    # EnggUtils expects a bank list
    bank_list = []

    # If calibration save file name not specified generate it
    if gsas_cal_fname == "":
        if bank == "":
            gsas_cal_fname = 'ENGINX_' + van_run_number + '_' + cer_run_number + '_all_banks.prm'

        else:
            gsas_cal_fname = 'ENGINX_' + van_run_number + '_' + cer_run_number + '_bank_' + str(bank)
            # Cast bank to list for EnggUtils
            bank_list.append(bank)

    # Save out to last directory in list of data search directories which should be output
    data_search_dirs = mantid.config.getDataSearchDirs()
    full_cal_output_path = data_search_dirs[-1] + gsas_cal_fname
            
    print "Output calibration filename " + gsas_cal_fname
            
    # Save out calibration
    import EnggUtils
    EnggUtils.write_ENGINX_GSAS_iparam_file(
        output_file=full_cal_output_path, difc=difC_cal, tzero=tZero_cal, bank_names=bank_list,
        ceria_run=cer_file_name, vanadium_run=van_file_name)

    # To end tidy up any workspaces
    # First hard coded output from enggCalibrate
    mantidSapi.DeleteWorkspace("engg_fit_ws_dsp")
    mantidSapi.DeleteWorkspace("engg_van_ws_dsp")

    # tmp created workspaces
    mantidSapi.DeleteWorkspace(tmp_fitted)

    # Loaded calibration workspaces
    mantidSapi.DeleteWorkspace(van_file_name)
    mantidSapi.DeleteWorkspace(cer_file_name)

    return van_curves_ws, van_integral_ws

def _load_run_data(file_path):
    """
    Attempts to load the run specified into a workspace
    for later processing
    @param :: file_path The path of the event workspaces to load
    @returns :: The loaded workspace
    """

    # Trim any trailing '.nxs' if present
    loaded_ws_name = os.path.basename(file_path)
    if loaded_ws_name.endswith('.nxs'):
        loaded_ws_name = loaded_ws_name[:-4]

    mantidSapi.Load(Filename=file_path, OutputWorkspace=loaded_ws_name)

    return loaded_ws_name

def _convert_workspace(input_workspace, params):
    """
    Rebins the event workspace and converts into an histogram workspace
    @param :: input_workspace The event workspaces to convertEventWsToHistoGss
    @param :: params The parameters to use during rebin, if blank default
    is selected
    @returns :: Event workspaces
    """
    rebin_workspace = input_workspace + '_rebin'
    if params == "":
        # Use hardcoded default rebin parameters

        # 0.5 - 4 Angstroms * 18400 (Average difC value)
        # Therefore Domain: 9200 <= D => 73600
        params = "9200, -0.0005, 73600"
    mantidSapi.Rebin(InputWorkspace=input_workspace, OutputWorkspace=rebin_workspace, \
        Params=params, PreserveEvents=False)

    return rebin_workspace

def _focus_workspace(rebinned_workspace, van_integral_ws, van_curves_ws):
    """
    Focuses the histogram workspace using 'EnggFocus'
    @param :: rebinned_workspace Workspace to focus
    @param :: The generated vanadium integral workspaces
    @param :: The workspace containing the vanadium curves
    @returns :: A list of focused workspaces
    """

    # Trim 'rebin' first as we use string names and "<name>_rebin" will be passed in
    rebin_workspace_name = rebinned_workspace
    if rebin_workspace_name.endswith('_rebin'):
        rebin_workspace_name = rebin_workspace_name[:-6]
    rebin_workspace_name = rebin_workspace_name + '_focussed'

    # Next gen file name for banks one and two
    bank_one_ws = rebin_workspace_name + '_bank_1'
    bank_two_ws = rebin_workspace_name + '_bank_2'

    # Focus both workspaces
    # TODO respect user input for banks
    mantidSapi.EnggFocus(InputWorkspace=rebinned_workspace, OutputWorkspace=bank_one_ws,\
        VanIntegrationWorkspace=van_integral_ws, VanCurvesWorkspace=van_curves_ws, Bank='1')

    mantidSapi.EnggFocus(InputWorkspace=rebinned_workspace, OutputWorkspace=bank_two_ws,\
        VanIntegrationWorkspace=van_integral_ws, VanCurvesWorkspace=van_curves_ws, Bank='2')

    focus_list = []
    focus_list.append(bank_one_ws)
    focus_list.append(bank_two_ws)

    return focus_list

def _save_focused_xye_file(focused_workspace, save_path):
    """
    Takes a focused workspace list and saves it to a focused XYE file
    @param :: focus_workspace The workspace to save out
    @param :: save_path The path to save the files in
    """
    for ws_name in focused_workspace:
        out_file_name = save_path + ws_name + '.ascii'
        mantidSapi.SaveFocusedXYE(InputWorkspace=ws_name, Filename=out_file_name, SplitFiles=False)

def _delete_ws_list(workspace_list):
    """
    Takes a list of workspaces and deletes the from the ADS
    @param :: workspace_list The workspaces to deleter
    """
    for workspace in workspace_list:
        mantidSapi.DeleteWorkspace(Workspace=workspace)

# ---------------------------------

# TODO remove hardcoded file paths used during testing

import os

in_list = []
target_dir = "D:\\ENGINX Test Data\\SplitEvent"
for filename in os.listdir(target_dir):
    in_list.append(target_dir + "\\" + filename)
    print "Found " + filename


convert_event_ws_to_histo_gss(van_file="ENGINX00246043.nxs", cer_file="ENGINX00249918.nxs", input_file_list=in_list)
