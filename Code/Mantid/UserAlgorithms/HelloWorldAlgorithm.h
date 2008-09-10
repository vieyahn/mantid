#ifndef USER_ALGORITHMS_HELLOWORLDALGORITHM_H_
#define USER_ALGORITHMS_HELLOWORLDALGORITHM_H_

#include "MantidAPI/Algorithm.h"

namespace Mantid
{
namespace Algorithms
{
/** Algorithm basic test class.

    @author Matt Clarke, ISIS, RAL
    @date 09/11/2007

    Copyright &copy; 2007 STFC Rutherford Appleton Laboratories

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

    File change history is stored at: <https://svn.mantidproject.org/mantid/trunk/Code/Mantid>.
    Code Documentation is available at: <http://doxygen.mantidproject.org>
 */
class HelloWorldAlgorithm : public API::Algorithm
{
public:
  ///no arg constructor
  HelloWorldAlgorithm() : API::Algorithm() {}
  ///virtual destructor
  virtual ~HelloWorldAlgorithm() {}
  /// Algorithm's name for identification overriding a virtual method
  virtual const std::string name() const { return "HelloWorldAlgorithm";}
  /// Algorithm's version for identification overriding a virtual method
  virtual const int version()const { return (1);}
  /// Algorithm's category for identification overriding a virtual method
  virtual const std::string category() const { return "Examples";}

private:
  ///Initialisation code
  void init();
  ///Execution code
  void exec();

  /// Static reference to the logger class
  static Mantid::Kernel::Logger& g_log;
};

} // namespace Algorithm
} // namespace Mantid

#endif /*USER_ALGORITHMS_HELLOWORLDALGORITHM_H_*/

