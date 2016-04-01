#include "MantidKernel/Histogram/BinStandardDeviations.h"
#include "MantidKernel/Histogram/BinVariances.h"

using namespace Mantid;
using namespace Kernel;

BinStandardDeviations::BinStandardDeviations(const BinVariances &variances) {
  m_data = make_cow<std::vector<double>>();
  auto &data = m_data.access();
  std::transform(cbegin(variances), cend(variances), std::back_inserter(data),
                 static_cast<double (*)(double)>(sqrt));
}
