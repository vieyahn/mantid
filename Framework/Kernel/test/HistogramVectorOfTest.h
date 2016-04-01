#ifndef MANTID_KERNEL_HISTOGRAM_VECTOROF_TEST_H_
#define MANTID_KERNEL_HISTOGRAM_VECTOROF_TEST_H_

#include <cxxtest/TestSuite.h>

#include "MantidKernel/Histogram/VectorOf.h"
#include "MantidKernel/Histogram/ConstIterable.h"

using namespace Mantid;
using namespace Mantid::Kernel;

class VectorOfTester : public VectorOf<VectorOfTester>,
                       public ConstIterable<VectorOfTester> {
  using VectorOf<VectorOfTester>::VectorOf;
};

class HistogramVectorOfTest : public CxxTest::TestSuite {
public:
  void test_empty_constructor() {
    const VectorOfTester values;
    TS_ASSERT_EQUALS(values.size(), 0);
  }

  void test_count_value_constructor() {
    const VectorOfTester values(2, 0.1);
    TS_ASSERT_EQUALS(values.size(), 2);
    TS_ASSERT_EQUALS(values[0], 0.1);
    TS_ASSERT_EQUALS(values[1], 0.1);
  }

  void test_count_constructor() {
    const VectorOfTester values(2);
    TS_ASSERT_EQUALS(values.size(), 2);
    TS_ASSERT_EQUALS(values[0], 0.0);
    TS_ASSERT_EQUALS(values[1], 0.0);
  }

  void test_initializer_list_constructor() {
    const VectorOfTester values{0.1, 0.2, 0.3};
    TS_ASSERT_EQUALS(values.size(), 3);
    TS_ASSERT_EQUALS(values[0], 0.1);
    TS_ASSERT_EQUALS(values[1], 0.2);
    TS_ASSERT_EQUALS(values[2], 0.3);
  }

  void test_copy_constructor() {
    const VectorOfTester src(2, 0.1);
    const VectorOfTester dest(src);
    TS_ASSERT_EQUALS(dest[0], 0.1);
    TS_ASSERT_EQUALS(dest[1], 0.1);
  }

  void test_move_constructor() {
    // boost::shared_ptr<std::vector<double>> src3;
    // src3->resize(2);
    // TS_ASSERT_EQUALS(src3->size(), 2);
    // boost::shared_ptr<std::vector<double>> dest3(std::move(src3));
    // TS_ASSERT_EQUALS(src3->size(), 0);
    // TS_ASSERT_EQUALS(dest3->size(), 2);

    // Kernel::cow_ptr<std::vector<double>> src2;
    // src2.access().resize(2);
    // TS_ASSERT_EQUALS(src2->size(), 2);
    // Kernel::cow_ptr<std::vector<double>> dest2 =
    // Kernel::cow_ptr<std::vector<double>>(std::move(src2));
    // printf("%p\n",src2.operator->());
    // printf("%p\n",dest2.operator->());
    // TS_ASSERT_EQUALS(src2.operator->(), nullptr);
    // TS_ASSERT_EQUALS(dest2->size(), 2);

    VectorOfTester src(2, 0.1);
    TS_ASSERT_EQUALS(src.size(), 2);
    TS_ASSERT(src);
    const VectorOfTester dest(std::move(src));
    TS_ASSERT(!src);
    TS_ASSERT_EQUALS(dest[0], 0.1);
    TS_ASSERT_EQUALS(dest[1], 0.1);
  }

  void test_copy_assignment() {
    const VectorOfTester src(2, 0.1);
    VectorOfTester dest(1);
    TS_ASSERT_EQUALS(dest.size(), 1);
    TS_ASSERT_EQUALS(dest[0], 0.0);
    dest = src;
    TS_ASSERT_EQUALS(dest.size(), 2);
    TS_ASSERT_EQUALS(dest[0], 0.1);
    TS_ASSERT_EQUALS(dest[1], 0.1);
  }

  void test_move_assignment() {
    VectorOfTester src(2, 0.1);
    VectorOfTester dest(1);
    TS_ASSERT_EQUALS(dest.size(), 1);
    TS_ASSERT_EQUALS(dest[0], 0.0);
    TS_ASSERT(src);
    dest = std::move(src);
    TS_ASSERT(!src);
    TS_ASSERT_EQUALS(dest[0], 0.1);
    TS_ASSERT_EQUALS(dest[1], 0.1);
  }

  void test_initializer_list_assignment() {
    VectorOfTester values(2, 0.1);
    TS_ASSERT_EQUALS(values.size(), 2);
    TS_ASSERT_EQUALS(values[0], 0.1);
    TS_ASSERT_EQUALS(values[1], 0.1);
    values = {0.1, 0.2, 0.3};
    TS_ASSERT_EQUALS(values.size(), 3);
    TS_ASSERT_EQUALS(values[0], 0.1);
    TS_ASSERT_EQUALS(values[1], 0.2);
    TS_ASSERT_EQUALS(values[2], 0.3);
  }

  void test_size() {
    const VectorOfTester values(42);
    TS_ASSERT_EQUALS(values.size(), 42);
  }

  void test_data() {
    const VectorOfTester values(2, 0.1);
    const auto &data = values.data();
    TS_ASSERT_EQUALS(data.size(), 2);
    TS_ASSERT_EQUALS(data[0], 0.1);
    TS_ASSERT_EQUALS(data[1], 0.1);
  }

  // void test_assignment() {
  //  BinEdges edges = {0.1, 1.1, 2.2};
  //  // assignment call
  //  auto copy = edges;
  //  // overwrite input vector so we can verify that it is a deep copy
  //  edges = {0.0, 0.0, 0.0};
  //  TS_ASSERT_EQUALS(copy[0], 0.1);
  //  TS_ASSERT_EQUALS(copy[1], 1.1);
  //  TS_ASSERT_EQUALS(copy[2], 2.2);
  //}
};

#endif /* MANTID_KERNEL_HISTOGRAM_VECTOROF_TEST_H_ */
