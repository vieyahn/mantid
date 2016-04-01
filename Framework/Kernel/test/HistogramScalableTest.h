#ifndef MANTID_KERNEL_HISTOGRAM_SCALABLE_TEST_H_
#define MANTID_KERNEL_HISTOGRAM_SCALABLE_TEST_H_

#include <cxxtest/TestSuite.h>

#include "MantidKernel/Histogram/VectorOf.h"
#include "MantidKernel/Histogram/ConstIterable.h"
#include "MantidKernel/Histogram/Scalable.h"

using namespace Mantid;
using namespace Mantid::Kernel;

class ScalableTester : public VectorOf<ScalableTester>,
                       public ConstIterable<ScalableTester>,
                       public Scalable<ScalableTester> {
  using VectorOf<ScalableTester>::VectorOf;
};

class HistogramScalableTest : public CxxTest::TestSuite {
public:
  // This pair of boilerplate methods prevent the suite being created statically
  // This means the constructor isn't called when running other tests
  static HistogramScalableTest *createSuite() {
    return new HistogramScalableTest();
  }
  static void destroySuite(HistogramScalableTest *suite) { delete suite; }

  void test_times_equals() {
    ScalableTester lhs{0.1, 0.2};
    const double rhs = 3.0;
    lhs *= rhs;
    TS_ASSERT_EQUALS(lhs.size(), 2);
    TS_ASSERT_DELTA(lhs[0], 0.3, 1e-14);
    TS_ASSERT_DELTA(lhs[1], 0.6, 1e-14);
  }

  void test_times() {
    const ScalableTester rhs1{0.1, 0.2};
    const double rhs2 = 3.0;
    const ScalableTester lhs(rhs1 * rhs2);
    TS_ASSERT_EQUALS(lhs.size(), 2);
    TS_ASSERT_DELTA(lhs[0], 0.3, 1e-14);
    TS_ASSERT_DELTA(lhs[1], 0.6, 1e-14);
  }
};

#endif /* MANTID_KERNEL_HISTOGRAM_SCALABLE_TEST_H_ */
