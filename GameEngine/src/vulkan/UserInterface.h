/* Dear ImGui */
#pragma once
#include <string>
#include <vector>

#include <imgui.h>

#include "VkRenderData.h"
#include "../model/AssimpModel.h"
#include "../model/AssimpInstance.h"
#include "../vulkan/ModelInstanceCamData.h"
#include "../model/InstanceSettings.h"
#include "../tools/CameraSettings.h"
#include "../tools/Camera.h"
#include "../model/SingleInstanceBehavior.h"
#include "../model/AssimpLevel.h"

#include "../editor/hierarchy.h"
#include "../editor/inspector.h"
#include "../editor/project.h"

class UserInterface {
  public:
      
    bool init(VkRenderData& renderData);
    void hideMouse(bool hide);

    void createFrame(VkRenderData& renderData);
    void createSettingsWindow(VkRenderData& renderData, ModelInstanceCamData& modInstCamData);
    void createStatusBar(VkRenderData &renderData, ModelInstanceCamData &modInstCamData);

    void render(VkRenderData& renderData);
    void cleanup(VkRenderData& renderData);

    ImVec2 thinkingWindowPos;

private:

    std::string mControlsHelpText;
    const char* settingKeyName;

    bool isControlsSetting = false;

    bool mInstancePosWindowOpen = true;
    bool mStatusBarVisible = true;

    bool mControlsWindowOpen = false;
};
