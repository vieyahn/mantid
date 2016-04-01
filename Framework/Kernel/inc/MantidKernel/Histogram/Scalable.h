#ifndef MANTID_KERNEL_HISTOGRAM_SCALABLE_H_
#define MANTID_KERNEL_HISTOGRAM_SCALABLE_H_

#include "MantidKernel/DllConfig.h"

#include <algorithm>

namespace Mantid {
namespace Kernel {

template <class T> class MANTID_KERNEL_DLL Scalable {
public:
  T &operator*=(const double scale) {
    auto &data = static_cast<T *>(this)->data();
    std::for_each(data.begin(), data.end(),
                  [=](double &value) { value *= scale; });
    return *static_cast<T *>(this);
  }

protected:
  ~Scalable() = default;
};
template <class T> inline T operator*(T lhs, const double rhs) {
  return static_cast<T>(lhs *= rhs);
}

} // Namespace Kernel
} // Namespace Mantid

#endif /*MANTID_KERNEL_HISTOGRAM_SCALABLE_H_*/
