#ifndef MANTID_KERNEL_HISTOGRAM_BINSTANDARDDEVIATIONS_H_
#define MANTID_KERNEL_HISTOGRAM_BINSTANDARDDEVIATIONS_H_

#include "MantidKernel/DllConfig.h"
#include "MantidKernel/Histogram/VectorOf.h"
#include "MantidKernel/Histogram/ConstIterable.h"

namespace Mantid {
namespace Kernel {

class BinVariances;

class MANTID_KERNEL_DLL BinStandardDeviations
    : public VectorOf<BinStandardDeviations>,
      public ConstIterable<BinStandardDeviations> {
public:
  using VectorOf<BinStandardDeviations>::VectorOf;

  BinStandardDeviations(const BinVariances &variances);
};

} // Namespace Kernel
} // Namespace Mantid

#endif /*MANTID_KERNEL_HISTOGRAM_BINSTANDARDDEVIATIONS_H_*/
