/* scale arrows */
#pragma once
#include <vector>
#include <glm/glm.hpp>

#include "../vulkan/VkRenderData.h"

class ScaleArrowsModel {
  public:
    VkLineMesh getVertexData();

  private:
    void init();
    VkLineMesh mVertexData{};
};
