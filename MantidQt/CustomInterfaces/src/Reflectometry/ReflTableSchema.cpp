#include "MantidQtCustomInterfaces/Reflectometry/ReflTableSchema.h"
#include <utility>

namespace MantidQt {
namespace CustomInterfaces {
namespace ReflTableSchema {

ColumnIndexNameMap makeColumnIndexMap() {
  return {{COL_RUNS, RUNS},
          {COL_ANGLE, ANGLE},
          {COL_TRANSMISSION, TRANSMISSION},
          {COL_QMIN, QMINDEF},
          {COL_QMAX, QMAXDEF},
          {COL_DQQ, DQQ},
          {COL_SCALE, SCALE},
          {COL_GROUP, GROUP},
          {COL_OPTIONS, OPTIONS}};
}

ColumnNameIndexMap makeColumnNameMap() {
  auto indexMap = makeColumnIndexMap();
  ColumnNameIndexMap columnMap;
  for (auto it = indexMap.begin(); it != indexMap.end(); ++it) {
    columnMap.emplace(it->second, it->first);
  }
  return columnMap;
}
}
} // namespace CustomInterfaces
} // namespace Mantid
