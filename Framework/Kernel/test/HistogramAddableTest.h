#ifndef MANTID_KERNEL_HISTOGRAM_ADDABLE_TEST_H_
#define MANTID_KERNEL_HISTOGRAM_ADDABLE_TEST_H_

#include <cxxtest/TestSuite.h>

#include "MantidKernel/Histogram/VectorOf.h"
#include "MantidKernel/Histogram/ConstIterable.h"
#include "MantidKernel/Histogram/Addable.h"

using namespace Mantid;
using namespace Mantid::Kernel;

class AddableTester : public VectorOf<AddableTester>,
                      public ConstIterable<AddableTester>,
                      public Addable<AddableTester> {
  using VectorOf<AddableTester>::VectorOf;
};

class HistogramAddableTest : public CxxTest::TestSuite {
public:
  // This pair of boilerplate methods prevent the suite being created statically
  // This means the constructor isn't called when running other tests
  static HistogramAddableTest *createSuite() {
    return new HistogramAddableTest();
  }
  static void destroySuite(HistogramAddableTest *suite) { delete suite; }

  void test_plus_equals() {
    AddableTester lhs{0.1, 0.2};
    const AddableTester rhs{0.2, 0.3};
    lhs += rhs;
    TS_ASSERT_EQUALS(lhs.size(), 2);
    TS_ASSERT_DELTA(lhs[0], 0.3, 1e-14);
    TS_ASSERT_DELTA(lhs[1], 0.5, 1e-14);
  }

  void test_plus() {
    const AddableTester rhs1{0.1, 0.2};
    const AddableTester rhs2{0.2, 0.3};
    const AddableTester lhs(rhs1 + rhs2);
    TS_ASSERT_EQUALS(lhs.size(), 2);
    TS_ASSERT_DELTA(lhs[0], 0.3, 1e-14);
    TS_ASSERT_DELTA(lhs[1], 0.5, 1e-14);
  }
};

#endif /* MANTID_KERNEL_HISTOGRAM_ADDABLE_TEST_H_ */
