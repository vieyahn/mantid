#ifndef MANTID_CURVEFITTING_STEEPESTDESCENT_MINIMIZERMINIMIZER_H_
#define MANTID_CURVEFITTING_STEEPESTDESCENT_MINIMIZERMINIMIZER_H_

//----------------------------------------------------------------------
// Includes
//----------------------------------------------------------------------
#include "MantidCurveFitting/DllConfig.h"
#include "MantidCurveFitting/FuncMinimizers/DerivMinimizer.h"

namespace Mantid {
namespace CurveFitting {
namespace FuncMinimisers {
/** Implementing the steepest descent algorithm
    by wrapping the IFuncMinimizer interface around the GSL implementation of
   this algorithm.

    @author Roman Tolchenov, Tessella plc

    Copyright &copy; 2009 ISIS Rutherford Appleton Laboratory, NScD Oak Ridge
   National Laboratory & European Spallation Source

    This file is part of Mantid.

    Mantid is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    Mantid is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

    File change history is stored at: <https://github.com/mantidproject/mantid>.
    Code Documentation is available at: <http://doxygen.mantidproject.org>
*/
class DLLExport SteepestDescentMinimizer : public DerivMinimizer {
public:
  /// Constructor.
  SteepestDescentMinimizer() : DerivMinimizer() {}
  /// Name of the minimizer.
  std::string name() const override { return "SteepestDescentMinimizer"; }

protected:
  /// Return a concrete type to initialize m_gslSolver with
  const gsl_multimin_fdfminimizer_type *getGSLMinimizerType() override;

  /// Static reference to the logger class
  static Kernel::Logger &g_log;
};

} // namespace FuncMinimisers
} // namespace CurveFitting
} // namespace Mantid

#endif /*MANTID_CURVEFITTING_STEEPESTDESCENT_MINIMIZERMINIMIZER_H_*/
