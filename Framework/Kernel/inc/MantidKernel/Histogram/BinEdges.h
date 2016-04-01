#ifndef MANTID_KERNEL_HISTOGRAM_BINEDGES_H_
#define MANTID_KERNEL_HISTOGRAM_BINEDGES_H_

#include "MantidKernel/DllConfig.h"
#include "MantidKernel/Histogram/VectorOf.h"
#include "MantidKernel/Histogram/ConstIterable.h"
#include "MantidKernel/Histogram/Offsetable.h"
#include "MantidKernel/Histogram/Scalable.h"

namespace Mantid {
namespace Kernel {

class MANTID_KERNEL_DLL BinEdges : public VectorOf<BinEdges>,
                 public ConstIterable<BinEdges>,
                 public Offsetable<BinEdges>,
                 public Scalable<BinEdges> {
public:
  using VectorOf<BinEdges>::VectorOf;
};

} // Namespace Kernel
} // Namespace Mantid

#endif /*MANTID_KERNEL_HISTOGRAM_BINEDGES_H_*/
