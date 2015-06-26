import unittest
from mantid.simpleapi import *
from mantid.api import *

class EnginXCalibrateFullTest(unittest.TestCase):

    _data_ws = None

    # Note not using @classmethod setUpClass / tearDownClass because that's not supported in the old
    # unittest of rhel6. setUpClass would do LoadNexus(...) and then tearDownClass DeleteWorkspace(...)
    def setUp(self):
        """
        Set up dependencies for one or more of the tests below.
        """
        if not self.__class__._data_ws:
            self.__class__._data_ws = LoadNexus("ENGINX00228061.nxs", OutputWorkspace='ENGIN-X_test_ws')

    def test_issues_with_properties(self):
        """
        Handle in/out property errors appropriately.
        """

        # No Filename property (required)
        self.assertRaises(RuntimeError,
                          EnginXCalibrateFull,
                          Input='foo', Bank=1)

        # Wrong workspace name
        self.assertRaises(RuntimeError,
                          EnginXCalibrateFull,
                          Input='this_ws_is_not_there.not', Bank=2)

        # mispelled ExpectedPeaks
        self.assertRaises(RuntimeError,
                          EnginXCalibrateFull,
                          InputWorkspace=self.__class__._data_ws, Bank=2, Peaks='2')

        # all fine, except missing DetectorPositions (output)
        self.assertRaises(RuntimeError,
                          EnginXCalibrateFull,
                          InputWorkspace=self.__class__._data_ws, Bank=2)

    def test_wrong_fit_fails_gracefully(self):
        """
        Checks a bad run fails reasonably.
        """

        # This should produce fitting 'given peak center ... is outside of data range'
        # warnings and finally raise after a 'some peaks not found' error
        self.assertRaises(RuntimeError,
                          EnginXCalibrateFull,
                          InputWorkspace=self.__class__._data_ws, ExpectedPeaks=[0.01], Bank=1,
                          DetectorPositions='out_det_positions_table')


    def test_run_ok_but_bad_data(self):
        """
        Tests a run that doesn't go well becaus of inappropriate data is used here.
        """

        # This is not a realistic CalibrateFull run, but it just runs fine
        # for testing purposes. A test with real (much larger) data that produces
        # a correct fit is included in system tests
        tbl_name = 'det_peaks_tbl'
        det_peaks_tbl = CreateEmptyTableWorkspace()
        self.assertRaises(RuntimeError,
                          EnginXCalibrateFull,
                          InputWorkspace=self.__class__._data_ws, Bank=2,
                          ExpectedPeaks='0.915, 1.257, 1.688',
                          DetectorPositions=tbl_name)



if __name__ == '__main__':
    unittest.main()
