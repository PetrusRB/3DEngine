#pragma once
#include <memory>
#include <vector>

namespace Engine {

struct Component {
  virtual ~Component() = default;
  virtual void renderUI() {}
  virtual const char *name() const = 0;
};

} // namespace Engine
