#ifndef SLICE_VIEWER_NON_ORTHOGONAL_TEST_H_
#define SLICE_VIEWER_NON_ORTHOGONAL_TEST_H_

#include <cxxtest/TestSuite.h>
#include "MantidQtSliceViewer/NonOrthogonal.h"
#include "MantidAPI/IMDEventWorkspace.h"
#include "MantidAPI/IMDHistoWorkspace.h"
#include "MantidTestHelpers/MDEventsTestHelper.h"
#include "MantidKernel/SpecialCoordinateSystem.h"
#include "MantidKernel/MDUnitFactory.h"
#include "MantidKernel/Unit.h"
#include "MantidKernel/UnitLabelTypes.h"
#include "MantidGeometry/MDGeometry/HKL.h"
#include "MantidGeometry/MDGeometry/QSample.h"
#include "MantidAPI/AlgorithmManager.h"
#include "MantidDataObjects/CoordTransformAffine.h"
#include "MantidCrystal/SetUB.h"


using namespace Mantid::DataObjects;
using namespace Mantid::Kernel;
using namespace Mantid::API;
using namespace Mantid::SliceViewer;
using namespace Mantid::Geometry;

class NonOrthogonalTest : public CxxTest::TestSuite {
public:
  // This pair of boilerplate methods prevent the suite being created statically
  // This means the constructor isn't called when running other tests
  static NonOrthogonalTest *createSuite() {
    return new NonOrthogonalTest();
  }
  static void destroySuite(NonOrthogonalTest *suite) {
    delete suite;
  }

  void
  test_that_orthogonal_event_workspace_does_not_require_skew_matrix() {
    // Arrange
    auto eventWorkspace = getOrthogonalEventWorkspace();

    // Act
    auto requiresSkew = requiresSkewMatrix(eventWorkspace);

    // Assert
    TSM_ASSERT("Should not require a skew matrix", !requiresSkew);
  }

  void test_that_orthogonal_event_workspace_throws() {
    // Arrange
    auto eventWorkspace = getOrthogonalEventWorkspace();

    // Act + Assert
    Mantid::Kernel::DblMatrix matrix(3,3,true);
    TSM_ASSERT_THROWS("Orthogonal data sets cannot be investigated", provideSkewMatrix(matrix, eventWorkspace),
                      std::invalid_argument &);
  }

  void test_that_non_orhtogonal_matrix_creates_expected_skew_matrix() {
    // Arrange
    auto eventWorkspace = getNonOrthogonalEventWorkspace(true);

    // Act
    Mantid::Kernel::DblMatrix matrix(4,4,true);
    provideSkewMatrix(matrix, eventWorkspace);

    // Assert
    std::cout << "===============" <<std::endl;
    std::cout << matrix.numCols()<<std::endl;;
    std::cout << matrix.numRows()<<std::endl;;
    std::cout << std::endl;
    for (size_t x = 0; x < 3; ++x) {
        for (size_t y = 0; y < 3; ++y) {
          std::cout << matrix[x][y] << " ";
        }
        std::cout << std::endl;
    }
  }


private:
  Mantid::API::IMDEventWorkspace_sptr getOrthogonalEventWorkspace() {
    ReciprocalLatticeUnitFactory factory;
    auto product = factory.create(Units::Symbol::RLU);
    Mantid::Geometry::HKL frame(product);
    auto workspace = MDEventsTestHelper::makeMDEWWithFrames<3>(5, -10, 10, frame);
    return workspace;
  }

  IMDEventWorkspace_sptr getNonOrthogonalEventWorkspace(bool nonUnityTransform, bool wrongCoords = false,
                                      bool forgetUB = false, bool forgetWmat = false,
                                      bool forgetAffmat = false, double scale = 1.0) {

      std::string wsName = "simpleWS";
      IMDEventWorkspace_sptr ws;
      // Set the coordinate system
      if (wrongCoords) {
        Mantid::Geometry::QSample frame;
        ws = MDEventsTestHelper::makeAnyMDEWWithFrames<MDEvent<4>, 4>(
            1, 0.0, 1.0, frame, 1, wsName);

      } else {
        Mantid::Geometry::HKL frame(new Mantid::Kernel::ReciprocalLatticeUnit);
        ws = MDEventsTestHelper::makeAnyMDEWWithFrames<MDEvent<4>, 4>(
            1, 0.0, 1.0, frame, 1, wsName);
      }

      // Set the UB matrix
      if (!forgetUB) {
        Mantid::Crystal::SetUB alg;
        alg.initialize();
        alg.setChild(true);
        alg.setRethrows(true);
        alg.setProperty("Workspace", ws);
        alg.setProperty("a", 3.643 * scale);
        alg.setProperty("b", 3.643);
        alg.setProperty("c", 5.781);
        alg.setProperty("alpha", 90.0);
        alg.setProperty("beta", 90.0);
        alg.setProperty("gamma", 120.0);
        std::vector<double> uVec = {1*scale, 1, 0};
        std::vector<double> vVec = {0, 0, 1};
        alg.setProperty("u", uVec);
        alg.setProperty("v", vVec);
        alg.execute();
      }

      // Create the coordinate transformation information
      std::vector<Mantid::coord_t> affMatVals{1, 0, 0, 0, 0,
                                              0, 0, 1, 0, 0,
                                              0, 0, 0, 1, 0,
                                              0, 1, 0, 0, 0,
                                              0, 0, 0, 0, 1};

      Mantid::DataObjects::CoordTransformAffine affMat(4, 4);
      affMat.setMatrix(Matrix<Mantid::coord_t>(affMatVals));
      if (!forgetAffmat) {
        ws->setTransformToOriginal(affMat.clone(), 0);
      }

      // Create the transform (W) matrix
      // Need it as a vector
      std::vector<double> wMat;
      if (!nonUnityTransform) {
        DblMatrix temp(3, 3, true);
        wMat = temp.getVector();
      } else {
        wMat = {1, 1, 0,
                1, -1, 0,
                0, 0, 1};
      }

      if (!forgetWmat) {
        // Create property for W matrix and add it as log to run object
        PropertyWithValue<std::vector<double>> *p;
        p = new PropertyWithValue<std::vector<double>>("W_MATRIX", wMat);
        ws->getExperimentInfo(0)->mutableRun().addProperty(p, true);
      }

      return ws;
  }

};

#endif

