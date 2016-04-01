#ifndef MANTID_KERNEL_HISTOGRAM_TEST_H_
#define MANTID_KERNEL_HISTOGRAM_TEST_H_

#include <cxxtest/TestSuite.h>

#include "MantidKernel/Histogram/Histogram.h"

using namespace Mantid;
using namespace Mantid::Kernel;

class HistogramTest : public CxxTest::TestSuite {
public:
  // This pair of boilerplate methods prevent the suite being created statically
  // This means the constructor isn't called when running other tests
  static HistogramTest *createSuite() {
    return new HistogramTest();
  }
  static void destroySuite(HistogramTest *suite) { delete suite; }

  void test_constructor() { TS_ASSERT_THROWS_NOTHING(Histogram{}); }

  void test_assignment() {
    Histogram histogram;
    // assignment call
    auto copy = histogram;
  }

  void test_edges() {
    Histogram hist;
    hist.resize(1);
    auto &edges = hist.edges();
    TS_ASSERT_EQUALS(edges.size(), 2);
    // Default constructed doubles, should be 0.0.
    TS_ASSERT_EQUALS(edges[0], 0.0);
    TS_ASSERT_EQUALS(edges[1], 0.0);
    edges.data() = {0.1, 0.2};
    TS_ASSERT_EQUALS(edges[0], 0.1);
    TS_ASSERT_EQUALS(edges[1], 0.2);
  }

  void test_edges_copy_on_write() {
    Histogram hist;
    hist.resize(1);
    auto copy = hist;
    TS_ASSERT_EQUALS(&copy.edges().constData(), &hist.edges().constData());
    auto &edges = copy.edges();
    TS_ASSERT_EQUALS(edges.size(), 2);
    edges.data() = {0.1, 0.2};
    TS_ASSERT_EQUALS(edges[0], 0.1);
    TS_ASSERT_EQUALS(edges[1], 0.2);
    TS_ASSERT_DIFFERS(&copy.edges().constData(), &hist.edges().constData());
    TS_ASSERT_EQUALS(hist.edges()[0], 0.0);
    TS_ASSERT_EQUALS(hist.edges()[1], 0.0);
  }

  void test_size() {
    Histogram hist;
    TS_ASSERT_EQUALS(hist.size(), 0);
    hist.resize(3);
    TS_ASSERT_EQUALS(hist.size(), 3);
    TS_ASSERT_EQUALS(hist.edges().size(), 4);
    TS_ASSERT_EQUALS(hist.values().size(), 3);
  }

  void test_implicitErrors() {
    Histogram hist;
    TS_ASSERT(hist.implicitErrors());
    TS_ASSERT(!hist.explicitErrors());
  }

  void test_implicitErrors_values() {
    Histogram hist;
    hist.resize(1);
    hist.edges().data() = {0.1, 0.2};
    hist.values().data() = {1.2};
    TS_ASSERT_DELTA(hist.variances()[0], 12.0, 1e-14);
  }
};

#endif /* MANTID_KERNEL_HISTOGRAM_TEST_H_ */
