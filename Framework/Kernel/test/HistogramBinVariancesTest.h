#ifndef MANTID_KERNEL_HISTOGRAM_BINVARIANCES_TEST_H_
#define MANTID_KERNEL_HISTOGRAM_BINVARIANCES_TEST_H_

#include <cxxtest/TestSuite.h>

#include "MantidKernel/Histogram/BinVariances.h"
#include "MantidKernel/Histogram/BinStandardDeviations.h"
#include "MantidKernel/Histogram/BinValues.h"
#include "MantidKernel/Histogram/BinWidths.h"

using namespace Mantid;
using namespace Mantid::Kernel;

class HistogramBinVariancesTest : public CxxTest::TestSuite {
public:
  // This pair of boilerplate methods prevent the suite being created statically
  // This means the constructor isn't called when running other tests
  static HistogramBinVariancesTest *createSuite() {
    return new HistogramBinVariancesTest();
  }
  static void destroySuite(HistogramBinVariancesTest *suite) { delete suite; }

  void test_construct_from_bin_standard_deviation() {
    const BinStandardDeviations sigmas = {0.1, 0.2};
    BinVariances variances(sigmas);
    TS_ASSERT_EQUALS(variances.size(), 2);
    TS_ASSERT_DELTA(variances[0], 0.01, 1e-14);
    TS_ASSERT_DELTA(variances[1], 0.04, 1e-14);
  }

  void test_construct_from_values_and_widths() {
    const BinValues values = {0.1, 0.3};
    const BinWidths widths = {2.0, 3.0};
    BinVariances variances(values, widths);
    TS_ASSERT_EQUALS(variances.size(), 2);
    TS_ASSERT_DELTA(variances[0], 0.05, 1e-14);
    TS_ASSERT_DELTA(variances[1], 0.1, 1e-14);
  }
};

#endif /* MANTID_KERNEL_HISTOGRAM_BINVARIANCES_TEST_H_ */
