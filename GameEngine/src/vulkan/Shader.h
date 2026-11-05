/* Vulkan shader */
#pragma once

#include <string>
#include <vector>
#include <vulkan/vulkan.h>

class Shader {
  public:
    static VkShaderModule loadShader(VkDevice device, std::string shaderFileName);
    static std::vector<char> loadBinaryFile(const std::string& filename);
    static void cleanup(VkDevice device, VkShaderModule module);
};
