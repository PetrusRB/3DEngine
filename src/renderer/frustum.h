#pragma once
#include "../entity/entity.h"
#include <glm/glm.hpp>

namespace Engine {

struct FrustumPlane {
  glm::vec3 normal;
  float d;

  float distance(const glm::vec3 &point) const {
    return glm::dot(normal, point) + d;
  }
};

class Frustum {
public:
  void update(const glm::mat4 &vp, float margin = 0.0f) {
    glm::mat4 t = glm::transpose(vp);
    m_planes[0] = makePlane(t[3] + t[0], margin);
    m_planes[1] = makePlane(t[3] - t[0], margin);
    m_planes[2] = makePlane(t[3] + t[1], margin);
    m_planes[3] = makePlane(t[3] - t[1], margin);
    m_planes[4] = makePlane(t[2], margin);
    m_planes[5] = makePlane(t[3] - t[2], margin);
  }

  bool isAABBInside(const AABB &aabb) const {
    for (int i = 0; i < 6; i++) {
      glm::vec3 pv = {m_planes[i].normal.x >= 0.0f ? aabb.max.x : aabb.min.x,
                      m_planes[i].normal.y >= 0.0f ? aabb.max.y : aabb.min.y,
                      m_planes[i].normal.z >= 0.0f ? aabb.max.z : aabb.min.z};
      if (m_planes[i].distance(pv) < 0.0f)
        return false;
    }
    return true;
  }

private:
  FrustumPlane m_planes[6];

  static FrustumPlane makePlane(const glm::vec4 &p, float margin) {
    float len = glm::length(glm::vec3(p));
    if (len < 1e-6f)
      return {glm::vec3(0.0f, 0.0f, 0.0f), 1.0f};
    return {glm::vec3(p) / len, p.w / len + margin};
  }
};

} // namespace Engine
