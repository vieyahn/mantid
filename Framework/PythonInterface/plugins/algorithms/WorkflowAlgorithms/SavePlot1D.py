#pylint: disable=no-init,invalid-name
import mantid,sys

class SavePlot1D(mantid.api.PythonAlgorithm):

    _wksp = None

    def category(self):
        """ Category
        """
        return "DataHandling\\Plots"

    def name(self):
        """ Algorithm name
        """
        return "SavePlot1D"

    def summary(self):
        return "Save 1D plots to a file"

    def checkGroups(self):
        return False

    def PyInit(self):
        #declare properties
        self.declareProperty(mantid.api.WorkspaceProperty("InputWorkspace","",mantid.kernel.Direction.Input),"Workspace to plot")
        self.declareProperty(mantid.api.FileProperty('OutputFilename', '', action=mantid.api.FileAction.Save, extensions = ["png"]),
                             doc='Name of the image file to savefile.')
        self.declareProperty("XLabel","","Label on the X axis. If empty, it will be taken from workspace")
        self.declareProperty("YLabel","","Label on the Y axis. If empty, it will be taken from workspace")


    def PyExec(self):
        ok2run=''
        try:
            import matplotlib
            from distutils.version import LooseVersion
            if LooseVersion(matplotlib.__version__)<LooseVersion("1.2.0"):
                ok2run='Wrong version of matplotlib. Required >= 1.2.0'
        except ImportError:
            ok2run='Problem importing matplotlib'
        if ok2run!='':
            raise RuntimeError(ok2run)

        #plotly import and version check
        #import plotly.offline as pltly

        matplotlib=sys.modules['matplotlib']
        matplotlib.use("agg")
        import matplotlib.pyplot as plt
        self._wksp = self.getProperty("InputWorkspace").value
        if type(self._wksp)==mantid.api.WorkspaceGroup:
            num_subplots=self._wksp.getNumberOfEntries()
            fig, axarr = plt.subplots(num_subplots)
            for i in range(self._wksp.getNumberOfEntries()):
                self.DoPlot(axarr[i],self._wksp.getItem(i))
        else:
            fig, ax = plt.subplots()
            self.DoPlot(ax,self._wksp)

        #the following does not work with plotly
        plt.tight_layout(1.08)
        filename = self.getProperty("OutputFilename").value
        #plotly or png
        #pltly.plot_mpl(fig, filename=filename)
        fig.savefig(filename,bbox_inches='tight')

    def DoPlot(self,ax,ws):
        spectra=ws.getNumberHistograms()
        if spectra>10:
            mantid.kernel.logger.warning("more than 10 spectra to plot")
        prog_reporter=mantid.api.Progress(self,start=0.0,end=1.0,\
                    nreports=spectra)

        for j in range(spectra):
            x=ws.readX(j)
            y=ws.readY(j)
            if x.size==y.size+1:
                x=(x[:-1]+x[1:])*0.5
            #get labels for the curves
            a=ws.getAxis(1)
            if a.isSpectra():
                plotlabel=a.label(j)
            else:
                LHS=a.title()
                if LHS=="":
                    LHS=a.getUnit().caption()
                plotlabel=LHS+" = "+str(float(a.label(j)))
            ax.plot(x,y,label=plotlabel)
            xlabel=self.getProperty("XLabel").value
            ylabel=self.getProperty("YLabel").value
            if xlabel=="":
                xaxis=ws.getAxis(0)
                unitLabel=xaxis.getUnit().symbol().latex()
                xlabel=xaxis.getUnit().caption()
                if len(unitLabel)>0:
                    xlabel+=" ($"+unitLabel+"$)"
            if ylabel=="":
                ylabel=ws.YUnit()
                if ylabel=='':
                    ylabel = ws.YUnitLabel()

            # $...$ does not work with plotly
            ax.set_xlabel(xlabel)
            ax.set_ylabel(ylabel)
            prog_reporter.report("Processing")
        #legend does not work with plotly
        if spectra>1 and spectra<=10:
            ax.legend()



mantid.api.AlgorithmFactory.subscribe(SavePlot1D)
