#pragma once
#include "../entity/entity.h"
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <unordered_map>
#include <vector>

namespace Engine {

class SpatialGrid {
public:
  explicit SpatialGrid(float cellSize = 2.0f) : m_cellSize(cellSize) {}

  void clear() { m_cells.clear(); }

  void insert(uint32_t id, const AABB &box) {
    int minX = cellCoord(box.min.x);
    int maxX = cellCoord(box.max.x);
    int minZ = cellCoord(box.min.z);
    int maxZ = cellCoord(box.max.z);

    for (int x = minX; x <= maxX; x++)
      for (int z = minZ; z <= maxZ; z++)
        m_cells[packKey(x, z)].push_back(id);
  }

  void query(const AABB &box, std::vector<uint32_t> &result) const {
    result.clear();
    int minX = cellCoord(box.min.x);
    int maxX = cellCoord(box.max.x);
    int minZ = cellCoord(box.min.z);
    int maxZ = cellCoord(box.max.z);

    for (int x = minX; x <= maxX; x++) {
      for (int z = minZ; z <= maxZ; z++) {
        auto it = m_cells.find(packKey(x, z));
        if (it != m_cells.end())
          result.insert(result.end(), it->second.begin(), it->second.end());
      }
    }

    std::sort(result.begin(), result.end());
    result.erase(std::unique(result.begin(), result.end()), result.end());
  }

private:
  float m_cellSize;
  std::unordered_map<int64_t, std::vector<uint32_t>> m_cells;

  int cellCoord(float v) const {
    return static_cast<int>(std::floor(v / m_cellSize));
  }

  int64_t packKey(int x, int z) const {
    return (static_cast<int64_t>(x) << 32) | static_cast<uint32_t>(z);
  }
};

} // namespace Engine
