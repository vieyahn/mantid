#include "MantidKernel/Histogram/BinVariances.h"
#include "MantidKernel/Histogram/BinValues.h"
#include "MantidKernel/Histogram/BinWidths.h"
#include "MantidKernel/Histogram/BinStandardDeviations.h"

using namespace Mantid;
using namespace Kernel;

BinVariances::BinVariances(const BinStandardDeviations &sigmas) {
  m_data = make_cow<std::vector<double>>();
  auto &data = m_data.access();
  std::transform(cbegin(sigmas), cend(sigmas), std::back_inserter(data),
                 [](double sigma) { return sigma * sigma; });
}
BinVariances::BinVariances(const BinValues &values, const BinWidths &widths) {
  auto &data = m_data.access();
  std::transform(cbegin(values), cend(values), cbegin(widths),
                 std::back_inserter(data), std::divides<double>());
}
