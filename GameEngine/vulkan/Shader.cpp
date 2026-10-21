#include <fstream>

#include "Shader.h"
#include "../tools/Tools.h"
#include "../tools/Logger.h"

#pragma warning(disable:4996)

std::vector<char> Shader::loadBinaryFile(const std::string& filename) {
    std::ifstream file(filename, std::ios::ate | std::ios::binary);
    if (!file.is_open()) {
        Logger::log(1, "%s error: could not open file %s\n", __FUNCTION__, filename.c_str());
        return {};
    }

    size_t fileSize = (size_t)file.tellg();
    std::vector<char> buffer(fileSize);

    file.seekg(0);
    file.read(buffer.data(), fileSize);
    file.close();

    return buffer;
}

VkShaderModule Shader::loadShader(VkDevice device, std::string shaderFileName) {
    std::vector<char> shaderCode = loadBinaryFile(shaderFileName);

    if (shaderCode.empty()) {
        Logger::log(1, "%s: shader file '%s' is empty or failed to load\n", __FUNCTION__, shaderFileName.c_str());
        return VK_NULL_HANDLE;
    }

    VkShaderModuleCreateInfo shaderCreateInfo{};
    shaderCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    shaderCreateInfo.codeSize = shaderCode.size();
    shaderCreateInfo.pCode = reinterpret_cast<const uint32_t*>(shaderCode.data());

    VkShaderModule shaderModule;
    if (vkCreateShaderModule(device, &shaderCreateInfo, nullptr, &shaderModule) != VK_SUCCESS) {
        Logger::log(1, "%s: could not create shader module from file '%s'\n", __FUNCTION__, shaderFileName.c_str());
        return VK_NULL_HANDLE;
    }

    return shaderModule;
}

void Shader::cleanup(VkDevice device, VkShaderModule module) {
  vkDestroyShaderModule(device, module, nullptr);
}
