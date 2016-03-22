from mantid.simpleapi import *
from mantid.api import *
from mantid.kernel import *
from vesuvio.commands import load_and_crop_data
import math
import numpy as np
import scipy

#==============================================================================

NEUTRON_MASS_AMU = scipy.constants.value("neutron mass in u")

def calc_r_t(mass, theta, rt_tar):
    aa = (mass / NEUTRON_MASS_AMU)**2 - math.sin(theta)**2
    num = math.cos(theta) + math.sqrt(aa)
    denom = (mass / NEUTRON_MASS_AMU) + 1
    return (num / denom) - rt_tar

#==============================================================================

class EVSSelectModel(PythonAlgorithm):

    _instrument_params = None
    _detected_peaks = None

#------------------------------------------------------------------------------

    def summary(self):
        return 'Select the most probable fitting model for VESUVIO data.'

    def category(self):
        return "Inelastic\\Indirect\\Vesuvio"

#------------------------------------------------------------------------------

    def PyInit(self):
        self.declareProperty(name='Runs', defaultValue='',
                             doc='Sample run range')

        self.declareProperty(name='FitMode', defaultValue='spectra',
                             validator=StringListValidator(['spectra', 'bank']),
                             doc='Fitting mode')

        self.declareProperty(name='Spectra', defaultValue='',
                             doc='Spectra range or bank')

        self.declareProperty(name='DiffMode', defaultValue='single',
                             validator=StringListValidator(['single', 'double']),
                             doc='Differencing mode')

        self.declareProperty(name='BinParameters', defaultValue='',
                             doc='Input binning parameters')

        self.declareProperty(FileProperty('IPFile', '', action=FileAction.OptionalLoad,
                                          extensions=['dat', 'par']),
                             doc='Instrument parameter file')

#------------------------------------------------------------------------------

    def validateInputs(self):
        issues = dict()

        for p_name in ['Runs', 'Spectra']:
            if self.getPropertyValue(p_name) == '':
                issues[p_name] = 'Property must have a single value or range'

        return issues

#------------------------------------------------------------------------------

    def PyExec(self):
        # Load instrument and parameters
        ip_file = self.getPropertyValue('IPFile');

        self._load_instrument_params(ip_file)

        bin_params = self.getPropertyValue('BinParameters')
        if bin_params == "":
            bin_params = None

        sample_data = load_and_crop_data(self.getPropertyValue('Runs'),
                                         self.getPropertyValue('Spectra'),
                                         ip_file,
                                         self.getPropertyValue('DiffMode'),
                                         self.getPropertyValue('FitMode'),
                                         bin_params)

        # Initial peak finding
        self._find_peaks(sample_data)

        # Model generation
        for p in self._detected_peaks:
            spec, tof, width = p
            spec_idx = spec - 1

            tof *= scipy.constants.micro

            t_0 = self._instrument_params['t0'][spec_idx] * scipy.constants.micro
            l_0 = self._instrument_params['L0'][spec_idx]
            l_1 = self._instrument_params['L1'][spec_idx]
            theta = self._instrument_params['theta'][spec_idx]

            e_1 = 4900
            v_1 = math.sqrt(e_1 / (5.2276 * scipy.constants.micro))
            r_t = (v_1 * (tof - t_0) - l_1) / l_0

            try:
                res = scipy.optimize.fsolve(calc_r_t, NEUTRON_MASS_AMU, args=(theta, r_t))
                print p, res
            except ValueError:
                logger.warn('Failed to obtain mass for ')

            # TODO

#------------------------------------------------------------------------------

    def _load_instrument_params(self, par_file):
        # Load data
        param_names = ['spectrum', 'theta', 't0', 'L0', 'L1']
        file_data = np.loadtxt(par_file, skiprows=1, usecols=[0, 2, 3, 4, 5], unpack=True)

        self._instrument_params = {}
        for name, column in zip(param_names, file_data):
            self._instrument_params[name] = column

        # Convert theta to radians
        self._instrument_params['theta'] = np.radians(self._instrument_params['theta'])

#------------------------------------------------------------------------------

    def _find_peaks(self, sample_data):
        peak_table = FindPeaks(InputWorkspace=sample_data,
                               PeakFunction='Gaussian',
                               FWHM=25)

        self._detected_peaks = list()
        for row in range(peak_table.rowCount()):
            ws_idx = peak_table.cell('spectrum', row)
            spectrum = sample_data.getSpectrum(ws_idx).getSpectrumNo()
            centre = peak_table.cell('centre', row)
            width = peak_table.cell('width', row)
            self._detected_peaks.append((spectrum, centre, width))

        DeleteWorkspace(peak_table)

#==============================================================================

# Register algorithm with Mantid
AlgorithmFactory.subscribe(EVSSelectModel)
