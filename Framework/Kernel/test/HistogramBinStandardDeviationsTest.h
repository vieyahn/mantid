#ifndef MANTID_KERNEL_HISTOGRAM_BINSTANDARDDEVIATIONS_TEST_H_
#define MANTID_KERNEL_HISTOGRAM_BINSTANDARDDEVIATIONS_TEST_H_

#include <cxxtest/TestSuite.h>

#include "MantidKernel/Histogram/BinStandardDeviations.h"
#include "MantidKernel/Histogram/BinVariances.h"

using namespace Mantid;
using namespace Mantid::Kernel;

class HistogramBinStandardDeviationsTest : public CxxTest::TestSuite {
public:
  // This pair of boilerplate methods prevent the suite being created statically
  // This means the constructor isn't called when running other tests
  static HistogramBinStandardDeviationsTest *createSuite() {
    return new HistogramBinStandardDeviationsTest();
  }
  static void destroySuite(HistogramBinStandardDeviationsTest *suite) {
    delete suite;
  }

  void test_construct_from_bin_variances() {
    const BinVariances variances = {4.0, 9.0};
    BinStandardDeviations sigmas(variances);
    TS_ASSERT_EQUALS(sigmas.size(), 2);
    TS_ASSERT_DELTA(sigmas[0], 2.0, 1e-14);
    TS_ASSERT_DELTA(sigmas[1], 3.0, 1e-14);
  }
};

#endif /* MANTID_KERNEL_HISTOGRAM_BINSTANDARDDEVIATIONS_TEST_H_ */
