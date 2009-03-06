#include <iomanip>
#include <iostream>
#include <vector>

#include "MantidAPI/AlgorithmManager.h"
#include "MantidAPI/Algorithm.h"
#include "MantidAPI/AlgorithmProxy.h"
#include "MantidKernel/Exception.h"

using namespace Mantid::Kernel;

namespace Mantid
{
  namespace API
  {

    /// Private Constructor for singleton class
    AlgorithmManagerImpl::AlgorithmManagerImpl(): g_log(Kernel::Logger::get("AlgorithmManager")), no_of_alg(0)
    {
      std::cerr << "Algorithm Manager created." << std::endl;
      g_log.debug() << "Algorithm Manager created." << std::endl;
    }

    /** Private destructor
    *  Prevents client from calling 'delete' on the pointer handed 
    *  out by Instance
    */
    AlgorithmManagerImpl::~AlgorithmManagerImpl()
    {
      std::cerr << "Algorithm Manager destroyed." << std::endl;
      //		g_log.debug() << "Algorithm Manager destroyed." << std::endl;
    }

    /** Creates an instance of an algorithm, but does not own that instance
    * 
    *  @param  algName The name of the algorithm required
    *  @param  version The version of the algorithm required, if not defined most recent version is used -> version =-1
    *  @return A pointer to the created algorithm
    *  @throw  NotFoundError Thrown if algorithm requested is not registered
    */
    IAlgorithm_sptr AlgorithmManagerImpl::createUnmanaged(const std::string& algName,const int& version) const
    {
        return AlgorithmFactory::Instance().create(algName,version);                // Throws on fail:
    }

    /** Gets the names and categories of all the currently available algorithms
    *
    *  \return A vector of pairs of algorithm names and categories
    */
    const std::vector<std::pair<std::string,std::string> > 
      AlgorithmManagerImpl::getNamesAndCategories() const
    {
      std::vector<std::pair<std::string,std::string> > retVector;

      for (unsigned int i=0; i < regAlg.size(); ++i)
      {
        std::pair<std::string,std::string> alg(regAlg[i]->name(),regAlg[i]->category());
        retVector.push_back(alg);
      }

      return retVector;
    }

    /** Creates and initialises an instance of an algorithm
    *
    *  @param  algName The name of the algorithm required
    *  @param  version The version of the algorithm required, if not defined most recent version is used -> version =-1
    *  @return A pointer to the created algorithm
    *  @throw  NotFoundError Thrown if algorithm requested is not registered
    *  @throw  std::runtime_error Thrown if properties string is ill-formed
    */
    IAlgorithm_sptr AlgorithmManagerImpl::create(const std::string& algName, const int& version)
    {
      try
      {
          IAlgorithm_sptr alg = AlgorithmFactory::Instance().create(algName,version);// Throws on fail:
          regAlg.push_back(IAlgorithm_sptr(new AlgorithmProxy(alg)));      
          regAlg.back()->initialize();
      }
      catch(std::runtime_error& ex)
      {
        g_log.error()<<"AlgorithmManager:: Unable to create algorithm "<< algName <<ex.what() << std::endl;  
        throw std::runtime_error("AlgorithmManager:: Unable to create algorithm " + algName); 
      }
      no_of_alg++;		
      return regAlg.back();
    }

    /// deletes all registered algorithms
    void AlgorithmManagerImpl::clear()
    {
      std::vector<Algorithm_sptr>::iterator vc;
      regAlg.clear();
      no_of_alg=0;
      return;
    }

    /// Returns a shared pointer by algorithm id
    IAlgorithm_sptr AlgorithmManagerImpl::getAlgorithm(AlgorithmID id)const
    {
        for( std::vector<IAlgorithm_sptr>::const_iterator a = regAlg.begin();a!=regAlg.end();a++)
            if ((**a).getAlgorithmID() == id) return *a;
        return IAlgorithm_sptr();
    }

  } // namespace API
} // namespace Mantid
