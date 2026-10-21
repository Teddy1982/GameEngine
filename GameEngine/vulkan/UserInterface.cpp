#include <string>
#include <limits>

#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>
#include <misc/cpp/imgui_stdlib.h> // ImGui::InputText with std::string

#include <ImGuiFileDialog.h>

#include <imnodes.h>

#include <filesystem>

#include "UserInterface.h"
#include "CommandBuffer.h"
#include "../model/AssimpAnimClip.h"
#include "../model/AssimpSettingsContainer.h"
#include "../model/ModelSettings.h"
#include "../model/LevelSettings.h"
#include "../tools/Logger.h"
#include "../octree/BoundingBox3D.h"
#include "../tools/Tools.h"
#include "../editor/user_interface_globals.h"

#include "imgui_internal.h"

bool UserInterface::init(VkRenderData& renderData) {
  IMGUI_CHECKVERSION();

  ImGui::CreateContext();
  ImGuiIO& imguiIo = ImGui::GetIO();
  imguiIo.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
  imguiIo.ConfigDockingWithShift = false;

  ImNodes::CreateContext();

  std::vector<VkDescriptorPoolSize> imguiPoolSizes =
  {
    { VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
    { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
    { VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
    { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
    { VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
    { VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
    { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
    { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
    { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
    { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
    { VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 }
  };

  VkDescriptorPoolCreateInfo imguiPoolInfo{};
  imguiPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
  imguiPoolInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
  imguiPoolInfo.maxSets = 1000;
  imguiPoolInfo.poolSizeCount = static_cast<uint32_t>(imguiPoolSizes.size());
  imguiPoolInfo.pPoolSizes = imguiPoolSizes.data();

  VkResult result = vkCreateDescriptorPool(renderData.rdVkbDevice.device, &imguiPoolInfo, nullptr, &renderData.rdImguiDescriptorPool);
  if (result != VK_SUCCESS) {
    Logger::log(1, "%s error: could not init ImGui descriptor pool \n", __FUNCTION__);
    return false;
  }

  if (!ImGui_ImplGlfw_InitForVulkan(renderData.rdWindow, true)) {
    Logger::log(1, "%s error: could not init ImGui GLFW for Vulkan \n", __FUNCTION__);
    return false;
  }

  ImGui_ImplVulkan_InitInfo imguiIinitInfo{};
  imguiIinitInfo.Instance = renderData.rdVkbInstance.instance;
  imguiIinitInfo.PhysicalDevice = renderData.rdVkbPhysicalDevice.physical_device;
  imguiIinitInfo.Device = renderData.rdVkbDevice.device;
  imguiIinitInfo.Queue = renderData.rdGraphicsQueue;
  imguiIinitInfo.DescriptorPool = renderData.rdImguiDescriptorPool;
  imguiIinitInfo.MinImageCount = 2;
  imguiIinitInfo.ImageCount = static_cast<uint32_t>(renderData.rdSwapchainImages.size());
  imguiIinitInfo.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
  //  imguiIinitInfo.RenderPass = renderData.rdImGuiRenderpass;

  //=======================================================================================================
   //=======================================================================================================

  if (!ImGui_ImplVulkan_Init(&imguiIinitInfo, renderData.rdRenderpass)) {
      Logger::log(1, "%s error: could not init ImGui for Vulkan \n", __FUNCTION__);
      return false;
  }

  ImGuiIO& io1 = ImGui::GetIO();
  if (io1.Fonts->Fonts.empty()) {
      io1.Fonts->AddFontDefault();
  }

  {
      VkCommandPool uploadPool = renderData.rdCommandPool;
      VkCommandBuffer cmd = CommandBuffer::createSingleShotBuffer(renderData, uploadPool);
      if (cmd == VK_NULL_HANDLE) {
          Logger::log(1, "%s error: could not create single shot cmd buffer for ImGui font upload\n", __FUNCTION__);
          return false;
      }

      ImGui_ImplVulkan_CreateFontsTexture(cmd);

      if (!CommandBuffer::submitSingleShotBuffer(renderData, uploadPool, cmd, renderData.rdGraphicsQueue)) {
          Logger::log(1, "%s error: could not submit font upload command buffer\n", __FUNCTION__);
          return false;
      }

      ImGui_ImplVulkan_DestroyFontUploadObjects();
  }

  //=======================================================================================================
  //=======================================================================================================

  ImGui::StyleColorsDark();
  ImNodes::StyleColorsDark();

  
  /* Use CTRL to detach links */
  ImNodesIO& io = ImNodes::GetIO();
  io.LinkDetachWithModifierClick.Modifier = &ImGui::GetIO().KeyCtrl;

  return true;
}

void UserInterface::createFrame(VkRenderData &renderData) {
  ImGui_ImplVulkan_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();

  
 ImGuiID dockspaceId = ImGui::DockSpaceOverViewport(
      ImGui::GetMainViewport(),
      ImGuiDockNodeFlags_NoDockingInCentralNode |
      ImGuiDockNodeFlags_PassthruCentralNode
  );
  
  hierarchyWindow.createFrame();
  inspectorWindow.createFrame();
  projectWindow.createFrame();

  if (ImGuiDockNode* centralNode =
      ImGui::DockBuilderGetCentralNode(dockspaceId)) {

      const ImVec2 mainPos = ImGui::GetMainViewport()->Pos;

      // Position relativ zum Hauptfenster.
      renderData.rdSceneViewportPos = glm::vec2(
          centralNode->Pos.x - mainPos.x,
          centralNode->Pos.y - mainPos.y
      );

      renderData.rdSceneViewportSize = glm::vec2(
          centralNode->Size.x,
          centralNode->Size.y
      );
  }
}

void UserInterface::createSettingsWindow(VkRenderData& renderData, ModelInstanceCamData& modInstCamData) {
    ImGuiIO& io = ImGui::GetIO();
    ImGuiWindowFlags imguiWindowFlags = 0;

    ImGui::SetNextWindowBgAlpha(0.8f);

    /* dim background for modal dialogs */
    ImGuiStyle& style = ImGui::GetStyle();
    style.Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.75f);

    bool loadModelRequest = false;
    bool loadLevelRequest = false;

    bool openUnsavedChangesNewDialog = false;
    bool openUnsavedChangesLoadDialog = false;
    bool openUnsavedChangesExitDialog = false;

    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            ImGui::MenuItem("New Config", "CTRL+N", &renderData.rdNewConfigRequest);
            ImGui::MenuItem("Load Config", "CTRL+L", &renderData.rdLoadConfigRequest);
            ImGui::BeginDisabled(modInstCamData.micModelList.size() == 1);
            ImGui::MenuItem("Save Config", "CTRL+S", &renderData.rdSaveConfigRequest);
            ImGui::EndDisabled();
            ImGui::MenuItem("Exit", "CTRL+Q", &renderData.rdRequestApplicationExit);
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Edit")) {
            ImGui::BeginDisabled(modInstCamData.micSettingsContainer->getUndoSize() == 0);
            if (ImGui::MenuItem("Undo", "CTRL+Z")) {
                modInstCamData.micUndoCallbackFunction();
            }
            ImGui::EndDisabled();

            ImGui::BeginDisabled(modInstCamData.micSettingsContainer->getRedoSize() == 0);
            if (ImGui::MenuItem("Redo", "CTRL+Y")) {
                modInstCamData.micRedoCallbackFunction();
            }
            ImGui::EndDisabled();
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Models")) {
            ImGui::MenuItem("Load Model...", nullptr, &loadModelRequest);
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Levels")) {
            ImGui::MenuItem("Load Level...", nullptr, &loadLevelRequest);
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("View")) {
            ImGui::MenuItem("Instance Positions", nullptr, &mInstancePosWindowOpen);
            ImGui::MenuItem("Status Bar", nullptr, &mStatusBarVisible);
            ImGui::EndMenu();
        }

        if (!mControlsHelpText.empty()) {
            if (ImGui::BeginMenu("Help")) {
                ImGui::MenuItem("Show Controls", "F1", &renderData.rdShowControlsHelpRequest);
                ImGui::EndMenu();
            }
        }
        ImGui::EndMainMenuBar();
    }

    /* application exit */
    if (renderData.rdRequestApplicationExit) {
        ImGuiFileDialog::Instance()->Close();
        ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x * 0.5f, io.DisplaySize.y * 0.5f), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
        ImGui::OpenPopup("Do you want to quit?");
    }

    if (ImGui::BeginPopupModal("Do you want to quit?", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("  Exit Application?  ");

        /* cheating a bit to get buttons more to the center */
        ImGui::Indent();
        if (ImGui::Button("OK") || ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Enter))) {
            if (modInstCamData.micGetConfigDirtyCallbackFunction()) {
                openUnsavedChangesExitDialog = true;
                renderData.rdRequestApplicationExit = false;
            }
            else {
                renderData.rdAppExitCallbackFunction();
            }
            ImGui::CloseCurrentPopup();
        }

        ImGui::SameLine();
        if (ImGui::Button("Cancel") || ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Escape))) {
            renderData.rdRequestApplicationExit = false;
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }

    /* unsaved changes, ask */
    if (openUnsavedChangesExitDialog) {
        ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x * 0.5f, io.DisplaySize.y * 0.5f), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
        ImGui::OpenPopup("Exit - Unsaved Changes");
    }

    if (ImGui::BeginPopupModal("Exit - Unsaved Changes", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("You have unsaved Changes!");
        ImGui::Text("Still exit?");

        /* cheating a bit to get buttons more to the center */
        ImGui::Indent();
        if (ImGui::Button("OK") || ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Enter))) {
            renderData.rdAppExitCallbackFunction();
            ImGui::CloseCurrentPopup();
        }

        ImGui::SameLine();
        if (ImGui::Button("Cancel") || ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Escape))) {
            renderData.rdRequestApplicationExit = false;
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }

    /* new config */
    if (renderData.rdNewConfigRequest) {
        if (modInstCamData.micGetConfigDirtyCallbackFunction()) {
            openUnsavedChangesNewDialog = true;
        }
        else {
            modInstCamData.micNewConfigCallbackFunction();
        }
    }

    /* unsaved changes, ask */
    if (openUnsavedChangesNewDialog) {
        ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x * 0.5f, io.DisplaySize.y * 0.5f), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
        ImGui::OpenPopup("New - Unsaved Changes");
    }

    if (ImGui::BeginPopupModal("New - Unsaved Changes", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("You have unsaved Changes!");
        ImGui::Text("Continue?");

        /* cheating a bit to get buttons more to the center */
        ImGui::Indent();
        if (ImGui::Button("OK") || ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Enter))) {
            modInstCamData.micNewConfigCallbackFunction();
            ImGui::CloseCurrentPopup();
        }

        ImGui::SameLine();
        if (ImGui::Button("Cancel") || ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Escape))) {
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }

    const std::string defaultFileName = "config/conf.acfg";

    /* load config */
    if (renderData.rdLoadConfigRequest) {
        IGFD::FileDialogConfig config;
        config.path = ".";
        config.countSelectionMax = 1;
        config.flags = ImGuiFileDialogFlags_Modal;
        config.filePathName = defaultFileName;
        ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x * 0.5f, io.DisplaySize.y * 0.5f), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
        ImGuiFileDialog::Instance()->OpenDialog("LoadConfigFile", "Load Configuration File",
            ".acfg", config);
    }

    bool loadSuccessful = true;
    if (ImGuiFileDialog::Instance()->Display("LoadConfigFile")) {
        if (ImGuiFileDialog::Instance()->IsOk()) {
            if (modInstCamData.micGetConfigDirtyCallbackFunction()) {
                openUnsavedChangesLoadDialog = true;
            }
            else {
                std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
                loadSuccessful = modInstCamData.micLoadConfigCallbackFunction(filePathName);
            }
        }
        ImGuiFileDialog::Instance()->Close();
    }

    /* ask for replacement  */
    if (openUnsavedChangesLoadDialog) {
        ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x * 0.5f, io.DisplaySize.y * 0.5f), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
        ImGui::OpenPopup("Load - Unsaved Changes");
    }

    if (ImGui::BeginPopupModal("Load - Unsaved Changes", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("You have unsaved Changes!");
        ImGui::Text("Continue?");

        /* cheating a bit to get buttons more to the center */
        ImGui::Indent();
        if (ImGui::Button("OK") || ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Enter))) {
            std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
            loadSuccessful = modInstCamData.micLoadConfigCallbackFunction(filePathName);
            ImGui::CloseCurrentPopup();
        }

        ImGui::SameLine();
        if (ImGui::Button("Cancel") || ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Escape))) {
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }

    /* show error message if load was not successful */
    if (!loadSuccessful) {
        ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x * 0.5f, io.DisplaySize.y * 0.5f), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
        ImGui::OpenPopup("Load Error!");
    }

    if (ImGui::BeginPopupModal("Load Error!", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("Error loading config!");
        ImGui::Text("Check console output!");

        /* cheating a bit to get buttons more to the center */
        ImGui::Indent();
        ImGui::Indent();
        ImGui::Indent();
        if (ImGui::Button("OK") || ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Enter))) {
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }

    /* save config */
    if (renderData.rdSaveConfigRequest) {
        IGFD::FileDialogConfig config;
        config.path = ".";
        config.countSelectionMax = 1;
        config.flags = ImGuiFileDialogFlags_Modal | ImGuiFileDialogFlags_ConfirmOverwrite;
        config.filePathName = defaultFileName;
        ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x * 0.5f, io.DisplaySize.y * 0.5f), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
        ImGuiFileDialog::Instance()->OpenDialog("SaveConfigFile", "Save Configuration File",
            ".acfg", config);
    }

    bool saveSuccessful = true;
    if (ImGuiFileDialog::Instance()->Display("SaveConfigFile")) {
        if (ImGuiFileDialog::Instance()->IsOk()) {
            std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
            saveSuccessful = modInstCamData.micSaveConfigCallbackFunction(filePathName);

            if (saveSuccessful) {
                modInstCamData.micSetConfigDirtyCallbackFunction(false);
            }
        }
        ImGuiFileDialog::Instance()->Close();
    }

    /* show error message if save was not successful */
    if (!saveSuccessful) {
        ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x * 0.5f, io.DisplaySize.y * 0.5f), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
        ImGui::OpenPopup("Save Error!");
    }

    if (ImGui::BeginPopupModal("Save Error!", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("Error saving config!");
        ImGui::Text("Check console output!");

        /* cheating a bit to get buttons more to the center */
        ImGui::Indent();
        ImGui::Indent();
        ImGui::Indent();
        if (ImGui::Button("OK") || ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Enter))) {
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }

    /* load model */
    if (loadModelRequest) {
        IGFD::FileDialogConfig config;
        config.path = ".";
        config.countSelectionMax = 1;
        config.flags = ImGuiFileDialogFlags_Modal;
        ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x * 0.5f, io.DisplaySize.y * 0.5f), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
        ImGuiFileDialog::Instance()->OpenDialog("ChooseModelFile", "Choose Model File",
            "Supported Model Files{.gltf,.glb,.obj,.fbx,.dae,.mdl,.md3,.pk3}", config);
    }

    if (ImGuiFileDialog::Instance()->Display("ChooseModelFile")) {
        if (ImGuiFileDialog::Instance()->IsOk()) {

            std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();

            /* try to construct a relative path */
            std::filesystem::path currentPath = std::filesystem::current_path();
            std::string relativePath = std::filesystem::relative(filePathName, currentPath).generic_string();

            if (!relativePath.empty()) {
                filePathName = relativePath;
            }
            /* Windows does understand forward slashes, but std::filesystem preferres backslashes... */
            std::replace(filePathName.begin(), filePathName.end(), '\\', '/');

            if (!modInstCamData.micModelAddCallbackFunction(filePathName, true, true)) {
                Logger::log(1, "%s error: unable to load model file '%s', unnown error \n", __FUNCTION__, filePathName.c_str());
            }
        }
        ImGuiFileDialog::Instance()->Close();
    }

    /* load level */
    if (loadLevelRequest) {
        IGFD::FileDialogConfig config;
        config.path = ".";
        config.countSelectionMax = 1;
        config.flags = ImGuiFileDialogFlags_Modal;
        ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x * 0.5f, io.DisplaySize.y * 0.5f), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
        ImGuiFileDialog::Instance()->OpenDialog("ChooseLevelFile", "Choose Level File",
            "Supported Level Files{.gltf,.glb,.obj,.fbx,.dae,.pk3}", config);
    }

    if (ImGuiFileDialog::Instance()->Display("ChooseLevelFile")) {
        if (ImGuiFileDialog::Instance()->IsOk()) {

            std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();

            /* try to construct a relative path */
            std::filesystem::path currentPath = std::filesystem::current_path();
            std::string relativePath = std::filesystem::relative(filePathName, currentPath).generic_string();

            if (!relativePath.empty()) {
                filePathName = relativePath;
            }
            /* Windows does understand forward slashes, but std::filesystem preferres backslashes... */
            std::replace(filePathName.begin(), filePathName.end(), '\\', '/');

            if (!modInstCamData.micLevelAddCallbackFunction(filePathName)) {
                Logger::log(1, "%s error: unable to load level file '%s', unnown error \n", __FUNCTION__, filePathName.c_str());
            }
        }
        ImGuiFileDialog::Instance()->Close();
    }

    /* show help text */
    if (renderData.rdShowControlsHelpRequest && !mControlsHelpText.empty()) {
        ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x * 0.5f, io.DisplaySize.y * 0.5f), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
        ImGui::OpenPopup("Controls");
    }

    bool helpWindowOpen = true;
    if (ImGui::BeginPopupModal("Controls", &helpWindowOpen, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::TextWrapped("%s", mControlsHelpText.c_str());
        ImGui::EndPopup();
    }

    /* reset values to false to avoid side-effects */
    renderData.rdNewConfigRequest = false;
    renderData.rdLoadConfigRequest = false;
    renderData.rdSaveConfigRequest = false;
    renderData.rdShowControlsHelpRequest = false;
}

void UserInterface::hideMouse(bool hide) {
  /* v1.89.8 removed the check for disabled mouse cursor in GLFW
   * we need to ignore the mouse postion if the mouse lock is active */
  ImGuiIO& io = ImGui::GetIO();

  if (hide) {
    io.ConfigFlags |= ImGuiConfigFlags_NoMouse;
  } else {
    io.ConfigFlags &= ~ImGuiConfigFlags_NoMouse;
  }
}

void UserInterface::createStatusBar(
    VkRenderData& renderData,
    ModelInstanceCamData& modInstCamData)
{
    if (!mStatusBarVisible) {
        return;
    }

    const ImGuiWindowFlags statusBarFlags =
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoScrollbar |
        ImGuiWindowFlags_NoResize;

    // Ausgewählte Instanz prüfen.
    const int instanceIndex = modInstCamData.micSelectedInstance;
    const auto& instances = modInstCamData.micAssimpInstances;

    if (instanceIndex >= 0 &&
        static_cast<std::size_t>(instanceIndex) < instances.size()) {

        inspectorWindow.mCurrentInstance = instances.at(instanceIndex);
    }
    else {
        // Keine veraltete Instanzreferenz behalten.
        inspectorWindow.mCurrentInstance.reset();
    }

    ImGui::SetNextWindowPos(
        ImVec2(0.0f, renderData.rdHeight - 35.0f),
        ImGuiCond_Always
    );

    ImGui::SetNextWindowSize(
        ImVec2(renderData.rdWidth, 35.0f)
    );

    ImGui::SetNextWindowBgAlpha(0.5f);

    if (ImGui::Begin("Status", nullptr, statusBarFlags)) {
        ImGui::AlignTextToFramePadding();
        ImGui::TextUnformatted("Mode (F10):");
        ImGui::SameLine();

        if (ImGui::Button(
            renderData.mAppModeMap.at(
                renderData.rdApplicationMode
            ).c_str())) {

            if (modInstCamData.micSsetAppModeCallbackFunction) {
                modInstCamData.micSsetAppModeCallbackFunction(
                    ++renderData.rdApplicationMode
                );
            }
        }

        // Auch die ausgewählte Kamera prüfen.
        const int cameraIndex = modInstCamData.micSelectedCamera;
        const auto& cameras = modInstCamData.micCameras;

        const char* cameraName = "Keine";

        if (cameraIndex >= 0 &&
            static_cast<std::size_t>(cameraIndex) < cameras.size() &&
            cameras.at(cameraIndex)) {

            cameraName = cameras.at(cameraIndex)->getName().c_str();

            ImGui::SameLine();
            ImGui::Text(
                " | Active Camera: %16s | FPS: %7.2f",
                cameraName,
                inspectorWindow.mFramesPerSecond
            );
        }
        else {
            ImGui::SameLine();
            ImGui::Text(
                " | Active Camera: %16s | FPS: %7.2f",
                cameraName,
                inspectorWindow.mFramesPerSecond
            );
        }

        ImGui::SameLine();

        if (inspectorWindow.mCurrentInstance) {
            const InstanceSettings settings =
                inspectorWindow.mCurrentInstance->getInstanceSettings();

            ImGui::Text(
                " | Speed: %2.4f | Accel: %2.4f | State: %6s",
                glm::length(settings.isSpeed),
                glm::length(settings.isAccel),
                modInstCamData.micMoveStateMap.at(
                    settings.isMoveState
                ).c_str()
            );
        }
        else {
            ImGui::TextUnformatted(
                " | Keine Instanz ausgewaehlt"
            );
        }
    }

    ImGui::End();
}


void UserInterface::render(VkRenderData& renderData) {
  ImGui::Render();
  ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), renderData.rdImGuiCommandBuffer);
}

void UserInterface::cleanup(VkRenderData& renderData) {
  ImGui_ImplVulkan_Shutdown();
  ImGui_ImplGlfw_Shutdown();

  ImNodes::DestroyContext();
  vkDestroyDescriptorPool(renderData.rdVkbDevice.device, renderData.rdImguiDescriptorPool, nullptr);
  ImGui::DestroyContext();
}
