#ifndef MANTID_SLICEVIEWER_NON_ORTHOGONAL_H_
#define MANTID_SLICEVIEWER_NON_ORTHOGONAL_H_

#include "DllOption.h"
#include "MantidAPI/IMDWorkspace.h"
#include "MantidKernel/Matrix.h"

namespace Mantid
{
namespace SliceViewer
{
void EXPORT_OPT_MANTIDQT_SLICEVIEWER provideSkewMatrix(Kernel::DblMatrix &skewMatrix, API::IMDWorkspace_sptr workspace);

bool EXPORT_OPT_MANTIDQT_SLICEVIEWER requiresSkewMatrix(API::IMDWorkspace_sptr workspace);

}
}

#endif
