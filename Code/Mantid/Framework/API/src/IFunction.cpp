//----------------------------------------------------------------------
// Includes
//----------------------------------------------------------------------
#include "MantidKernel/Exception.h"
#include "MantidKernel/Logger.h"
#include "MantidAPI/IFunction.h"
#include "MantidAPI/Jacobian.h"
#include "MantidAPI/IConstraint.h"
#include "MantidAPI/ParameterTie.h"
#include "MantidAPI/Expression.h"
#include "MantidAPI/ConstraintFactory.h"
#include "MantidAPI/FunctionFactory.h"

#include "MantidAPI/MatrixWorkspace.h"
#include "MantidAPI/IFunctionWithLocation.h"
#include "MantidGeometry/Instrument/ParameterMap.h"
#include "MantidGeometry/Instrument/Component.h"
#include "MantidGeometry/Instrument/DetectorGroup.h"
#include "MantidGeometry/Instrument/FitParameter.h"
#include "MantidKernel/UnitFactory.h"
#include "MantidKernel/MultiThreaded.h"
#include "MantidKernel/ProgressBase.h"
#include "MantidGeometry/muParser_Silent.h"

#include <boost/lexical_cast.hpp>

#include <Poco/StringTokenizer.h>

#include <limits>
#include <sstream>
#include <iostream> 

namespace Mantid
{
namespace API
{
  
  using namespace Geometry;

  Kernel::Logger& IFunction::g_log = Kernel::Logger::get("IFunction");

/**
 * Destructor
 */
  IFunction::~IFunction()
  {
    if (m_handler)
    {
      delete m_handler;
    }
  }

  /**
   * Virtual copy constructor
   */
  boost::shared_ptr<IFunction> IFunction::clone() const
  {
    return FunctionFactory::Instance().createInitialized( this->asString() );
  }

/**
 * Attach a progress reporter
 * @param reporter :: A pointer to a progress reporter that can be called during function evaluation
 */
void IFunction::setProgressReporter(Kernel::ProgressBase *reporter)
{
  m_progReporter = reporter;
  m_progReporter->setNotifyStep(0.01);
}

/**
 * If a reporter object is set, reports progress with an optional message
 * @param msg :: A message to display (default = "")
 */
void IFunction::reportProgress(const std::string & msg) const
{
  if(m_progReporter)
  {
    const_cast<Kernel::ProgressBase*>(m_progReporter)->report(msg);
  }
}

/**
 *
 * @returns true if a progress reporter is set & evalaution has been requested to stop
 */
bool IFunction::cancellationRequestReceived() const
{
  if (m_progReporter) return m_progReporter->hasCancellationBeenRequested();
  else return false;
}

/** Base class implementation calculates the derivatives numerically.
 * @param domain :: The domain of the function
 * @param jacobian :: A Jacobian matrix. It is expected to have dimensions of domain.size() by nParams().
 */
void IFunction::functionDeriv(const FunctionDomain& domain, Jacobian& jacobian)
{
  calNumericalDeriv(domain, jacobian);
}

/**
 * Ties a parameter to other parameters
 * @param parName :: The name of the parameter to tie.
 * @param expr ::    A math expression 
 * @return newly ties parameters
 */
ParameterTie* IFunction::tie(const std::string& parName,const std::string& expr, bool isDefault)
{
  ParameterTie* ti = new ParameterTie(this,parName,expr,isDefault);
  addTie(ti);
  this->fix(getParameterIndex(*ti));
  return ti;
}

/**
 * Add ties to the function.
 * @param ties :: Comma-separated list of name=value pairs where name is a parameter name and value
 *  is a math expression tying the parameter to other parameters or a constant.
 */
void IFunction::addTies(const std::string& ties, bool isDefault)
{
  Expression list;
  list.parse(ties);
  list.toList();
  for(auto t = list.begin(); t != list.end(); ++t)
  {
    if (t->name() == "=" && t->size() >= 2)
    {
      size_t n = t->size() - 1;
      const std::string value = (*t)[n].str();
      for( size_t i = n; i != 0; )
      {
        --i;
        this->tie( (*t)[i].name(), value, isDefault );
      }
    }
  }
}

/** Removes the tie off a parameter. The parameter becomes active
 * This method can be used when constructing and editing the IFunction in a GUI
 * @param parName :: The name of the parameter which ties will be removed.
 */
void IFunction::removeTie(const std::string& parName)
{
  size_t i = parameterIndex(parName);
  this->removeTie(i);
}

/**
 * Writes a string that can be used in Fit.IFunction to create a copy of this IFunction
 * @return string representation of the function
 */
std::string IFunction::asString()const
{
  std::ostringstream ostr;
  ostr << "name="<<this->name();
  // print the attributes
  std::vector<std::string> attr = this->getAttributeNames();
  for(size_t i=0;i<attr.size();i++)
  {
    std::string attName = attr[i];
    std::string attValue = this->getAttribute(attr[i]).value();
    if (!attValue.empty())
    {
      ostr<<','<<attName<<'='<<attValue;
    }
  }
  // print the parameters
  for(size_t i=0;i<nParams();i++)
  {
    const ParameterTie* tie = getTie(i);
    if ( !tie || !tie->isDefault() )
    {
      ostr<<','<<parameterName(i)<<'='<<getParameter(i);
    }
  }
  // collect non-default constraints
  std::string constraints;
  for(size_t i=0;i<nParams();i++)
  {
    const IConstraint* c = getConstraint(i);
    if ( c && !c->isDefault() )
    {
      std::string tmp = c->asString();
      if (!tmp.empty())
      {
        if (!constraints.empty())
        {
          constraints += ",";
        }
        constraints += tmp;
      }
    }
  }
  // print constraints
  if (!constraints.empty())
  {
    ostr << ",constraints=(" << constraints << ")";
  }
  // collect the non-default ties
  std::string ties;
  for(size_t i=0;i<nParams();i++)
  {
    const ParameterTie* tie = getTie(i);
    if ( tie && !tie->isDefault() )
    {
      std::string tmp = tie->asString(this);
      if (!tmp.empty())
      {
        if (!ties.empty())
        {
          ties += ",";
        }
        ties += tmp;
      }
    }
  }
  // print the ties
  if (!ties.empty())
  {
    ostr << ",ties=(" << ties << ")";
  }
  return ostr.str();
}

/** Add a list of constraints from a string
 * @param str :: A comma-separated list of constraint expressions.
 */
void IFunction::addConstraints(const std::string& str, bool isDefault)
{
  Expression list;
  list.parse(str);
  list.toList();
  for(auto expr = list.begin(); expr != list.end(); ++expr)
  {
    IConstraint* c = ConstraintFactory::Instance().createInitialized(this,*expr,isDefault);
    this->addConstraint(c);
  }
}

/**
 * Return a vector with all parameter names.
 */
std::vector<std::string> IFunction::getParameterNames() const
{
  std::vector<std::string> out;
  for(size_t i = 0; i < nParams(); ++i)
  {
    out.push_back(parameterName(i));
  }
  return out;
}

/** Set a function handler
 * @param handler :: A new handler
 */
void IFunction::setHandler(FunctionHandler* handler)
{
  m_handler = handler;
  if (handler && handler->function().get() != this)
  {
    throw std::runtime_error("Function handler points to a different function");
  }
  m_handler->init();
}

/// Function to return all of the categories that contain this function
const std::vector<std::string> IFunction::categories() const
{
  std::vector < std::string > res;
  Poco::StringTokenizer tokenizer(category(), categorySeparator(),
      Poco::StringTokenizer::TOK_TRIM | Poco::StringTokenizer::TOK_IGNORE_EMPTY);
  Poco::StringTokenizer::Iterator h = tokenizer.begin();

  for (; h != tokenizer.end(); ++h)
  {
    res.push_back(*h);
  }

  return res;
}

/**
 * Operator <<
 * @param ostr :: The output stream
 * @param f :: The IFunction
 */
std::ostream& operator<<(std::ostream& ostr,const IFunction& f)
{
  ostr << f.asString();
  return ostr;
}

/**
 * Const attribute visitor returning the type of the attribute
 */
class AttType: public IFunction::ConstAttributeVisitor<std::string>
{
protected:
  /// Apply if string
  std::string apply(const std::string&)const{return "std::string";}
  /// Apply if int
  std::string apply(const int&)const{return "int";}
  /// Apply if double
  std::string apply(const double&)const{return "double";}
};

std::string IFunction::Attribute::type()const
{
  AttType tmp;
  return apply(tmp);
}

/**
 * Const attribute visitor returning the type of the attribute
 */
class AttValue: public IFunction::ConstAttributeVisitor<std::string>
{
public:
  AttValue(bool quoteString=false) : 
    IFunction::ConstAttributeVisitor<std::string>(),
    m_quoteString(quoteString) 
  {
  }

protected:
  /// Apply if string
  std::string apply(const std::string& str)const
  {
    return (m_quoteString) ? std::string("\"" + str + "\"") : str;
  }
  /// Apply if int
  std::string apply(const int& i)const{return boost::lexical_cast<std::string>(i);}
  /// Apply if double
  std::string apply(const double& d)const{return boost::lexical_cast<std::string>(d);}

private:
  /// Flag to quote a string value returned
  bool m_quoteString;
};

std::string IFunction::Attribute::value()const
{
  AttValue tmp(m_quoteValue);
  return apply(tmp);
}

/**
 * Return the attribute as a string if it is a string.
 */
std::string IFunction::Attribute::asString()const
{
  if( m_quoteValue ) return asQuotedString();
  
  try
  {
    return boost::get<std::string>(m_data);
  }
  catch(...)
  {
    throw std::runtime_error("Trying to access a "+type()+" attribute "
      "as string");
  }
}

/**
 * Return the attribute as a quoted string if it is a string.
 */
std::string IFunction::Attribute::asQuotedString()const
{
  std::string attr;

  try
  {
    attr = boost::get<std::string>(m_data);
  }
  catch(...)
  {
    throw std::runtime_error("Trying to access a "+type()+" attribute "
      "as string");
  }
  std::string quoted(attr);
  if( *(attr.begin()) != '\"' ) quoted = "\"" + attr;
  if( *(quoted.end() - 1) != '\"' ) quoted += "\"";

  return quoted;
}

/**
 * Return the attribute as an unquoted string if it is a string.
 */
std::string IFunction::Attribute::asUnquotedString()const
{
  std::string attr;

  try
  {
    attr = boost::get<std::string>(m_data);
  }
  catch(...)
  {
    throw std::runtime_error("Trying to access a "+type()+" attribute "
      "as string");
  }
  std::string unquoted(attr);
  if ( attr.empty() ) return "";
  if( *(attr.begin()) == '\"' ) unquoted = std::string(attr.begin() + 1, attr.end() - 1);
  if( *(unquoted.end() - 1) == '\"' ) unquoted = std::string(unquoted.begin(), unquoted.end() - 1);
  
  return unquoted;
}

/**
 * Return the attribute as an int if it is a int.
 */
int IFunction::Attribute::asInt()const
{
  try
  {
    return boost::get<int>(m_data);
  }
  catch(...)
  {
    throw std::runtime_error("Trying to access a "+type()+" attribute "
      "as int");
  }
}

/**
 * Return the attribute as a double if it is a double.
 */
double IFunction::Attribute::asDouble()const
{
  try
  {
    return boost::get<double>(m_data);
  }
  catch(...)
  {
    throw std::runtime_error("Trying to access a "+type()+" attribute "
      "as double");
  }
}

/** Sets new value if attribute is a string. If the type is wrong 
 * throws an exception
 * @param str :: The new value
 */
void IFunction::Attribute::setString(const std::string& str)
{
  try
  {
    boost::get<std::string>(m_data) = str;
  }
  catch(...)
  {
    throw std::runtime_error("Trying to access a "+type()+" attribute "
      "as string");
  }
}

/** Sets new value if attribute is a double. If the type is wrong 
 * throws an exception
 * @param d :: The new value
 */
void IFunction::Attribute::setDouble(const double& d)
{
  try
  {
    boost::get<double>(m_data) = d;
  }
  catch(...)
  {
    throw std::runtime_error("Trying to access a "+type()+" attribute "
      "as double");
  }
}

/** Sets new value if attribute is an int. If the type is wrong 
 * throws an exception
 * @param i :: The new value
 */
void IFunction::Attribute::setInt(const int& i)
{
  try
  {
    boost::get<int>(m_data) = i;
  }
  catch(...)
  {
    throw std::runtime_error("Trying to access a "+type()+" attribute "
      "as int");
  }
}

/**
 * Attribute visitor setting new value to an attribute
 */
class SetValue: public IFunction::AttributeVisitor<>
{
public:
  /**
   * Constructor
   * @param value :: The value to set
   */
  SetValue(const std::string& value):m_value(value){}
protected:
  /// Apply if string
  void apply(std::string& str)const{str = m_value;}
  /// Apply if int
  void apply(int& i)const
  {
    std::istringstream istr(m_value+" ");
    istr >> i;
    if (!istr.good()) throw std::invalid_argument("Failed to set int attribute "
      "from string "+m_value);
  }
  /// Apply if double
  void apply(double& d)const
  {
    std::istringstream istr(m_value+" ");
    istr >> d;
    if (!istr.good()) throw std::invalid_argument("Failed to set double attribute "
      "from string "+m_value);
  }
private:
  std::string m_value; ///<the value as a string
};

/** Set value from a string. Throws exception if the string has wrong format
 * @param str :: String representation of the new value
 */
void IFunction::Attribute::fromString(const std::string& str)
{
  SetValue tmp(str);
  apply(tmp);
}

/// Value of i-th active parameter. Override this method to make fitted parameters different from the declared
double IFunction::activeParameter(size_t i)const 
{
  if ( !isActive(i) )
  {
    throw std::runtime_error("Attempt to use an inactive parameter");
  }
  return getParameter(i);
}

/// Set new value of i-th active parameter. Override this method to make fitted parameters different from the declared
void IFunction::setActiveParameter(size_t i, double value) 
{
  if ( !isActive(i) )
  {
    throw std::runtime_error("Attempt to use an inactive parameter");
  }
  setParameter(i,value);
}

/// Returns the name of active parameter i
std::string IFunction::nameOfActive(size_t i)const 
{
  if ( !isActive(i) )
  {
    throw std::runtime_error("Attempt to use an inactive parameter");
  }
  return parameterName(i);
}

/// Returns the name of active parameter i
std::string IFunction::descriptionOfActive(size_t i)const 
{
  if ( !isActive(i) )
  {
    throw std::runtime_error("Attempt to use an inactive parameter");
  }
  return parameterDescription(i);
}

/** Calculate numerical derivatives.
 * @param domain :: The domain of the function
 * @param jacobian :: A Jacobian matrix. It is expected to have dimensions of domain.size() by nParams().
 */
void IFunction::calNumericalDeriv(const FunctionDomain& domain, Jacobian& jacobian)
{
    const double minDouble = std::numeric_limits<double>::min();
    const double epsilon = std::numeric_limits<double>::epsilon() * 100;
    double stepPercentage = 0.001; // step percentage
    double step; // real step
    double cutoff = 100.0*minDouble/stepPercentage;
    size_t nParam = nParams();
    size_t nData = domain.size();

    FunctionValues minusStep(domain);
    FunctionValues plusStep(domain);
    std::vector<double> params(nParam);

    //PARALLEL_CRITICAL(numeric_deriv)
    {
      applyTies(); // just in case
      function(domain,minusStep);
    }

    for (size_t iP = 0; iP < nParam; iP++)
    {
      if ( isActive(iP) )
      {
        const double& val = activeParameter(iP);
        if (fabs(val) < cutoff)
        {
          step = epsilon;
        }
        else
        {
          step = val*stepPercentage;
        }

        double paramPstep = val + step;

        //PARALLEL_CRITICAL(numeric_deriv)
        {
          setActiveParameter(iP, paramPstep);
          applyTies(); 
          function(domain,plusStep);
          setActiveParameter(iP, val);
        }

        step = paramPstep - val;
        for (size_t i = 0; i < nData; i++) 
        {
          jacobian.set(i,iP, 
            (plusStep.getCalculated(i) - minusStep.getCalculated(i))/step);
        }
      }
    }
}

/** Initialize the function providing it the workspace
 * @param workspace :: The workspace to set
 * @param wi :: The workspace index
 * @param startX :: The lower bin index
 * @param endX :: The upper bin index
 */
void IFunction::setMatrixWorkspace(boost::shared_ptr<const API::MatrixWorkspace> workspace,size_t wi,double startX, double endX)
{
  UNUSED_ARG(startX);
  UNUSED_ARG(endX);

  if (!workspace) return; // unset the workspace

  try
  {

    // check if parameter are specified in instrument definition file

    const Geometry::ParameterMap& paramMap = workspace->instrumentParameters();


    Geometry::IDetector_const_sptr det;
    size_t numDetectors = workspace->getSpectrum(wi)->getDetectorIDs().size() ;
    if (numDetectors > 1)
    {
      // If several detectors are on this workspace index, just use the ID of the first detector
      // Note JZ oct 2011 - I'm not sure why the code uses the first detector and not the group. Ask Roman.
      Instrument_const_sptr inst = workspace->getInstrument();
      det = inst->getDetector( *workspace->getSpectrum(wi)->getDetectorIDs().begin() );
    }
    else
      // Get the detector (single) at this workspace index
      det = workspace->getDetector(wi);;

    for (size_t i = 0; i < nParams(); i++)
    {
      if ( !isExplicitlySet(i) )
      {
        Geometry::Parameter_sptr param = paramMap.getRecursive(&(*det), parameterName(i), "fitting");
        if (param != Geometry::Parameter_sptr())
        {
          // get FitParameter
          const Geometry::FitParameter& fitParam = param->value<Geometry::FitParameter>();

          // check first if this parameter is actually specified for this function
          if ( name().compare(fitParam.getFunction()) == 0 )
          {
            // update value          
            IFunctionWithLocation* testWithLocation = dynamic_cast<IFunctionWithLocation*>(this);
            if ( testWithLocation == NULL || 
              (fitParam.getLookUpTable().containData() == false && fitParam.getFormula().compare("") == 0) )
            {
              setParameter(i, fitParam.getValue());
            }
            else
            {
              double centreValue = testWithLocation->centre();
              Kernel::Unit_sptr centreUnit;  // unit of value used in formula or to look up value in lookup table
              if ( fitParam.getFormula().compare("") == 0 )
                centreUnit = fitParam.getLookUpTable().getXUnit();  // from table
              else
              {
                if ( !fitParam.getFormulaUnit().empty() )
                {
                  try
                  {
                    centreUnit = Kernel::UnitFactory::Instance().create(fitParam.getFormulaUnit());  // from formula
                  }
                  catch (...)
                  {
                    g_log.warning() << fitParam.getFormulaUnit() << " Is not an recognised formula unit for parameter " 
                                    << fitParam.getName() << "\n";
                  }
                }
              }

              // if unit specified convert centre value to unit required by formula or look-up-table
              if (centreUnit)
                centreValue = convertValue(centreValue, centreUnit, workspace, wi);

              double paramValue = fitParam.getValue(centreValue);

              // this returned param value by say a formula or a look-up-table may have
              // a unit of its own. If this is specified, try the following
              if ( fitParam.getFormula().compare("") == 0 )
              {
                // so from look up table
                Kernel::Unit_sptr resultUnit = fitParam.getLookUpTable().getYUnit();  // from table
                paramValue /= convertValue(1.0, resultUnit, workspace, wi);
              }
              else
              {
                // so from formula 

                std::string resultUnitStr = fitParam.getResultUnit();

                if ( !resultUnitStr.empty() )
                {
                  std::vector<std::string> allUnitStr = Kernel::UnitFactory::Instance().getKeys();
                  for (unsigned iUnit = 0; iUnit < allUnitStr.size(); iUnit++)
                  {
                    size_t found = resultUnitStr.find(allUnitStr[iUnit]);
                    if ( found != std::string::npos )
                    {
                      size_t len = allUnitStr[iUnit].size();
                      std::stringstream readDouble;
                      Kernel::Unit_sptr unt = Kernel::UnitFactory::Instance().create(allUnitStr[iUnit]);
                      readDouble << 1.0 / 
                        convertValue(1.0, unt, workspace, wi);
                      resultUnitStr.replace(found, len, readDouble.str());
                    }
                  }  // end for

                  try
                  {
                    mu::Parser p;
                    p.SetExpr(resultUnitStr);
                    paramValue *= p.Eval();
                  }
                  catch (mu::Parser::exception_type &e)
                  {
                    g_log.error() << "Cannot convert formula unit to workspace unit"
                      << " Formula unit which cannot be passed is " << resultUnitStr 
                      << ". Muparser error message is: " << e.GetMsg() << std::endl;
                  }
                } // end if
              } // end else
              

              setParameter(i, paramValue);
            }  // end of update parameter value

            // add tie if specified for this parameter in instrument definition file
            if ( fitParam.getTie().compare("") )
            {  
              std::ostringstream str;
              str << getParameter(i);
              tie(parameterName(i), str.str());
            }

            // add constraint if specified for this parameter in instrument definition file
            if ( fitParam.getConstraint().compare("") )
            {  
              IConstraint* constraint = ConstraintFactory::Instance().createInitialized(this, fitParam.getConstraint());
              if ( fitParam.getConstraintPenaltyFactor().compare("") )
              {
                double penalty;
                try
                {
                  penalty = atof(fitParam.getConstraintPenaltyFactor().c_str());
                  constraint->setPenaltyFactor(penalty);
                }
                catch (...)
                {
                  g_log.warning() << "Can't set penalty factor for constraint\n";
                }
              }
              addConstraint(constraint);
            }
          }
        }
      }
    }
  }
  catch(...)
  {
  }
}

/** Convert a value from unit defined in workspace (ws) to outUnit
 *
 *  @param value ::   assumed to be in unit of workspace
 *  @param outUnit ::  unit to convert to
 *  @param ws ::      workspace
 *  @param wsIndex :: workspace index
 *  @return converted value
 */
double IFunction::convertValue(double value, Kernel::Unit_sptr& outUnit, 
                               boost::shared_ptr<const MatrixWorkspace> ws,
                               size_t wsIndex)const
{
  double retVal = value;
  Kernel::Unit_sptr wsUnit = ws->getAxis(0)->unit();

  // if unit required by formula or look-up-table different from ws-unit then 
  if ( outUnit->unitID().compare(wsUnit->unitID()) != 0 )
  {
    // first check if it is possible to do a quick convertion convert
    double factor,power;
    if (wsUnit->quickConversion(*outUnit,factor,power) )
    {
      retVal = factor * std::pow(retVal,power);
    }
    else
    {
      double l1,l2,twoTheta;

      // Get l1, l2 and theta  (see also RemoveBins.calculateDetectorPosition())
      Instrument_const_sptr instrument = ws->getInstrument();
      Geometry::IObjComponent_const_sptr sample = instrument->getSample();
      if (sample == NULL)
      {
        g_log.error() << "No sample defined instrument. Cannot convert units for function\n"
                      << "Ignore convertion."; 
        return value; 
      }
      l1 = instrument->getSource()->getDistance(*sample);
      Geometry::IDetector_const_sptr det = ws->getDetector(wsIndex);
      if ( ! det->isMonitor() )
      {
        l2 = det->getDistance(*sample);
        twoTheta = ws->detectorTwoTheta(det);
      }
      else  // If this is a monitor then make l1+l2 = source-detector distance and twoTheta=0
      {
        l2 = det->getDistance(*(instrument->getSource()));
        l2 = l2 - l1;
        twoTheta = 0.0;
      }

      std::vector<double> endPoint;
      endPoint.push_back(retVal);
      std::vector<double> emptyVec;
      wsUnit->toTOF(endPoint,emptyVec,l1,l2,twoTheta,0,0.0,0.0);
      outUnit->fromTOF(endPoint,emptyVec,l1,l2,twoTheta,0,0.0,0.0);
      retVal = endPoint[0];
    }
  }  
  return retVal;
}

/** Convert values from unit defined in workspace (ws) to outUnit
 *
 *  @param values ::   As input: assumed to be in unit of workspace. 
 *                  As output: in unit of outUnit
 *  @param outUnit ::  unit to convert to
 *  @param ws ::      workspace
 *  @param wsIndex :: workspace index
 */
void IFunction::convertValue(std::vector<double>& values, Kernel::Unit_sptr& outUnit, 
                               boost::shared_ptr<const MatrixWorkspace> ws,
                               size_t wsIndex) const
{
  Kernel::Unit_sptr wsUnit = ws->getAxis(0)->unit();

  // if unit required by formula or look-up-table different from ws-unit then 
  if ( outUnit->unitID().compare(wsUnit->unitID()) != 0 )
  {
    // first check if it is possible to do a quick convertion convert
    double factor,power;
    if (wsUnit->quickConversion(*outUnit,factor,power) )
    {
      for (size_t i = 0; i < values.size(); i++)
        values[i] = factor * std::pow(values[i],power);
    }
    else
    {
      double l1,l2,twoTheta;

      // Get l1, l2 and theta  (see also RemoveBins.calculateDetectorPosition())
      Instrument_const_sptr instrument = ws->getInstrument();
      Geometry::IObjComponent_const_sptr sample = instrument->getSample();
      if (sample == NULL)
      {
        g_log.error() << "No sample defined instrument. Cannot convert units for function\n"
                      << "Ignore convertion."; 
        return; 
      }
      l1 = instrument->getSource()->getDistance(*sample);
      Geometry::IDetector_const_sptr det = ws->getDetector(wsIndex);
      if ( ! det->isMonitor() )
      {
        l2 = det->getDistance(*sample);
        twoTheta = ws->detectorTwoTheta(det);
      }
      else  // If this is a monitor then make l1+l2 = source-detector distance and twoTheta=0
      {
        l2 = det->getDistance(*(instrument->getSource()));
        l2 = l2 - l1;
        twoTheta = 0.0;
      }

      std::vector<double> emptyVec;
      wsUnit->toTOF(values,emptyVec,l1,l2,twoTheta,0,0.0,0.0);
      outUnit->fromTOF(values,emptyVec,l1,l2,twoTheta,0,0.0,0.0);
    }
  }  
}

} // namespace API
} // namespace Mantid

///\cond TEMPLATE
namespace Mantid
{
namespace Kernel
{

template<> MANTID_API_DLL
boost::shared_ptr<Mantid::API::IFunction> IPropertyManager::getValue<boost::shared_ptr<Mantid::API::IFunction> >(const std::string &name) const
{
  PropertyWithValue<boost::shared_ptr<Mantid::API::IFunction> >* prop =
                    dynamic_cast<PropertyWithValue<boost::shared_ptr<Mantid::API::IFunction> >*>(getPointerToProperty(name));
  if (prop)
  {
    return *prop;
  }
  else
  {
    std::string message = "Attempt to assign property "+ name +" to incorrect type. Expected IFitFunction.";
    throw std::runtime_error(message);
  }
}

} // namespace Kernel
} // namespace Mantid
///\endcond TEMPLATE
