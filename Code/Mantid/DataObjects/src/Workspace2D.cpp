#include "MantidDataObjects/Workspace2D.h"
#include "MantidKernel/Exception.h"
#include "MantidAPI/RefAxis.h"
#include "MantidAPI/LocatedDataRef.h"
#include "MantidAPI/WorkspaceIterator.h"
#include "MantidAPI/WorkspaceIteratorCode.h"
#include "MantidAPI/WorkspaceProperty.h"
#include "MantidAPI/WorkspaceFactory.h"

DECLARE_WORKSPACE(Workspace2D)

namespace Mantid
{
  namespace DataObjects
  {
  
    // Get a reference to the logger
    Kernel::Logger& Workspace2D::g_log = Kernel::Logger::get("Workspace2D");

    /// Constructor
    Workspace2D::Workspace2D()
    {}

    ///Destructor
    Workspace2D::~Workspace2D()
    {}

    /** Sets the size of the workspace and initializes arrays to zero
    *  @param NVectors The number of vectors/histograms/detectors in the workspace
    *  @param XLength The number of X data points/bin boundaries in each vector (must all be the same)
    *  @param YLength The number of data/error points in each vector (must all be the same)
    */
    void Workspace2D::init(const int &NVectors, const int &XLength, const int &YLength)
    {
      if (NVectors <= 0 || XLength <= 0 || YLength <= 0)
      {
        g_log.error("All arguments to init must be positive and non-zero");
        throw std::out_of_range("All arguments to init must be positive and non-zero");
      }

      m_noVectors = NVectors;
      data.resize(m_noVectors);
      m_axes.resize(2);
      m_axes[0] = new API::RefAxis(XLength, this);
      // This axis is always a spectra one for now
      m_axes[1] = new API::Axis(API::AxisType::Spectra,m_noVectors);

      Histogram1D::RCtype t1,t2;
      t1.access().resize(XLength); //this call initializes array to zero 
      t2.access().resize(YLength);
      for (int i=0;i<m_noVectors;i++)
      {
        this->setX(i,t1);
        // Y,E,E2 arrays populated
        this->setData(i,t2,t2,t2);
      }
    }

    /**
    Get the amount of memory used by the 2D workspace
    \todo to be changed
    */
    long int 
      Workspace2D::getMemorySize() const 
    {
      return 0;
    }

    /**
    Set the x values
    \param histnumber :: Index to the histogram
    \param Vec :: Vec to set [Should be typedef]
    */
    void 
      Workspace2D::setX(const int histnumber, const std::vector<double>& Vec)
    {
      if (histnumber<0 || histnumber>=m_noVectors)
        throw std::range_error("Workspace2D::setX, histogram number out of range");

      data[histnumber].dataX()=Vec;
      return;
    }

    /**
    Set the x values
    \param histnumber :: Index to the histogram
    \param Vec :: Shared ptr base object
    */
    void 
      Workspace2D::setX(const int histnumber, const Histogram1D::RCtype::ptr_type& Vec)
    {
      if (histnumber<0 || histnumber>=m_noVectors)
        throw std::range_error("Workspace2D::setX, histogram number out of range");

      data[histnumber].setX(Vec);
      return;
    }

    /**
    Set the x values
    \param histnumber :: Index to the histogram
    \param PA :: Reference counted histogram
    */
    void 
      Workspace2D::setX(const int histnumber, const Histogram1D::RCtype& PA)
    {
      if (histnumber<0 || histnumber>=m_noVectors)
        throw std::range_error("Workspace2D::setX, histogram number out of range");

      data[histnumber].setX(PA);
      return;
    }

    /**
    Sets the data in the workspace
    \param histnumber The histogram to be set
    \param Vec A vector containing the data	
    */
    void 
      Workspace2D::setData(const int histnumber, const std::vector<double>& Vec)
    {
      if (histnumber<0 || histnumber>=m_noVectors)
        throw std::range_error("Workspace2D::setDAta, histogram number out of range");

      data[histnumber].dataY()=Vec;
    }

    /**
    Sets the data in the workspace (including errors)
    \param histnumber The histogram to be set
    \param Vec A vector containing the data	
    \param VecErr A vector containing the corresponding errors
    */
    void 
      Workspace2D::setData(const int histnumber, const std::vector<double>& Vec, 
      const std::vector<double>& VecErr)
    {
      if (histnumber<0 || histnumber>=m_noVectors)
        throw std::range_error("Workspace2D::setData, histogram number out of range");

      data[histnumber].dataY()=Vec;
      data[histnumber].dataE()=VecErr;
      return;
    }

    /**
    Sets the data in the workspace (including errors)
    \param histnumber The histogram to be set
    \param Vec A vector containing the data	
    \param VecErr A vector containing the corresponding errors
    \param VecErr2 A vector containing the corresponding second error value
    */
    void 
      Workspace2D::setData(const int histnumber, const std::vector<double>& Vec, 
      const std::vector<double>& VecErr, const std::vector<double>& VecErr2)
    {
      if (histnumber<0 || histnumber>=m_noVectors)
        throw std::range_error("Workspace2D::setData, histogram number out of range");

      data[histnumber].dataY()=Vec;
      data[histnumber].dataE()=VecErr;
      data[histnumber].dataE2()=VecErr2;
      return;
    }

    /**
    Sets the data in the workspace
    \param histnumber The histogram to be set
    \param PY A reference counted data range	
    */
    void 
      Workspace2D::setData(const int histnumber, const Histogram1D::RCtype& PY)
    {
      if (histnumber<0 || histnumber>=m_noVectors)
        throw std::range_error("Workspace2D::setData, histogram number out of range");

      data[histnumber].setData(PY);
    }

    /**
    Sets the data in the workspace
    \param histnumber The histogram to be set
    \param PY A reference counted data range	
    \param PE A reference containing the corresponding errors
    */
    void Workspace2D::setData(const int histnumber, const Histogram1D::RCtype& PY, 
      const Histogram1D::RCtype& PE)
    {
      if (histnumber<0 || histnumber>=m_noVectors)
        throw std::range_error("Workspace2D::setData, histogram number out of range");

      data[histnumber].setData(PY,PE);
      return;
    }

    /**
    Sets the data in the workspace
    \param histnumber The histogram to be set
    \param PY A reference counted data range	
    \param PE A reference containing the corresponding errors
    \param PE2 A reference containing the corresponding error second value
    */
    void Workspace2D::setData(const int histnumber, const Histogram1D::RCtype& PY, 
      const Histogram1D::RCtype& PE,const Histogram1D::RCtype& PE2)
    {
      if (histnumber<0 || histnumber>=m_noVectors)
        throw std::range_error("Workspace2D::setData, histogram number out of range");

      data[histnumber].setData(PY,PE,PE2);
      return;
    }

    /**
    Sets the data in the workspace
    \param histnumber The histogram to be set
    \param PY A reference counted data range	
    \param PE A reference containing the corresponding errors
    */
    void Workspace2D::setData(const int histnumber, const Histogram1D::RCtype::ptr_type& PY, 
      const Histogram1D::RCtype::ptr_type& PE)
    {
      if (histnumber<0 || histnumber>=m_noVectors)
        throw std::range_error("Workspace2D::setData, histogram number out of range");

      data[histnumber].setData(PY,PE);
      return;
    }

    /**
    Sets the data in the workspace
    \param histnumber The histogram to be set
    \param PY A reference counted data range	
    \param PE A reference containing the corresponding errors
    \param PE2 A reference containing the corresponding error second value	
    */
    void Workspace2D::setData(const int histnumber, const Histogram1D::RCtype::ptr_type& PY, 
      const Histogram1D::RCtype::ptr_type& PE, const Histogram1D::RCtype::ptr_type& PE2)
    {
      if (histnumber<0 || histnumber>=m_noVectors)
        throw std::range_error("Workspace2D::setData, histogram number out of range");

      data[histnumber].setData(PY,PE,PE2);
      return;
    }

    /** Gets the number of histograms
    \return Integer
    */
    const int 
      Workspace2D::getHistogramNumber() const
    {
      return getHistogramNumberHelper();
    }

    /**
    Get the x data of a specified histogram
    @param index The number of the histogram
    @return A vector of doubles containing the x data
    */
    const std::vector<double>& 
      Workspace2D::dataX(const int index) const
    {
      if (index<0 || index>=m_noVectors)
        throw std::range_error("Workspace2D::dataX, histogram number out of range");

      return data[index].dataX();
    }

    /**
    Get the y data of a specified histogram
    @param index The number of the histogram
    @return A vector of doubles containing the y data
    */
    const std::vector<double>& 
      Workspace2D::dataY(const int index) const
    {
      if (index<0 || index>=m_noVectors)
        throw std::range_error("Workspace2D::dataY, histogram number out of range");

      return data[index].dataY();
    }

    /**
    Get the error data for a specified histogram
    @param index The number of the histogram
    @return A vector of doubles containing the error data
    */
    const std::vector<double>& 
      Workspace2D::dataE(const int index) const
    {
      if (index<0 || index>=m_noVectors)
        throw std::range_error("Workspace2D::dataE, histogram number out of range");

      return data[index].dataE();
    }

    /**
    Get the error data for a specified histogram
    @param index The number of the histogram
    @return A vector of doubles containing the error data
    */
    const std::vector<double>& 
      Workspace2D::dataE2(const int index) const
    {
      if (index<0 || index>=m_noVectors)
        throw std::range_error("Workspace2D::dataE2, histogram number out of range");

      return data[index].dataE2();
    }

    /// get pseudo size
    int Workspace2D::size() const 
    { 
      return data.size() * blocksize(); 
    } 

    ///get the size of each vector
    int Workspace2D::blocksize() const
    {
      return (data.size() > 0) ? data[0].size() : 0;
    }

    ///Returns the x data
    std::vector<double>& Workspace2D::dataX(int const index)
    {
      if (index<0 || index>=m_noVectors)
        throw std::range_error("Workspace2D::dataX, histogram number out of range");

      return data[index].dataX();
    }
    ///Returns the y data
    std::vector<double>& Workspace2D::dataY(int const index)
    {
      if (index<0 || index>=m_noVectors)
        throw std::range_error("Workspace2D::dataY, histogram number out of range");

      return data[index].dataY();
    }
    ///Returns the error data
    std::vector<double>& Workspace2D::dataE(int const index)
    {
      if (index<0 || index>=m_noVectors)
        throw std::range_error("Workspace2D::dataE, histogram number out of range");

      return data[index].dataE();
    }
    ///Returns the error data
    std::vector<double>& Workspace2D::dataE2(int const index)
    {
      if (index<0 || index>=m_noVectors)
        throw std::range_error("Workspace2D::dataE2, histogram number out of range");

      return data[index].dataE2();
    }

    /** Returns the number of histograms.
    For some reason Visual Studio couldn't deal with the main getHistogramNumber() method
    being virtual so it now just calls this private (and virtual) method which does the work.
    */
    const int Workspace2D::getHistogramNumberHelper() const
    {
      return static_cast<const int>(data.size());
    }

    ///Returns the ErrorHelper applicable for this spectra
    const API::IErrorHelper* Workspace2D::errorHelper(int const index) const
    {
      if (index<0 || index>=m_noVectors)
        throw std::range_error("Workspace2D::errorHelper, histogram number out of range");

      return data[index].errorHelper();
    }

    ///Sets the ErrorHelper for this spectra
    void Workspace2D::setErrorHelper(int const index,API::IErrorHelper* errorHelper)
    {
      if (index<0 || index>=m_noVectors)
        throw std::range_error("Workspace2D::setErrorHelper, histogram number out of range");

      data[index].setErrorHelper(errorHelper);
    }

    ///Sets the ErrorHelper for this spectra
    void Workspace2D::setErrorHelper(int const index,const API::IErrorHelper* errorHelper)
    {
      if (index<0 || index>=m_noVectors)
        throw std::range_error("Workspace2D::setErrorHelper, histogram number out of range");

      data[index].setErrorHelper(errorHelper);
    }
    
    
     /**
    Get the x data of a specified histogram thru python
    \param index The number of the histogram
    \return A vector of doubles containing the x data
    */
    const std::vector<double>& 
      Workspace2D::getX(const int index) const
    {
      return Workspace2D::dataX(index);
    }

    /**
    Get the y data of a specified histogram thru python
    \param index The number of the histogram
    \return A vector of doubles containing the y data
    */
    const std::vector<double>&
      Workspace2D::getY(const int index) const
    {
      return Workspace2D::dataY(index);
    }

    /**
    Get the error data for a specified histogram thru python
    \param index The number of the histogram
    \return A vector of doubles containing the error data
    */
    const std::vector<double>& 
      Workspace2D::getE(const int index) const
    {
      return Workspace2D::dataE(index);
    }

    /**
    Get the error data for a specified histogram thru python
    \param index The number of the histogram
    \return A vector of doubles containing the error data
    */
    const std::vector<double>& 
      Workspace2D::getE2(const int index) const
    {
      return Workspace2D::dataE2(index);
    }

  } // namespace DataObjects
} //NamespaceMantid






///\cond TEMPLATE
template DLLExport class Mantid::API::workspace_iterator<Mantid::API::LocatedDataRef, Mantid::DataObjects::Workspace2D>;
template DLLExport class Mantid::API::workspace_iterator<const Mantid::API::LocatedDataRef, const Mantid::DataObjects::Workspace2D>;

template DLLExport class Mantid::API::WorkspaceProperty<Mantid::DataObjects::Workspace2D>;

namespace Mantid
{
  namespace Kernel
  {
    template<> DLLExport
      Mantid::DataObjects::Workspace2D_sptr PropertyManager::getValue<Mantid::DataObjects::Workspace2D_sptr>(const std::string &name) const
    {
      PropertyWithValue<Mantid::DataObjects::Workspace2D_sptr>* prop = 
        dynamic_cast<PropertyWithValue<Mantid::DataObjects::Workspace2D_sptr>*>(getPointerToProperty(name));
      if (prop)
      {
        return *prop;
      }
      else
      {
        throw std::runtime_error("Attempt to assign property of incorrect type");
      }
    }
    template<> DLLExport
      boost::shared_ptr<const Mantid::DataObjects::Workspace2D> PropertyManager::getValue<boost::shared_ptr<const  Mantid::DataObjects::Workspace2D> >(const std::string &name) const
    {
      PropertyWithValue<boost::shared_ptr<const  Mantid::DataObjects::Workspace2D> >* prop = 
        dynamic_cast<PropertyWithValue<boost::shared_ptr<const Mantid::DataObjects::Workspace2D> >*>(getPointerToProperty(name));
      if (prop)
      {
        return *prop;
      }
      else
      {
        throw std::runtime_error("Attempt to assign property of incorrect type");
      }
    }
  } // namespace Kernel
} // namespace Mantid

///\endcond TEMPLATE
