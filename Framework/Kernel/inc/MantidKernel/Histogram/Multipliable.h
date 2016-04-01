#ifndef MANTID_KERNEL_HISTOGRAM_MULTIPLIABLE_H_
#define MANTID_KERNEL_HISTOGRAM_MULTIPLIABLE_H_

#include "MantidKernel/DllConfig.h"

#include <algorithm>

namespace Mantid {
namespace Kernel {

template <class T> class MANTID_KERNEL_DLL Multipliable {
public:
  T &operator*=(const T &rhs) {
    auto &data = static_cast<T *>(this)->data();
    std::transform(data.begin(), data.end(), cbegin(rhs), data.begin(),
                   std::multiplies<double>());
    return *static_cast<T *>(this);
  }

protected:
  ~Multipliable() = default;
};

template <class T> inline T operator*(T lhs, const T &rhs) {
  return static_cast<T>(lhs *= rhs);
}

} // Namespace Kernel
} // Namespace Mantid

#endif /*MANTID_KERNEL_HISTOGRAM_MULTIPLIABLE_H_*/
