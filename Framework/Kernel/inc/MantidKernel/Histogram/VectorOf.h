#ifndef MANTID_KERNEL_HISTOGRAM_VECTOROF_H_
#define MANTID_KERNEL_HISTOGRAM_VECTOROF_H_

#include "MantidKernel/DllConfig.h"
#include "MantidKernel/cow_ptr.h"
#include "MantidKernel/make_cow.h"

#include <vector>

namespace Mantid {
namespace Kernel {

template <class T> class MANTID_KERNEL_DLL VectorOf {
public:
  explicit VectorOf() : m_data() {}
  VectorOf(size_t count, const double &value) {
    m_data = make_cow<std::vector<double>>(count, value);
  }
  explicit VectorOf(size_t count) {
    m_data = make_cow<std::vector<double>>(count);
  }
  VectorOf(std::initializer_list<double> init) {
    m_data = make_cow<std::vector<double>>(init);
  }
  VectorOf(const VectorOf &other) = default;
  VectorOf(VectorOf &&other) = default;
  VectorOf &operator=(const VectorOf &other) = default;
  VectorOf &operator=(VectorOf &&other) = default;

  VectorOf &operator=(std::initializer_list<double> ilist) {
    m_data.access().operator=(ilist);
    return *this;
  }

  // TODO figure out if we want all these overloads.
  VectorOf(const cow_ptr<std::vector<double>> &other) { m_data = other; }
  VectorOf(const boost::shared_ptr<std::vector<double>> &other) {
    m_data = other;
  }
  // TODO cow_ptr is not movable, can we implement move?
  VectorOf(const std::vector<double> &data) {
    m_data = make_cow<std::vector<double>>(data);
  }
  // VectorOf(std::vector<double> &&data) { m_data =
  // make_cow<std::vector<double>>(std::move(data)); }

  explicit operator bool() const { return m_data.operator->() != nullptr; }

  size_t size() const { return m_data->size(); }
  void resize(size_t size) { m_data.access().resize(size); }

  // Note that this function returns the internal data of VectorOf, i.e., does
  // not forward to std::vector::data().
  const std::vector<double> &data() const { return *m_data; }
  const std::vector<double> &constData() const { return *m_data; }
  std::vector<double> &data() { return m_data.access(); }

protected:
  // This is used as base class only, cannot delete polymorphically, so
  // destructor is protected.
  ~VectorOf() = default;

  cow_ptr<std::vector<double>> m_data;
};

// are things like this useful?
// BinCounts operator*(const BinValues &values, const BinWidths &widths) {
//    BinCounts counts;
//    const auto &value = values.data();
//    const auto &width = widths.data();
//    auto &count = counts.data();
//    std::transform(value.begin(), value.end(), width.begin(),
//    std::back_inserter(count), std::multiplies<double>());
//    return counts;
//}

} // Namespace Kernel
} // Namespace Mantid

#endif /*MANTID_KERNEL_HISTOGRAM_VECTOROF_H_*/
