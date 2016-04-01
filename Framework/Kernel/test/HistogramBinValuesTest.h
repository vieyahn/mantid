#ifndef MANTID_KERNEL_HISTOGRAM_BINVALUES_TEST_H_
#define MANTID_KERNEL_HISTOGRAM_BINVALUES_TEST_H_

#include <cxxtest/TestSuite.h>

#include "MantidKernel/Histogram/BinValues.h"

using namespace Mantid;
using namespace Mantid::Kernel;

class HistogramBinValuesTest : public CxxTest::TestSuite {
public:
  // This pair of boilerplate methods prevent the suite being created statically
  // This means the constructor isn't called when running other tests
  static HistogramBinValuesTest *createSuite() {
    return new HistogramBinValuesTest();
  }
  static void destroySuite(HistogramBinValuesTest *suite) { delete suite; }

  void test_construction_from_bin_counts() {
    // add test once constructor is implemented.
  }
};

#endif /* MANTID_KERNEL_HISTOGRAM_BINVALUES_TEST_H_ */
