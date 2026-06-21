#pragma once
#include <cstdint>
#include <vector>

namespace Engine {

struct MazeConfig {
  uint32_t width = 15;
  uint32_t height = 15;
  uint32_t seed = 0;
};

class MazeGenerator {
public:
  static std::vector<std::vector<int>> generate(const MazeConfig &config);

private:
  static void carve(uint32_t x, uint32_t y,
                    std::vector<std::vector<int>> &grid,
                    std::vector<std::vector<bool>> &visited,
                    uint32_t width, uint32_t height);
};

} // namespace Engine
