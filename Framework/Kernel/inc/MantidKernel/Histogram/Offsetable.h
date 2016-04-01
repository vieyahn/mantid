#ifndef MANTID_KERNEL_HISTOGRAM_OFFSETABLE_H_
#define MANTID_KERNEL_HISTOGRAM_OFFSETABLE_H_

#include "MantidKernel/DllConfig.h"

#include <algorithm>

namespace Mantid {
namespace Kernel {

template <class T> class MANTID_KERNEL_DLL Offsetable {
public:
  T &operator+=(const double offset) {
    auto &data = static_cast<T *>(this)->data();
    std::for_each(data.begin(), data.end(),
                  [=](double &value) { value += offset; });
    return *static_cast<T *>(this);
  }

protected:
  ~Offsetable() = default;
};

template <class T> inline T operator+(T lhs, const double rhs) {
  return static_cast<T>(lhs += rhs);
}

} // Namespace Kernel
} // Namespace Mantid

#endif /*MANTID_KERNEL_HISTOGRAM_OFFSETABLE_H_*/
