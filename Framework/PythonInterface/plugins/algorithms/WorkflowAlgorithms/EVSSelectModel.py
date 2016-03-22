from mantid.simpleapi import *
from mantid.api import *
from mantid.kernel import *
from vesuvio.commands import load_and_crop_data
import math
import numpy as np
import scipy

#==============================================================================

# Generated from: http://www.lfd.uci.edu/~gohlke/code/elements.py
MASSES = np.array([ 1.00794, 4.002602, 6.941, 9.012182, 10.811, 12.0107,
    14.0067, 15.9994, 18.9984032, 20.1797, 22.98977, 24.305, 26.981538,
    28.0855, 30.973761, 32.065, 35.453, 39.948, 39.0983, 40.078, 44.95591,
    47.867, 50.9415, 51.9961, 54.938049, 55.845, 58.9332, 58.6934, 63.546,
    65.409, 69.723, 72.64, 74.9216, 78.96, 79.904, 83.798, 85.4678, 87.62,
    88.90585, 91.224, 92.90638, 95.94, 97.907216, 101.07, 102.9055, 106.42,
    107.8682, 112.411, 114.818, 118.71, 121.76, 127.6, 126.90447, 131.293,
    132.90545, 137.327, 138.9055, 140.116, 140.90765, 144.24, 144.912744,
    150.36, 151.964, 157.25, 158.92534, 162.5, 164.93032, 167.259, 168.93421,
    173.04, 174.967, 178.49, 180.9479, 183.84, 186.207, 190.23, 192.217,
    195.078, 196.96655, 200.59, 204.3833, 207.2, 208.98038, 208.982416,
    209.9871, 222.0176, 223.0197307, 226.025403, 227.027747, 232.0381,
    231.03588, 238.02891, 237.048167, 244.064198, 243.061373, 247.070347,
    247.070299, 251.07958, 252.08297, 257.095099, 258.098425, 259.10102,
    262.10969, 261.10875, 262.11415, 266.12193, 264.12473, 269.13411,
    268.13882])

FINAL_ENERGY = 4897 # meV
FINAL_VELOCITY = math.sqrt(FINAL_ENERGY / (5.2276 * scipy.constants.micro))
NEUTRON_MASS_AMU = scipy.constants.value("neutron mass in u")

#==============================================================================

class EVSSelectModel(PythonAlgorithm):

    _instrument_params = None
    _mass_tof_positions = None
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

        # Cache positions of each mass if time of flight
        self._mass_tof_positions = {}
        for p in self._detected_peaks:
            spec = p[0]
            if spec not in self._mass_tof_positions:
                self._cache_mass_tof(spec)

        print self._mass_tof_positions

        # Model generation
        for p in self._detected_peaks:
            spec, tof, width = p

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

#------------------------------------------------------------------------------

    def _cache_mass_tof(self, spec):
        spec_idx = spec - 1

        t_0 = self._instrument_params['t0'][spec_idx] * scipy.constants.micro
        l_0 = self._instrument_params['L0'][spec_idx]
        l_1 = self._instrument_params['L1'][spec_idx]
        theta = self._instrument_params['theta'][spec_idx]

        r_t = (np.cos(theta) + np.sqrt((MASSES / NEUTRON_MASS_AMU)**2 - np.sin(theta)**2)) / ((MASSES / NEUTRON_MASS_AMU) + 1)
        tof = (((l_0 * r_t) + l_1) / FINAL_VELOCITY) / scipy.constants.micro

        self. _mass_tof_positions[spec] = tof

#==============================================================================

# Register algorithm with Mantid
AlgorithmFactory.subscribe(EVSSelectModel)
