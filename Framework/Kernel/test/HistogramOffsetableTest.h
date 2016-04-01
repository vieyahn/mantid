#ifndef MANTID_KERNEL_HISTOGRAM_OFFSETABLE_TEST_H_
#define MANTID_KERNEL_HISTOGRAM_OFFSETABLE_TEST_H_

#include <cxxtest/TestSuite.h>

#include "MantidKernel/Histogram/VectorOf.h"
#include "MantidKernel/Histogram/ConstIterable.h"
#include "MantidKernel/Histogram/Offsetable.h"

using namespace Mantid;
using namespace Mantid::Kernel;

class OffsetableTester : public VectorOf<OffsetableTester>,
                         public ConstIterable<OffsetableTester>,
                         public Offsetable<OffsetableTester> {
  using VectorOf<OffsetableTester>::VectorOf;
};

class HistogramOffsetableTest : public CxxTest::TestSuite {
public:
  // This pair of boilerplate methods prevent the suite being created statically
  // This means the constructor isn't called when running other tests
  static HistogramOffsetableTest *createSuite() {
    return new HistogramOffsetableTest();
  }
  static void destroySuite(HistogramOffsetableTest *suite) { delete suite; }

  void test_plus_equals() {
    OffsetableTester lhs{0.1, 0.2};
    const double rhs = 0.01;
    lhs += rhs;
    TS_ASSERT_EQUALS(lhs.size(), 2);
    TS_ASSERT_DELTA(lhs[0], 0.11, 1e-14);
    TS_ASSERT_DELTA(lhs[1], 0.21, 1e-14);
  }

  void test_plus() {
    const OffsetableTester rhs1{0.1, 0.2};
    const double rhs2 = 0.01;
    const OffsetableTester lhs(rhs1 + rhs2);
    TS_ASSERT_EQUALS(lhs.size(), 2);
    TS_ASSERT_DELTA(lhs[0], 0.11, 1e-14);
    TS_ASSERT_DELTA(lhs[1], 0.21, 1e-14);
  }
};

#endif /* MANTID_KERNEL_HISTOGRAM_OFFSETABLE_TEST_H_ */
