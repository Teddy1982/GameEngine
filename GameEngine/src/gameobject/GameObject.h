#pragma once

#include <cstdint>
#include <string>
#include <utility>

#include "../vulkan/ModelInstanceCamData.h"
#include "../editor/enums.h"

using AssimpInstanceVector =
decltype(std::declval<ModelInstanceCamData&>().micAssimpInstances);

using AssimpInstancePtr = AssimpInstanceVector::value_type;

struct GameObject {
    std::string name;
    std::uint64_t id = 0;
    FileType fileType;

    AssimpInstancePtr instance{};
};