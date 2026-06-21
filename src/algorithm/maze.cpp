#include "maze.h"
#include <algorithm>
#include <random>
#include <stack>

namespace Engine {

std::vector<std::vector<int>> MazeGenerator::generate(const MazeConfig &config) {
  uint32_t w = config.width;
  uint32_t h = config.height;

  std::vector<std::vector<int>> grid(h, std::vector<int>(w, 1));
  std::vector<std::vector<bool>> visited(h, std::vector<bool>(w, false));

  std::mt19937 rng(config.seed != 0 ? config.seed : std::random_device{}());
  std::stack<std::pair<uint32_t, uint32_t>> stk;

  uint32_t sx = 0;
  uint32_t sy = 0;
  grid[sy][sx] = 0;
  visited[sy][sx] = true;
  stk.push({sx, sy});

  const int dx[] = {0, 0, 2, -2};
  const int dy[] = {-2, 2, 0, 0};

  while (!stk.empty()) {
    auto [cx, cy] = stk.top();

    std::vector<uint32_t> dirs = {0, 1, 2, 3};
    std::shuffle(dirs.begin(), dirs.end(), rng);

    bool found = false;
    for (uint32_t d : dirs) {
      uint32_t nx = cx + dx[d];
      uint32_t ny = cy + dy[d];

      if (nx < w && ny < h && !visited[ny][nx]) {
        visited[ny][nx] = true;
        grid[ny][nx] = 0;
        grid[cy + dy[d] / 2][cx + dx[d] / 2] = 0;
        stk.push({nx, ny});
        found = true;
        break;
      }
    }

    if (!found)
      stk.pop();
  }

  return grid;
}

} // namespace Engine
