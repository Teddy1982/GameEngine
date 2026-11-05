#pragma once

#include <filesystem>
#include <string>

#include "enums.h"
#include "../vulkan/ModelInstanceCamData.h"

class ProjectWindow {
public:

    void setModelInstanceCamData(ModelInstanceCamData& data) {
        modInstCamData = &data;
    }

    void createFrame();

private:

    void selectFile(std::string filename, FileType type);

    ModelInstanceCamData* modInstCamData = nullptr;
};

