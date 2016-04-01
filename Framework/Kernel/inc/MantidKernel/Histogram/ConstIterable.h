#ifndef MANTID_KERNEL_HISTOGRAM_CONSTITERABLE_H_
#define MANTID_KERNEL_HISTOGRAM_CONSTITERABLE_H_

#include "MantidKernel/DllConfig.h"

namespace Mantid {
namespace Kernel {

template <class T> class MANTID_KERNEL_DLL ConstIterable {
public:
  const double &operator[](size_t pos) const {
    return static_cast<const T *>(this)->constData()[pos];
  }

protected:
  ~ConstIterable() = default;
};

template <class T>
auto cbegin(const ConstIterable<T> &container)
    -> decltype(static_cast<const T *>(&container)->data().cbegin()) {
  return static_cast<const T *>(&container)->data().cbegin();
}

template <class T>
auto cend(const ConstIterable<T> &container)
    -> decltype(static_cast<const T *>(&container)->data().cend()) {
  return static_cast<const T *>(&container)->data().cend();
}

} // Namespace Kernel
} // Namespace Mantid

#endif /*MANTID_KERNEL_HISTOGRAM_CONSTITERABLE_H_*/
