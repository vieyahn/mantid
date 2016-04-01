#ifndef MANTID_KERNEL_HISTOGRAM_BINWIDTHS_TEST_H_
#define MANTID_KERNEL_HISTOGRAM_BINWIDTHS_TEST_H_

#include <cxxtest/TestSuite.h>

#include "MantidKernel/Histogram/BinWidths.h"

using namespace Mantid;
using namespace Mantid::Kernel;

class HistogramBinWidthsTest : public CxxTest::TestSuite {
public:
  // This pair of boilerplate methods prevent the suite being created statically
  // This means the constructor isn't called when running other tests
  static HistogramBinWidthsTest *createSuite() {
    return new HistogramBinWidthsTest();
  }
  static void destroySuite(HistogramBinWidthsTest *suite) { delete suite; }

  void test_construct_from_bin_edges() {
    BinEdges edges = { 0.1, 0.2, 0.4, 0.7};
    TS_ASSERT_EQUALS(edges.size(), 4);
    BinWidths widths(edges);
    TS_ASSERT_EQUALS(widths.size(), 3);
    TS_ASSERT_DELTA(widths[0], 0.1, 1e-14);
    TS_ASSERT_DELTA(widths[1], 0.2, 1e-14);
    TS_ASSERT_DELTA(widths[2], 0.3, 1e-14);
  }
};

#endif /* MANTID_KERNEL_HISTOGRAM_BINWIDTHS_TEST_H_ */
