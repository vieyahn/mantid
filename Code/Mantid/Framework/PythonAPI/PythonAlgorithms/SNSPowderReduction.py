from MantidFramework import *
from mantidsimple import *
import os

COMPRESS_TOL_TOF = .01

class SNSPowderReduction(PythonAlgorithm):
    class PDConfigFile(object):
        class PDInfo:
            """Inner class for holding configuration information for a reduction."""
            def __init__(self, data, has_dspace=False):
                if data is None:
                    data = [None, None, 1, 0, 0, 0., 0.]
                self.freq = data[0]
                self.wl = data[1]
                self.bank = int(data[2])
                self.van = int(data[3])
                self.can = int(data[4])
                self.has_dspace = has_dspace
                if has_dspace:
                    self.dmin = data[5]
                    self.dmax = data[6]
                    if len(data) > 7:
                        self.tmin = data[7]
                        if len(data) > 8:
                            self.tmax = data[8]
                else:
                    self.tmin = data[5] * 1000. # convert to microseconds
                    self.tmax = data[6] * 1000.
    
        def __init__(self, filename):
            if len(filename.strip()) <= 0:
                filename = None
            self.filename = filename
            self._data = {}
            self.use_dspace = False
            if self.filename is None:
                return
            handle = file(filename, 'r')
            for line in handle.readlines():
                self._addData(line)
        def _addData(self, line):
            if line.startswith('#') or len(line.strip()) <= 0:
                if "d_min" in line and "d_max" in line:
                    self.use_dspace = True
                return
            data = line.strip().split()
            data = [float(i) for i in data]
            if data[0] not in self._data.keys():
                self._data[data[0]]={}
            info = self.PDInfo(data, self.use_dspace)
            self._data[info.freq][info.wl]=info
        def __getFrequency(self, request):
            for freq in self._data.keys():
                if 100. * abs(float(freq)-request)/request < 5.:
                    return freq
            raise RuntimeError("Failed to find frequency: %fHz" % request)
    
        def __getWavelength(self, frequency, request):
            for wavelength in self._data[frequency].keys():
                if 100. * abs(wavelength-request)/request < 5.:
                    return wavelength
            raise RuntimeError("Failed to find wavelength: %fAngstrom" % request)
    
        def getInfo(self, frequency, wavelength):
            #print "getInfo(%f, %f)" % (frequency, wavelength)
            if self.filename is not None:
                frequency = self.__getFrequency(float(frequency))
                wavelength = self.__getWavelength(frequency, float(wavelength))
        
                return self._data[frequency][wavelength]
            else:
                return self.PDInfo(None)

    def category(self):
        return "Diffraction"

    def name(self):
        return "SNSPowderReduction"

    def PyInit(self):
        instruments = ["PG3", "VULCAN", "SNAP", "NOM"]
        self.declareProperty("Instrument", "PG3",
                             Validator=ListValidator(instruments))
        #types = ["Event preNeXus", "Event NeXus"]
        #self.declareProperty("FileType", "Event NeXus",
        #                     Validator=ListValidator(types))
        self.declareListProperty("RunNumber", [0], Validator=ArrayBoundedValidator(Lower=0))
        extensions = [ "_histo.nxs", "_event.nxs", "_neutron_event.dat",
                      "_neutron0_event.dat", "_neutron1_event.dat", "_neutron0_event.dat and _neutron1_event.dat"]
        self.declareProperty("Extension", "_event.nxs",
                             Validator=ListValidator(extensions))
        self.declareProperty("CompressOnRead", False,
                             Description="Compress the event list when reading in the data")
        self.declareProperty("Sum", False,
                             Description="Sum the runs. Does nothing for characterization runs")
        self.declareProperty("BackgroundNumber", 0, Validator=BoundedValidator(Lower=0),
                             Description="If specified overrides value in CharacterizationRunsFile")
        self.declareProperty("VanadiumNumber", 0, Validator=BoundedValidator(Lower=0),
                             Description="If specified overrides value in CharacterizationRunsFile")
        self.declareFileProperty("CalibrationFile", "", FileAction.Load,
                                 [".cal"])
        self.declareFileProperty("CharacterizationRunsFile", "", FileAction.OptionalLoad,
                                 ['.txt'],
                                 Description="File with characterization runs denoted")
        self.declareProperty("UnwrapRef", 0., 
                             Description="Reference total flight path for frame unwrapping. Zero skips the correction")
        self.declareProperty("LowResRef", 0., 
                             Description="Reference DIFC for resolution removal. Zero skips the correction")
        self.declareProperty("RemovePromptPulseWidth", 0.0,
                             Description="Width of events (in microseconds) near the prompt pulse to remove. 0 disables")
        self.declareProperty("FilterByTimeMin", 0.,
                             Description="Relative time to start filtering by in seconds. Applies only to sample.")
        self.declareProperty("FilterByTimeMax", 0.,
                             Description="Relative time to stop filtering by in seconds. Applies only to sample.")
        self.declareListProperty("Binning", [0.,0.,0.],
                             Description="Positive is linear bins, negative is logorithmic")
        self.declareProperty("BinInDspace", True,
                             Description="If all three bin parameters a specified, whether they are in dspace (true) or time-of-flight (false)")
        self.declareProperty("VanadiumPeakWidthPercentage", 5.)
        self.declareProperty("VanadiumSmoothParams", "20,2")
        self.declareProperty("FilterBadPulses", True, Description="Filter out events measured while proton charge is more than 5% below average")
        outfiletypes = ['gsas', 'fullprof', 'gsas and fullprof']
        self.declareProperty("FilterByLogValue", "", Description="Name of log value to filter by")
        self.declareProperty("FilterMinimumValue", 0.0, Description="Minimum log value for which to keep events.")
        self.declareProperty("FilterMaximumValue", 0.0, Description="Maximum log value for which to keep events.")
        self.declareProperty("SaveAs", "gsas", ListValidator(outfiletypes))
        self.declareFileProperty("OutputDirectory", "", FileAction.Directory)

    def _findData(self, runnumber, extension):
        #self.log().information(str(dir()))
        #self.log().information(str(dir(mantidsimple)))
        result = FindSNSNeXus(Instrument=self._instrument,
                              RunNumber=runnumber, Extension=extension)
#        result = self.executeSubAlg("FindSNSNeXus", Instrument=self._instrument,
#                                    RunNumber=runnumber, Extension=extension)
        return result["ResultPath"].value

    def _loadPreNeXusData(self, runnumber, extension):
        # generate the workspace name
        name = "%s_%d" % (self._instrument, runnumber)
        filename = name + extension
        print filename

        try: # first just try loading the file
            alg = LoadEventPreNexus(EventFilename=filename, OutputWorkspace=name)
            wksp = alg['OutputWorkspace']
        except:
            pass

        if 'wksp' in locals():
            pass
        else:
            # find the file to load
            filename = self._findData(runnumber, extension)
            # load the prenexus file
            alg = LoadEventPreNexus(EventFilename=filename, OutputWorkspace=name)
            wksp = alg['OutputWorkspace']

        # add the logs to it
        nxsfile = "%s_%d_event.nxs" % (self._instrument, runnumber)
        try:
            LoadNexusLogs(Workspace=wksp, Filename=nxsfile)
            if str(self._instrument) == "SNAP":
                LoadInstrument(Workspace=wksp, InstrumentName=self._instrument, RewriteSpectraMap=False)
            return wksp
        except:
            pass

        nxsfile = "%s_%d_histo.nxs" % (self._instrument, runnumber)
        try:
            LoadNexusLogs(Workspace=wksp, Filename=nxsfile)
            if str(self._instrument) == "SNAP":
                LoadInstrument(Workspace=wksp, InstrumentName=self._instrument, RewriteSpectraMap=False)
            return wksp
        except:
            pass

        nxsfile = self._findData(runnumber, "_event.nxs")
        try:
            LoadNexusLogs(Workspace=wksp, Filename=nxsfile)
            if str(self._instrument) == "SNAP":
                LoadInstrument(Workspace=wksp, InstrumentName=self._instrument, RewriteSpectraMap=False)
            return wksp
        except:
            pass

        nxsfile = self._findData(runnumber, "_histo.nxs")
        try:
            LoadNexusLogs(Workspace=wksp, Filename=nxsfile)
            if str(self._instrument) == "SNAP":
                LoadInstrument(Workspace=wksp, InstrumentName=self._instrument, RewriteSpectraMap=False)
            return wksp
        except:
            pass

        # TODO filter out events using timemin and timemax

        return wksp

    def _loadEventNeXusData(self, runnumber, extension, **kwargs):
        if self.getProperty("CompressOnRead"):
            kwargs["CompressTolerance"] = COMPRESS_TOL_TOF
        else:
            kwargs["Precount"] = True
        name = "%s_%d" % (self._instrument, runnumber)
        filename = name + extension

        try: # first just try loading the file
            alg = LoadEventNexus(Filename=filename, OutputWorkspace=name, **kwargs)

            return alg.workspace()
        except:
            pass

        # find the file to load
        filename = self._findData(runnumber, extension)

        # TODO use timemin and timemax to filter what events are being read
        alg = LoadEventNexus(Filename=filename, OutputWorkspace=name, **kwargs)

        return alg.workspace()

    def _loadHistoNeXusData(self, runnumber, extension):
        name = "%s_%d" % (self._instrument, runnumber)
        filename = name + extension

        try: # first just try loading the file
            alg = LoadTOFRawNexus(Filename=filename, OutputWorkspace=name)

            return alg.workspace()
        except:
            pass

        # find the file to load
        filename = self._findData(runnumber, extension)

        alg = LoadTOFRawNexus(Filename=filename, OutputWorkspace=name)

        return alg.workspace()

    def _loadData(self, runnumber, extension, filterWall=None):
        filter = {}
        if filterWall is not None:
            if filterWall[0] > 0.:
                filter["FilterByTime_Start"] = filterWall[0]
            if filterWall[1] > 0.:
                filter["FilterByTime_Stop"] = filterWall[1]

        if  runnumber is None or runnumber <= 0:
            return None

        if extension.endswith("_event.nxs"):
            return self._loadEventNeXusData(runnumber, extension, **filter)
        elif extension.endswith("_histo.nxs"):
            return self._loadHistoNeXusData(runnumber, extension)
        elif "and" in extension:
            wksp0 = self._loadPreNeXusData(runnumber, "_neutron0_event.dat")
            wksp1 = self._loadPreNeXusData(runnumber, "_neutron1_event.dat")
            return wksp0 + wksp1;
        else:
            return self._loadPreNeXusData(runnumber, extension)

    def _focus(self, wksp, calib, info, filterLogs=None, preserveEvents=True):
        if wksp is None:
            return None

        # load the calibration file if the workspaces aren't already in memory
        if (mtd[self._instrument + "_offsets"] is None) or (mtd[self._instrument + "_mask"] is None) \
            or (mtd[self._instrument + "_group"] is None):
            LoadCalFile(InputWorkspace=wksp, CalFileName=calib, WorkspaceName=self._instrument)

        if not "histo" in self.getProperty("Extension"):
            # take care of filtering events
            if self._filterBadPulses and not self.getProperty("CompressOnRead"):
                FilterBadPulses(InputWorkspace=wksp, OutputWorkspace=wksp)
            if self._removePromptPulseWidth > 0.:
                RemovePromptPulse(InputWorkspace=wksp, OutputWorkspace=wksp, Width= self._removePromptPulseWidth)
            if filterLogs is not None:
                try:
                    logparam = wksp.getRun()[filterLogs[0]]
                    if logparam is not None:
                        FilterByLogValue(InputWorkspace=wksp, OutputWorkspace=wksp, LogName=filterLogs[0],
                                         MinimumValue=filterLogs[1], MaximumValue=filterLogs[2])
                except KeyError, e:
                    raise RuntimeError("Failed to find log '%s' in workspace '%s'" \
                                       % (filterLogs[0], str(wksp)))            
            if not self.getProperty("CompressOnRead"):
                CompressEvents(InputWorkspace=wksp, OutputWorkspace=wksp, Tolerance=COMPRESS_TOL_TOF) # 100ns
            SortEvents(wksp)
        try:
            if info.tmin > 0.:
                MaskBins(InputWorkspace=wksp, OutputWorkspace=wksp, XMin=0., XMax=info.tmin)
        except AttributeError:
            pass
        try:
            if info.tmax > 0.:
                if info.tmin >= info.tmax:
                    raise RuntimeError("Encountered tmin (%f) >= tmax (%f)" % (info.tmin, info.tmax))
                MaskBins(InputWorkspace=wksp, OutputWorkspace=wksp, XMin=info.tmax, XMax=5.*info.tmax)
        except AttributeError:
            pass
        MaskDetectors(Workspace=wksp, MaskedWorkspace=self._instrument + "_mask")
        AlignDetectors(InputWorkspace=wksp, OutputWorkspace=wksp, OffsetsWorkspace=self._instrument + "_offsets")
        LRef = self.getProperty("UnwrapRef")
        DIFCref = self.getProperty("LowResRef")
        if (LRef > 0.) or (DIFCref > 0.): # super special Jason stuff
            kwargs = {}
            #try:
            #    if info.tmin > 0:
            #        kwargs["Tmin"] = info.tmin
            #except:
            #    pass
            #try:
            #    kwargs["Tmax"] = info.tmax
            #except:
            #    pass
            ConvertUnits(InputWorkspace=wksp, OutputWorkspace=wksp, Target="TOF") # corrections only work in TOF for now
            if LRef > 0:
                UnwrapSNS(InputWorkspace=wksp, OutputWorkspace=wksp, LRef=LRef, **kwargs)
            if DIFCref > 0:
                if kwargs.has_key("Tmax"):
                    del kwargs["Tmax"]
                RemoveLowResTOF(InputWorkspace=wksp, OutputWorkspace=wksp, ReferenceDIFC=DIFCref,
                                K=3.22, **kwargs)
            ConvertUnits(InputWorkspace=wksp, OutputWorkspace=wksp, Target="dSpacing") # put it back to the original units
        if len(self._binning) == 3:
            info.has_dspace = self._bin_in_dspace
        if info.has_dspace:
            if len(self._binning) == 3:
                binning = self._binning
            else:
                binning = [info.dmin, self._binning[0], info.dmax]
            self.log().information("d-Spacing Binning: " + str(binning))
            Rebin(InputWorkspace=wksp, OutputWorkspace=wksp, Params=binning)
        else:
            preserveEvents = True
        DiffractionFocussing(InputWorkspace=wksp, OutputWorkspace=wksp, GroupingWorkspace=self._instrument + "_group", PreserveEvents=preserveEvents)
        ConvertUnits(InputWorkspace=wksp, OutputWorkspace=wksp, Target="TOF")
        if preserveEvents and not "histo" in self.getProperty("Extension"):
            CompressEvents(InputWorkspace=wksp, OutputWorkspace=wksp, Tolerance=COMPRESS_TOL_TOF) # 100ns
        if not info.has_dspace:
            if len(self._binning) == 3:
                binning = self._binning
            else:
                binning = [info.tmin, self._binning[0], info.tmax]
            Rebin(InputWorkspace=wksp, OutputWorkspace=wksp, Params=binning)
        NormaliseByCurrent(InputWorkspace=wksp, OutputWorkspace=wksp)

        return wksp

    def _getinfo(self, wksp):
        logs = wksp.getRun()
        # get the frequency
        if not "SpeedRequest1" in logs.keys():
            self.log().information("SpeedRequest1 is not specified")
            return self._config.getInfo(None, None)
        frequency = logs['SpeedRequest1']
        if frequency.units != "Hz":
            raise RuntimeError("Only know how to deal with frequency in Hz, not %s" % frequency.units)
        frequency = frequency.getStatistics().mean

        if not "LambdaRequest" in logs.keys():
            self.log().information("LambdaRequest is not in the datafile")
            return self._config.getInfo(None, None)
        wavelength = logs['LambdaRequest']
        if wavelength.units != "Angstrom":
            raise RuntimeError("Only know how to deal with LambdaRequest in Angstrom, not $s" % wavelength)
        wavelength = wavelength.getStatistics().mean

        self.log().information("frequency: " + str(frequency) + "Hz center wavelength:" + str(wavelength) + "Angstrom")
        return self._config.getInfo(frequency, wavelength)        

    def _save(self, wksp, info, normalized):
        filename = os.path.join(self._outDir, str(wksp))
        if "gsas" in self._outTypes:
            SaveGSS(InputWorkspace=wksp, Filename=filename+".gsa", SplitFiles="False", Append=False, MultiplyByBinWidth=normalized, Bank=info.bank, Format="SLOG")
        if "fullprof" in self._outTypes:
            SaveFocusedXYE(InputWorkspace=wksp, Filename=filename+".dat")

    def PyExec(self):
        # temporary hack for getting python algorithms working
        import mantidsimple
        globals()["FindSNSNeXus"] = mantidsimple.FindSNSNeXus

        # get generic information
        SUFFIX = self.getProperty("Extension")
        self._config = self.PDConfigFile(self.getProperty("CharacterizationRunsFile"))
        self._binning = self.getProperty("Binning")
        if len(self._binning) != 1 and len(self._binning) != 3:
            raise RuntimeError("Can only specify (width) or (start,width,stop) for binning. Found %d values." % len(self._binning))
        if len(self._binning) == 3:
            if self._binning[0] == 0. and self._binning[1] == 0. and self._binning[2] == 0.:
                raise RuntimeError("Failed to specify the binning")
        self._bin_in_dspace = self.getProperty("BinInDspace")
        self._instrument = self.getProperty("Instrument")
        mtd.settings['default.facility'] = "SNS"
        mtd.settings['default.instrument'] = self._instrument
#        self._timeMin = self.getProperty("FilterByTimeMin")
#        self._timeMax = self.getProperty("FilterByTimeMax")
        self._filterBadPulses = self.getProperty("FilterBadPulses")
        self._removePromptPulseWidth = self.getProperty("RemovePromptPulseWidth")
        filterLogs = self.getProperty("FilterByLogValue")
        if len(filterLogs.strip()) <= 0:
            filterLogs = None
        else:
            filterLogs = [filterLogs, 
                          self.getProperty("FilterMinimumValue"), self.getProperty("FilterMaximumValue")]
        self._vanPeakWidthPercent = self.getProperty("VanadiumPeakWidthPercentage")
        self._vanSmoothing = self.getProperty("VanadiumSmoothParams")
        calib = self.getProperty("CalibrationFile")
        self._outDir = self.getProperty("OutputDirectory")
        self._outTypes = self.getProperty("SaveAs")
        samRuns = self.getProperty("RunNumber")
        filterWall = (self.getProperty("FilterByTimeMin"), self.getProperty("FilterByTimeMax"))

        workspacelist = [] # all data workspaces that will be converted to d-spacing in the end

        if self.getProperty("Sum"):
            samRun = None
            info = None
            for temp in samRuns:
                temp = self._loadData(temp, SUFFIX, filterWall)
                tempinfo = self._getinfo(temp)
                temp = self._focus(temp, calib, tempinfo, filterLogs)
                if samRun is None:
                    samRun = temp
                    info = tempinfo
                else:
                    if abs(tempinfo.freq - info.freq)/info.freq > .05:
                        raise RuntimeError("Cannot add incompatible frequencies (%f!=%f)" \
                                           % (tempinfo.freq, info.freq))
                    if abs(tempinfo.wl - info.wl)/info.freq > .05:
                        raise RuntimeError("Cannot add incompatible wavelengths (%f != %f)" \
                                           % (tempinfo.wl, info.wl))
                    Plus(samRun, temp, samRun)
                    mtd.deleteWorkspace(str(temp))
            samRuns = [samRun]
            workspacelist.append(str(samRun))

        for samRun in samRuns:
            # first round of processing the sample
            if not self.getProperty("Sum"):
                samRun = self._loadData(samRun, SUFFIX, filterWall)
                info = self._getinfo(samRun)
                samRun = self._focus(samRun, calib, info, filterLogs)
                workspacelist.append(str(samRun))

            # process the container
            canRun = self.getProperty("BackgroundNumber")
            if canRun <= 0:
                canRun = info.can
            if canRun > 0:
                temp = mtd["%s_%d" % (self._instrument, canRun)]
                if temp is None:
                    canRun = self._loadData(canRun, SUFFIX, (0., 0.))
                    canRun = self._focus(canRun, calib, info)
                else:
                    canRun = temp
                ConvertUnits(InputWorkspace=canRun, OutputWorkspace=canRun, Target="TOF")
                workspacelist.append(str(canRun))
            else:
                canRun = None

            # process the vanadium run
            vanRun = self.getProperty("VanadiumNumber")
            if vanRun <= 0:
                vanRun = info.van
            if vanRun > 0:
                temp = mtd["%s_%d" % (self._instrument, vanRun)]
                if temp is None:
                    vanRun = self._loadData(vanRun, SUFFIX, (0., 0.))
                    vanRun = self._focus(vanRun, calib, info, preserveEvents=False)
                    ConvertUnits(InputWorkspace=vanRun, OutputWorkspace=vanRun, Target="dSpacing")
                    StripVanadiumPeaks(InputWorkspace=vanRun, OutputWorkspace=vanRun, PeakWidthPercent=self._vanPeakWidthPercent)
                    ConvertUnits(InputWorkspace=vanRun, OutputWorkspace=vanRun, Target="TOF")
                    FFTSmooth(InputWorkspace=vanRun, OutputWorkspace=vanRun, Filter="Butterworth",
                              Params=self._vanSmoothing,IgnoreXBins=True,AllSpectra=True)
                    MultipleScatteringCylinderAbsorption(InputWorkspace=vanRun, OutputWorkspace=vanRun, # numbers for vanadium
                                                         AttenuationXSection=2.8, ScatteringXSection=5.1,
                                                         SampleNumberDensity=0.0721, CylinderSampleRadius=.3175)
                    SetUncertaintiesToZero(InputWorkspace=vanRun, OutputWorkspace=vanRun)
                else:
                    vanRun = temp
                ConvertUnits(InputWorkspace=vanRun, OutputWorkspace=vanRun, Target="TOF")
                workspacelist.append(str(vanRun))
            else:
                vanRun = None

            # the final bit of math
            if canRun is not None:
                samRun -= canRun
                if not "histo" in self.getProperty("Extension"):
                    CompressEvents(InputWorkspace=samRun, OutputWorkspace=samRun,
                               Tolerance=COMPRESS_TOL_TOF) # 10ns
                canRun = str(canRun)
            if vanRun is not None:
                samRun /= vanRun
                normalized = True
                samRun.getRun()['van_number'] = vanRun.getRun()['run_number'].value
                vanRun = str(vanRun)
            else:
                normalized = False

            # write out the files
            if not "histo" in self.getProperty("Extension"):
                CompressEvents(InputWorkspace=samRun, OutputWorkspace=samRun,
                           Tolerance=COMPRESS_TOL_TOF) # 5ns
            self._save(samRun, info, normalized)
            samRun = str(samRun)
            mtd.releaseFreeMemory()

        # convert everything into d-spacing
        workspacelist = set(workspacelist) # only do each workspace once
        for wksp in workspacelist:
            ConvertUnits(InputWorkspace=wksp, OutputWorkspace=wksp, Target="dSpacing")

mtd.registerPyAlgorithm(SNSPowderReduction())
