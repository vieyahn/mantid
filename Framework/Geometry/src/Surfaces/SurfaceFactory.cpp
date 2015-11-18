#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <cmath>
#include <vector>
#include <map>
#include <list>
#include <stack>
#include <string>
#include <algorithm>

#include "MantidKernel/Exception.h"
#include "MantidKernel/make_unique.h"
#include "MantidKernel/Matrix.h"
#include "MantidKernel/V3D.h"
#include "MantidKernel/Strings.h"
#include "MantidGeometry/Surfaces/BaseVisit.h"
#include "MantidGeometry/Surfaces/Surface.h"
#include "MantidGeometry/Surfaces/Quadratic.h"
#include "MantidGeometry/Surfaces/Plane.h"
#include "MantidGeometry/Surfaces/Cylinder.h"
#include "MantidGeometry/Surfaces/Cone.h"
#include "MantidGeometry/Surfaces/General.h"
#include "MantidGeometry/Surfaces/Sphere.h"
//#include "MantidGeometry/Surfaces/Torus.h"
#include "MantidGeometry/Surfaces/SurfaceFactory.h"

namespace Mantid {

namespace Geometry {

SurfaceFactory *SurfaceFactory::FOBJ(0);

SurfaceFactory *SurfaceFactory::Instance()
/**
  Effective new command / this command
  @return Single instance of SurfaceFactory
*/
{
  if (!FOBJ) {
    FOBJ = new SurfaceFactory();
  }
  return FOBJ;
}

SurfaceFactory::SurfaceFactory()
/**
  Constructor
*/
{
  registerSurface();
}

SurfaceFactory::SurfaceFactory(const SurfaceFactory &A)
    : ID(A.ID)
/**
  Copy constructor
  @param A :: Object to copy
*/
{
  MapType::const_iterator vc;
  for (vc = A.SGrid.begin(); vc != A.SGrid.end(); ++vc)
    SGrid.push_back(MapType::value_type(vc->first, vc->second->clone()));
}

SurfaceFactory::~SurfaceFactory() {}

void SurfaceFactory::registerSurface()
/**
  Register tallies to be used
*/
{
  using Mantid::Kernel::make_unique;
  SGrid.push_back(std::make_pair("Plane", make_unique<Plane>()));
  SGrid.push_back(std::make_pair("Cylinder", make_unique<Cylinder>()));
  SGrid.push_back(std::make_pair("Cone", make_unique<Cone>()));
  // SGrid["Torus"]=new Torus;
  SGrid.push_back(std::make_pair("General", make_unique<General>()));
  SGrid.push_back(std::make_pair("Sphere", make_unique<Sphere>()));

  ID['c'] = "Cylinder";
  ID['k'] = "Cone";
  ID['g'] = "General";
  ID['p'] = "Plane";
  ID['s'] = "Sphere";
  // ID['t']="Torus";
  return;
}

namespace {
class KeyEquals {
public:
  KeyEquals(std::string key) : m_key(std::move(key)) {}
  bool
  operator()(const std::pair<std::string, std::unique_ptr<Surface>> &element) {
    return m_key == element.first;
  }

private:
  std::string m_key;
};
}

std::unique_ptr<Surface>
SurfaceFactory::createSurface(const std::string &Key) const
/**
  Creates an instance of tally
  given a valid key.

  @param Key :: Item to get
  @throw NotFoundError for the key if not found
  @return new tally object.
*/
{
  MapType::const_iterator vc;
  vc = std::find_if(SGrid.begin(), SGrid.end(), KeyEquals(Key));
  if (vc == SGrid.end()) {
    throw Kernel::Exception::NotFoundError("SurfaceFactory::createSurface",
                                           Key);
  }
  return vc->second->clone();
}

std::unique_ptr<Surface>
SurfaceFactory::createSurfaceID(const std::string &Key) const
/**
  Creates an instance of tally
  given a valid key.

  @param Key :: Form of first ID
  @throw NotFoundError for the key if not found
  @return new tally object.
*/
{
  std::map<char, std::string>::const_iterator mc;

  mc = (Key.empty()) ? ID.end() : ID.find(static_cast<char>(tolower(Key[0])));
  if (mc == ID.end()) {
    throw Kernel::Exception::NotFoundError("SurfaceFactory::createSurfaceID",
                                           Key);
  }

  return createSurface(mc->second);
}

std::unique_ptr<Surface>
SurfaceFactory::processLine(const std::string &Line) const
/**
  Creates an instance of a surface
  given a valid line

  @param Line :: Full description of line
  @throw InContainerError for the key if not found
  @return new surface object.
*/
{
  std::string key;
  if (!Mantid::Kernel::Strings::convert(Line, key))
    throw Kernel::Exception::NotFoundError("SurfaceFactory::processLine", Line);

  std::unique_ptr<Surface> X = createSurfaceID(key);
  if (X->setSurface(Line)) {
    std::cerr << "X:: " << X->setSurface(Line) << std::endl;
    throw Kernel::Exception::NotFoundError("SurfaceFactory::processLine", Line);
  }

  return X;
}

} // NAMESPACE Geometry

} // NAMESPACE Mantid