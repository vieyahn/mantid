#ifndef MANTID_KERNEL_HISTOGRAM_ADDABLE_H_
#define MANTID_KERNEL_HISTOGRAM_ADDABLE_H_

#include "MantidKernel/DllConfig.h"

#include <algorithm>
#include <iterator>

namespace Mantid {
namespace Kernel {

template <class T> class MANTID_KERNEL_DLL Addable {
public:
  T &operator+=(const T &rhs) {
    auto &data = static_cast<T *>(this)->data();
    std::transform(data.begin(), data.end(), cbegin(rhs), data.begin(),
                   std::plus<double>());
    return *static_cast<T *>(this);
  }

protected:
  ~Addable() = default;
};

template <class T> inline T operator+(T lhs, const T &rhs) {
  return static_cast<T>(lhs += rhs);
}

} // Namespace Kernel
} // Namespace Mantid

#endif /*MANTID_KERNEL_HISTOGRAM_ADDABLE_H_*/
