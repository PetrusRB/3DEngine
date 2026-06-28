#pragma once

namespace Engine {

struct UIComponent {
  virtual ~UIComponent() = default;
  virtual void render() = 0;
  bool visible = true;
};

} // namespace Engine
