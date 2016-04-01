#ifndef MANTID_KERNEL_HISTOGRAM_BINCOUNTS_TEST_H_
#define MANTID_KERNEL_HISTOGRAM_BINCOUNTS_TEST_H_

#include <cxxtest/TestSuite.h>

#include "MantidKernel/Histogram/BinCounts.h"

using namespace Mantid;
using namespace Mantid::Kernel;

class HistogramBinCountsTest : public CxxTest::TestSuite {
public:
  // This pair of boilerplate methods prevent the suite being created statically
  // This means the constructor isn't called when running other tests
  static HistogramBinCountsTest *createSuite() {
    return new HistogramBinCountsTest();
  }
  static void destroySuite(HistogramBinCountsTest *suite) { delete suite; }

  void test_construct_from_values_and_width() {
    const BinValues values = {0.1, 0.2};
    const BinWidths widths = {2.0, 3.0};
    BinCounts counts(values, widths);
    TS_ASSERT_EQUALS(counts.size(), 2);
    TS_ASSERT_DELTA(counts[0], 0.2, 1e-14);
    TS_ASSERT_DELTA(counts[1], 0.6, 1e-14);
  }
};

#endif /* MANTID_KERNEL_HISTOGRAM_BINCOUNTS_TEST_H_ */
