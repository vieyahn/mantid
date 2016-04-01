#ifndef MANTID_KERNEL_HISTOGRAM_BINVARIANCES_H_
#define MANTID_KERNEL_HISTOGRAM_BINVARIANCES_H_

#include "MantidKernel/DllConfig.h"
#include "MantidKernel/Histogram/VectorOf.h"
#include "MantidKernel/Histogram/ConstIterable.h"
#include "MantidKernel/Histogram/Addable.h"
#include "MantidKernel/Histogram/Offsetable.h"
#include "MantidKernel/Histogram/Multipliable.h"
#include "MantidKernel/Histogram/Scalable.h"

namespace Mantid {
namespace Kernel {

class BinWidths;
class BinValues;
class BinStandardDeviations;

class MANTID_KERNEL_DLL BinVariances
    : public VectorOf<BinVariances>,
      public ConstIterable<BinVariances>,
      public Addable<BinVariances>,
      public Offsetable<BinVariances>, // is it? a constant does not have a bin
                                       // width dependence
      public Multipliable<BinVariances>,
      public Scalable<BinVariances> {
public:
  using VectorOf<BinVariances>::VectorOf;
  BinVariances(const BinStandardDeviations &sigmas);
  BinVariances(const BinValues &values, const BinWidths &widths);

  // Overloads defined in different base classed, need to have 'using'.
  using Addable<BinVariances>::operator+=;
  using Offsetable<BinVariances>::operator+=;
  using Multipliable<BinVariances>::operator*=;
  using Scalable<BinVariances>::operator*=;
};

} // Namespace Kernel
} // Namespace Mantid

#endif /*MANTID_KERNEL_HISTOGRAM_BINVARIANCES_H_*/
