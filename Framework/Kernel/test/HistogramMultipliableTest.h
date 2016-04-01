#ifndef MANTID_KERNEL_HISTOGRAM_MULTIPLIABLE_TEST_H_
#define MANTID_KERNEL_HISTOGRAM_MULTIPLIABLE_TEST_H_

#include <cxxtest/TestSuite.h>

#include "MantidKernel/Histogram/VectorOf.h"
#include "MantidKernel/Histogram/ConstIterable.h"
#include "MantidKernel/Histogram/Multipliable.h"

using namespace Mantid;
using namespace Mantid::Kernel;

class MultipliableTester : public VectorOf<MultipliableTester>,
                           public ConstIterable<MultipliableTester>,
                           public Multipliable<MultipliableTester> {
  using VectorOf<MultipliableTester>::VectorOf;
};

class HistogramMultipliableTest : public CxxTest::TestSuite {
public:
  // This pair of boilerplate methods prevent the suite being created statically
  // This means the constructor isn't called when running other tests
  static HistogramMultipliableTest *createSuite() {
    return new HistogramMultipliableTest();
  }
  static void destroySuite(HistogramMultipliableTest *suite) { delete suite; }

  void test_times_equals() {
    MultipliableTester lhs{0.1, 0.2};
    const MultipliableTester rhs{0.2, 0.3};
    lhs *= rhs;
    TS_ASSERT_EQUALS(lhs.size(), 2);
    TS_ASSERT_DELTA(lhs[0], 0.02, 1e-14);
    TS_ASSERT_DELTA(lhs[1], 0.06, 1e-14);
  }

  void test_times() {
    const MultipliableTester rhs1{0.1, 0.2};
    const MultipliableTester rhs2{0.2, 0.3};
    const MultipliableTester lhs(rhs1 * rhs2);
    TS_ASSERT_EQUALS(lhs.size(), 2);
    TS_ASSERT_DELTA(lhs[0], 0.02, 1e-14);
    TS_ASSERT_DELTA(lhs[1], 0.06, 1e-14);
  }
};

#endif /* MANTID_KERNEL_HISTOGRAM_MULTIPLIABLE_TEST_H_ */
