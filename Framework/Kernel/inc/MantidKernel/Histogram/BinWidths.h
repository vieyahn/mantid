#ifndef MANTID_KERNEL_HISTOGRAM_BINWIDTHS_H_
#define MANTID_KERNEL_HISTOGRAM_BINWIDTHS_H_

#include "MantidKernel/DllConfig.h"
#include "MantidKernel/Histogram/BinEdges.h"
#include "MantidKernel/Histogram/VectorOf.h"
#include "MantidKernel/Histogram/ConstIterable.h"

#include <vector>

namespace Mantid {
namespace Kernel {

class MANTID_KERNEL_DLL BinWidths : public VectorOf<BinWidths>, public ConstIterable<BinWidths> {
public:
  using VectorOf<BinWidths>::VectorOf;

  BinWidths(const BinEdges &edges) {
    m_data = make_cow<std::vector<double>>();
    if (edges.size() < 2)
      return;
    auto &data = m_data.access();
    // Note: We cannot use std::adjacent_difference (without an extra copy)
    // since we do not want the extra first element.
    data.reserve(edges.size() - 1);
    for (auto it = cbegin(edges) + 1; it < cend(edges); ++it) {
      data.emplace_back(*it - *(it - 1));
    }
  }
};

} // Namespace Kernel
} // Namespace Mantid

#endif /*MANTID_KERNEL_HISTOGRAM_BINWIDTHS_H_*/
