#ifndef MANTID_KERNEL_HISTOGRAM_BINVALUES_H_
#define MANTID_KERNEL_HISTOGRAM_BINVALUES_H_

#include "MantidKernel/DllConfig.h"
#include "MantidKernel/Histogram/VectorOf.h"
#include "MantidKernel/Histogram/ConstIterable.h"
#include "MantidKernel/Histogram/Addable.h"
#include "MantidKernel/Histogram/Offsetable.h"
#include "MantidKernel/Histogram/Multipliable.h"
#include "MantidKernel/Histogram/Scalable.h"

namespace Mantid {
namespace Kernel {
class MANTID_KERNEL_DLL BinValues : public VectorOf<BinValues>,
                                    public ConstIterable<BinValues>,
                                    public Addable<BinValues>,
                                    public Offsetable<BinValues>,
                                    public Multipliable<BinValues>,
                                    public Scalable<BinValues> {
public:
  using VectorOf<BinValues>::VectorOf;
  // Overloads defined in different base classed, need to have 'using'.
  using Addable<BinValues>::operator+=;
  using Offsetable<BinValues>::operator+=;
  using Multipliable<BinValues>::operator*=;
  using Scalable<BinValues>::operator*=;
};

} // Namespace Kernel
} // Namespace Mantid

#endif /*MANTID_KERNEL_HISTOGRAM_BINVALUES_H_*/
