#include "MantidAlgorithms/WienerSmooth.h"
#include "MantidAPI/IFunction.h"
#include "MantidAPI/FunctionFactory.h"

#include <numeric>

namespace Mantid
{
namespace Algorithms
{

  using Mantid::Kernel::Direction;
  using Mantid::API::WorkspaceProperty;

  // Register the algorithm into the AlgorithmFactory
  DECLARE_ALGORITHM(WienerSmooth)
  
  namespace
  {
    // Square values
    struct PowerSpectrum
    {
      double operator()(double x) const {return x * x;}
    };
  }

  //----------------------------------------------------------------------------------------------
  /** Constructor
   */
  WienerSmooth::WienerSmooth()
  {
  }
    
  //----------------------------------------------------------------------------------------------
  /** Destructor
   */
  WienerSmooth::~WienerSmooth()
  {
  }
  

  //----------------------------------------------------------------------------------------------

  
  /// Algorithm's version for identification. @see Algorithm::version
  int WienerSmooth::version() const { return 1;};
  
  /// Algorithm's category for identification. @see Algorithm::category
  const std::string WienerSmooth::category() const { return "Arithmetic\\FFT;Transforms\\Smoothing";}

  /// Algorithm's summary for use in the GUI and help. @see Algorithm::summary
  const std::string WienerSmooth::summary() const { return "Smooth spectrum using Wiener filter.";};

  //----------------------------------------------------------------------------------------------
  /** Initialize the algorithm's properties.
   */
  void WienerSmooth::init()
  {
    declareProperty(new WorkspaceProperty<>("InputWorkspace","",Direction::Input), "An input workspace.");
    declareProperty("WorkspaceIndex",0,"A workspace index for the histogram to smooth.");
    declareProperty(new WorkspaceProperty<>("OutputWorkspace","",Direction::Output), "An output workspace.");
    //declareProperty(new WorkspaceProperty<>("OutputFilter","",Direction::Output), "An output workspace with the Wiener filter.");
  }

  //----------------------------------------------------------------------------------------------
  /** Execute the algorithm.
   */
  void WienerSmooth::exec()
  {
    // Get the data to smooth
    API::MatrixWorkspace_sptr inputWS = getProperty("InputWorkspace");
    size_t wsIndex = static_cast<int>( getProperty("WorkspaceIndex") );

    auto &X = inputWS->readX(wsIndex);
    auto &Y = inputWS->readY(wsIndex);

    // Digital fourier transform works best for data oscillating around 0.
    // Fit a spline with a small number of break points to the data. 
    // Make sure that the spline passes through the first and the last points
    // of the data.
    // The fitted spline will be subtracted from the data and the difference
    // will be smoothed with the Wiener filter. After that the spline will be
    // added to the smoothed data to produce the output.

    // number of spline break points, must be smaller than the data size but between 2 and 10
    size_t nbreak = 10;
    size_t ndata = Y.size();
    if ( nbreak * 3 > ndata ) nbreak = ndata / 3;
    if ( nbreak < 2 ) nbreak = 2;

    // define the spline
    API::IFunction_sptr spline = API::FunctionFactory::Instance().createFunction("BSpline");
    auto xInterval = getStartEnd( X, inputWS->isHistogramData() );
    spline->setAttributeValue( "StartX", xInterval.first );
    spline->setAttributeValue( "EndX", xInterval.second );
    spline->setAttributeValue( "NBreak", static_cast<int>(nbreak) );
    // fix the first and last parameters to the first and last data values
    spline->setParameter(0, Y.front() );
    spline->fix(0);
    size_t lastParamIndex = spline->nParams() - 1;
    spline->setParameter(lastParamIndex, Y.back() );
    spline->fix(lastParamIndex);

    // fit the spline to the data
    auto fit = createChildAlgorithm("Fit");
    fit->initialize();
    fit->setProperty( "Function", spline );
    fit->setProperty( "InputWorkspace", inputWS );
    fit->setProperty( "WorkspaceIndex", static_cast<int>(wsIndex) );
    fit->setProperty( "CreateOutput", true );
    fit->execute();

    // get the fit output workspace; spectrum 2 contains the difference that is to be smoothed
    API::MatrixWorkspace_sptr fitOut = fit->getProperty("OutputWorkspace");

    // Fourier transform the difference spectrum
    auto fourier = createChildAlgorithm( "RealFFT" );
    fourier->initialize();
    fourier->setProperty( "InputWorkspace", fitOut );
    fourier->setProperty( "WorkspaceIndex", 2 );
    // we don't require bin linearity as we don't need the exact transform
    fourier->setProperty( "IgnoreXBins", true );
    fourier->execute();

    API::MatrixWorkspace_sptr fourierOut = fourier->getProperty("OutputWorkspace");

    // spectrum 2 of the transformed workspace has the transform modulus which is a square 
    // root of the power spectrum
    auto &powerSpec = fourierOut->dataY(2);
    // convert the modulus to power spectrum wich is the base of the Wiener filter
    std::transform( powerSpec.begin(), powerSpec.end(), powerSpec.begin(), PowerSpectrum() );

    // estimate power spectrum's noise as the average of its high frequency half 
    size_t n2 = powerSpec.size();
    double noise = std::accumulate( powerSpec.begin() + n2/2, powerSpec.end(), 0.0 );
    noise /= n2;

    // storage for the Wiener filter, initialized with 0.0's
    std::vector<double> wf(n2);

    // The filter consists of two parts: 
    //   1) low frequency region, from 0 until the power spectrum falls to the noise level, filter is calculated 
    //      from the power spectrum
    //   2) high frequency noisy region, filter is a smooth function of frequency decreasing to 0
    
    // the following code is an adaptation of a fortran routine
    // noise starting index
    size_t i0 = 0; 
    // intermediate variables
    double xx = 0.0;
    double xy = 0.0;
    double ym = 0.0;
    // low frequency filter values: the higher the power spectrum the closer the filter to 1.0
    for(size_t i = 0; i < n2; ++i)
    {
      double cd1 = powerSpec[i] / noise;
      double cd2 = log(cd1);
      if ( i0 > 0 ) break;
      if ( cd1 < 1.0 && i0 == 0 )
      {
        i0 = i;
      }
      wf[i] = cd1 / (1.0 + cd1);
      double j = static_cast<double>(i+1);
      xx += j * j;
      xy += j * cd2;
      ym += cd2;
    }

    // high frequency filter values: smooth decreasing function
    double ri0f = static_cast<double>(i0 + 1);
    double xm = (1.0 + ri0f)/2;
    ym /= ri0f;
    double a1 = (xy - ri0f*xm*ym)/(xx-ri0f*xm*xm);
    double b1 = ym - a1*xm;
    const double dblev = -20.0;
    // cut-off index
    double ri1 = floor( (dblev/4-b1)/a1 );
    if ( ri1 <= 0.0 )
    {
      ri1 = static_cast<double>(n2);
    }
    size_t i1 = static_cast<size_t>(ri1);
    for(size_t i = i0; i < i1; ++i)
    {
      double s = exp(a1*static_cast<double>(i+1)+b1);
      wf[i] = s / (1.0 + s);
    }
    // wf[i] for i1 <= i < n2 are 0.0

    // multiply the fourier transform by the filter
    auto &re = fourierOut->dataY(0);
    auto &im = fourierOut->dataY(1);
    std::transform( re.begin(), re.end(), wf.begin(), re.begin(), std::multiplies<double>() );
    std::transform( im.begin(), im.end(), wf.begin(), im.begin(), std::multiplies<double>() );

    //powerSpec = wf;
    //setProperty( "OutputFilter", fourierOut );

    // inverse fourier transform
    fourier = createChildAlgorithm( "RealFFT" );
    fourier->initialize();
    fourier->setProperty( "InputWorkspace", fourierOut );
    fourier->setProperty( "IgnoreXBins", true );
    fourier->setPropertyValue( "Transform", "Backward" );
    fourier->execute();

    API::MatrixWorkspace_sptr out = fourier->getProperty("OutputWorkspace");
    auto &background = fitOut->readY(1);
    auto &y = out->dataY(0);

    // add the spline "background" to the smoothed data
    std::transform( y.begin(), y.end(), background.begin(), y.begin(), std::plus<double>() );
    // copy the x-values and errors from the original spectrum
    out->setX(0, X );
    auto &E = inputWS->readE(wsIndex);
    out->dataE(0).assign(E.begin(),E.end());

    // set the output
    setProperty( "OutputWorkspace", out );
  }

  /**
   * Get the start and end of the x-interval.
   * @param X :: The x-vector of a spectrum.
   * @param isHistogram :: Is the x-vector comming form a histogram? If it's true the bin
   *   centres are used.
   * @return :: A pair of start x and end x.
   */
  std::pair<double,double> WienerSmooth::getStartEnd( const MantidVec& X, bool isHistogram ) const
  {
    const size_t n = X.size();
    if ( n < 3 )
    {
      // 3 is the smallest number for this method to work without breaking
      throw std::runtime_error("Number of bins/data points cannot be smaller than 3.");
    }
    if ( isHistogram )
    {
      return std::make_pair( (X[0] + X[1])/2, (X[n-1] + X[n-2])/2 );
    }
    // else
    return std::make_pair( X.front(), X.back() );
  }

} // namespace Algorithms
} // namespace Mantid