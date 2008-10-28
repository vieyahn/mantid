#ifndef MANTID_ALGORITHM_DIFFRACTIONFOCUSSING_H_
#define MANTID_ALGORITHM_DIFFRACTIONFOCUSSING_H_

//----------------------------------------------------------------------
// Includes
//----------------------------------------------------------------------
#include "MantidAPI/Algorithm.h"
#include <Poco/NObserver.h>

namespace Mantid
{
  namespace Algorithms
  {
    /** 
    This is a parent algorithm that uses several different child algorithms to perform it's task.
    Takes a workspace as input and the filename of a grouping file of a suitable format.
    
    The input workspace is 
    1) Converted to d-spacing units
    2) Rebinnned to a common set of bins
    3) The spectra are grouped according to the grouping file.
    
	Required Properties:
    <UL>
    <LI> InputWorkspace - The name of the 2D Workspace to take as input </LI>
    <LI> GroupingFileName - The path to a grouping file</LI>
    <LI> OutputWorkspace - The name of the 2D workspace in which to store the result </LI>
    </UL>

    The structure of the grouping file is as follows:
    # Format: number  UDET offset  select  group
    0        611  0.0000000  1    0
    1        612  0.0000000  1    0
    2        601  0.0000000  0    0
    3        602  0.0000000  0    0
    4        621  0.0000000  1    0

   
    @author Nick Draper, Tessella
    @date 11/07/2008

    Copyright &copy; 2008 STFC Rutherford Appleton Laboratories

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

    File change history is stored at: <https://svn.mantidproject.org/mantid/trunk/Code/Mantid>    
    Code Documentation is available at: <http://doxygen.mantidproject.org>
    */
    class DLLExport DiffractionFocussing : public API::Algorithm
    {
    public:
      /// Default constructor
      DiffractionFocussing() : API::Algorithm(),m_childProgressObserver(*this, &DiffractionFocussing::handleChildProgressNotification) {};
      /// Destructor
      virtual ~DiffractionFocussing() {};
      /// Algorithm's name for identification overriding a virtual method
      virtual const std::string name() const { return "DiffractionFocussing";}
      /// Algorithm's version for identification overriding a virtual method
      virtual const int version() const { return 1;}
      /// Algorithm's category for identification overriding a virtual method
      virtual const std::string category() const { return "Diffraction";}
    
    private:
      // Overridden Algorithm methods
      void init();
      void exec();
      API::Workspace_sptr convertUnitsToDSpacing(API::Workspace_sptr workspace, std::string outputWorkspaceName);
      void RebinWorkspace(API::Workspace_sptr& workspace);
      void calculateRebinParams(API::Workspace_sptr workspace,double& min,double& max,double& step);
      bool readGroupingFile(std::string groupingFileName, std::multimap<int,int>& detectorGroups);

      /// Captures childs progress notifications.
      Poco::NObserver<DiffractionFocussing, ProgressNotification> m_childProgressObserver;
      void handleChildProgressNotification(const Poco::AutoPtr<ProgressNotification>& pNf);
    
      
	  /// Static reference to the logger class
      static Mantid::Kernel::Logger& g_log;
    };

  } // namespace Algorithm
} // namespace Mantid

#endif /*MANTID_ALGORITHM_DIFFRACTIONFOCUSSING_H_*/
