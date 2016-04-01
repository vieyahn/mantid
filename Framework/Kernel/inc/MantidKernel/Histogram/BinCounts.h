#ifndef MANTID_KERNEL_BINCOUNTS_H_
#define MANTID_KERNEL_BINCOUNTS_H_

#include "MantidKernel/Histogram/VectorOf.h"
#include "MantidKernel/Histogram/ConstIterable.h"
#include "MantidKernel/Histogram/Addable.h"
#include "MantidKernel/Histogram/Scalable.h"
#include "MantidKernel/Histogram/BinValues.h"
#include "MantidKernel/Histogram/BinWidths.h"

namespace Mantid {
namespace Kernel {

// Notes:
// - Not Offsetable: constant offset pointless for variable-size bins.
// - Not Multipliable: result would depend on bin size.
class MANTID_KERNEL_DLL BinCounts : public VectorOf<BinCounts>,
                                    public ConstIterable<BinCounts>,
                                    public Addable<BinCounts>,
                                    public Scalable<BinCounts> {
public:
  using VectorOf<BinCounts>::VectorOf;

  BinCounts(const BinValues &values, const BinWidths &widths) {
    auto &data = m_data.access();
    std::transform(cbegin(values), cend(values), cbegin(widths),
                   std::back_inserter(data), std::multiplies<double>());
  }
};

} // Namespace Kernel
} // Namespace Mantid

#endif /*MANTID_KERNEL_BINCOUNTS_H_*/
