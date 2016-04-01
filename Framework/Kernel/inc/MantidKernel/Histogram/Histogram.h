#ifndef MANTID_KERNEL_HISTOGRAM_H_
#define MANTID_KERNEL_HISTOGRAM_H_

#include "MantidKernel/DllConfig.h"
#include "MantidKernel/Histogram/VectorOf.h"
#include "MantidKernel/Histogram/BinCounts.h"
#include "MantidKernel/Histogram/BinValues.h"
#include "MantidKernel/Histogram/BinVariances.h"
#include "MantidKernel/Histogram/BinStandardDeviations.h"

#include <boost/optional.hpp>

#include <iterator>
#include <vector>

namespace Mantid {
namespace Kernel {

class MANTID_KERNEL_DLL Histogram {
private:
  BinEdges m_edges;
  BinValues m_values;
  boost::optional<BinVariances> m_variances;
  boost::optional<BinStandardDeviations> m_standardDeviations;

  // Implicit: errors is sqrt of counts (or rather, for distribution data, error
  // is sqrt of counts/width)
  // enum class ErrorMode { Implicit, Variance };
  enum class ErrorMode { Implicit, Variance, StandardDeviation };
  // When someone calls dataE, we transform into ErrorMode::StandardDeviation,
  // transform back on next call that requires Variance.
  // (can we swap memory of underlying vector, to avoid double allocation?)
  // TODO do we really need this? only to provide legacy interface? yes, used in
  // many places, need step-by-step change to new interface. Remove this
  // eventually.
  ErrorMode m_errorMode = ErrorMode::Implicit;

public:
  const BinEdges &edges() const { return m_edges; }
  BinEdges &edges() {
    return const_cast<BinEdges &>(
        static_cast<const Histogram *>(this)->edges());
  }
  const BinValues &values() const { return m_values; }
  BinValues &values() {
    return const_cast<BinValues &>(
        static_cast<const Histogram *>(this)->values());
  }

  BinVariances variances() const {
    if (implicitErrors())
      return {m_values, getWidths()};
    if (m_variances)
      return m_variances.get();
    // Fall back to construction from standard deviations.
    return {m_standardDeviations.get()};
  }

  // This function is meant only for transition period and should be removed
  // asap.
  BinVariances &variancesRef();
  BinStandardDeviations standardDeviations() const {
    if (implicitErrors())
      return {BinVariances(m_values, getWidths())};
    if (m_standardDeviations)
      return m_standardDeviations.get();
    // Fall back to construction from variances.
    return {m_variances.get()};
  }
  // This function is meant only for transition period and should be removed
  // asap.
  BinStandardDeviations &standardDeviationsRef();

  // Rename all these to 'makeXXX'?
  BinWidths getWidths() const { return {m_edges}; }

  BinCounts getCounts() const {
    return {m_values, getWidths()};
  };
  // void setBinCounts(const BinCounts &binCounts);
  // void addBinCounts(const BinCounts &binCounts);

  // void zero() {
  //  m_binValues->assign(size(), 0.0);
  //  m_binErrorsSquare->.assign(size(), 0.0);
  //}

  bool implicitErrors() const { return m_errorMode == ErrorMode::Implicit; }
  bool explicitErrors() const { return !implicitErrors(); }

  Histogram &operator+=(const Histogram &rhs) {
    // TODO figure out a policy for this class: do we check X-compatibility for
    // operations like this?
    m_values += rhs.m_values;
    // Need to check errorMode of both, lhs and rhs!
    if (!implicitErrors() || !rhs.implicitErrors()) {
      switchTo(ErrorMode::Variance);
      m_variances.get() += rhs.variances();
    }
    return *this;
  }

  size_t size() const { return m_values.size(); };
  void resize(size_t count) {
    m_edges.resize(count + 1);
    m_values.resize(count);
    if (explicitErrors())
      m_variances->resize(count);
  };

private:
  void switchToExplicitErrors() {
    if (implicitErrors())
      m_variances = variances();
  }

  void switchTo(ErrorMode mode) {
    if(mode == m_errorMode)
      return;
    switch(mode) {
      case ErrorMode::Implicit:
        m_variances = boost::none;
        m_standardDeviations = boost::none;
        break;
      case ErrorMode::Variance:
        m_variances = variances();
        m_standardDeviations = boost::none;
        break;
      case ErrorMode::StandardDeviation:
        m_variances = boost::none;
        m_standardDeviations = standardDeviations();
        break;
    }
    m_errorMode = mode;
  }

  //BinVariances &getVariance() {
  //  return const_cast<BinVariances &>(
  //      static_cast<const Histogram *>(this)->getVariance());
  //}
};

// NO! values is distribution, need to factor in width!
//BinVariances variance(const BinValues &values) { return {values.data()}; }

// BinStandardDeviation standardDeviation(const BinValues &values) {
//  BinStandardDeviation sigma;
//  return std::transform(begin(values), end(values), std::back_inserter(sigma),
//  std::sqrt<double>);
//}

// inline Histogram operator+(Histogram lhs, const Histogram &rhs) {
//  lhs += rhs;
//  return lhs;
//}

} // Namespace Kernel
} // Namespace Mantid

#endif /*MANTID_KERNEL_HISTOGRAM_H_*/
