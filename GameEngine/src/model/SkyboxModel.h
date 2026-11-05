#pragma once
#include <vector>
#include <glm/glm.hpp>

#include "../vulkan/VkRenderData.h"

class SkyboxModel {
  public:
    void init();

    VkSkyboxMesh getVertexData();

  private:
    VkSkyboxMesh mVertexData{};
};
