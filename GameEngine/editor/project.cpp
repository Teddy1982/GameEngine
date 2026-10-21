#include "project.h"

#include <windows.h>
#include <utility>

#include "imgui.h"
#include "user_interface_globals.h"
#include "../tools/Logger.h"

namespace fs = std::filesystem;

void ProjectWindow::createFrame()
{
    fs::path projectsPath = ".\\projects";
    static fs::path rootPath = projectsPath;
    static fs::path selectedDirectory = rootPath;
    static fs::path selectedFile = "";

    ImGui::Begin("Project");

    ImVec2 available = ImGui::GetContentRegionAvail();
    ImVec2 leftSize(available.x / 4.0f, available.y);
    ImVec2 rightSize(available.x - leftSize.x - ImGui::GetStyle().ItemSpacing.x, available.y);

    // Linkes ChildWindow
    ImGui::BeginChild("Project_Left", leftSize, true);

    ImGui::Text("Path: %s", selectedDirectory.string().c_str());
    ImGui::Separator();

    if (ImGui::Button(".."))
    {
        if (rootPath != projectsPath && rootPath.has_parent_path()) {
            rootPath = rootPath.parent_path();
            selectedFile = "";
        }
    }

    std::error_code ec;
    for (const auto& entry : fs::directory_iterator(rootPath, ec))
    {
        const fs::path path = entry.path();
        const std::string filename = path.filename().string();

        if (entry.is_directory())
        {
            bool selected = (selectedDirectory == path);

            std::string label = "[DIR] " + filename;

            if (ImGui::Selectable(label.c_str(), selected))
            {
                selectedDirectory = path;
            }

            if (ImGui::IsItemHovered() &&
                ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
            {
                if (entry.is_directory())
                {
                    rootPath = path;
                }
            }
        }
    }

    ImGui::EndChild();

    ImGui::SameLine();

    // Rechtes ChildWindow

    ImGui::BeginChild((selectedDirectory.empty()) ? "##" : selectedDirectory.string().c_str(), rightSize, true);

    if (!selectedDirectory.empty())
    {
        ImGui::Text("Selected: %s", selectedFile.filename().string().c_str());
        ImGui::Separator();

        if (ImGui::Button(".."))
        {
            if (selectedDirectory != projectsPath && selectedDirectory.has_parent_path()) {
                selectedDirectory = selectedDirectory.parent_path();
                rootPath = selectedDirectory;
                selectedFile = "";
            }
        }

        std::error_code ec2;
        for (const auto& entry : fs::directory_iterator(selectedDirectory, ec2))
        {
            const fs::path path = entry.path();
            const std::string filename = path.filename().string();

            if (entry.is_directory())
            {
                std::string label = "[DIR] " + filename;

                if (ImGui::Selectable(label.c_str()))
                {
                    selectedDirectory = path;
                }
            }
            else
            {
                if (ImGui::Selectable(filename.c_str()))
                {

                }

                if (ImGui::IsItemHovered() &&
                    ImGui::IsMouseClicked(ImGuiMouseButton_Left))
                {
                    selectedFile = path;
                }

                if (ImGui::IsItemHovered() &&
                    ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
                {
                    if (entry.is_directory())
                    {
                        rootPath = path;
                        break;
                    }

                    for (std::pair<std::vector<std::string>, FileType> type : fileDataTypes) {
                        for (std::string fileSuffix : type.first) {
                            if (filename.ends_with(fileSuffix)) {
                                if (type.second == FileType::MODEL) {
                                    const std::string filePathName = path.generic_string();

                                    if (!modInstCamData) {
                                        Logger::log(
                                            1,
                                            "ProjectWindow: model data is not assigned\n"
                                        );
                                    }
                                    else if (!modInstCamData->micModelAddCallbackFunction) {
                                        Logger::log(
                                            1,
                                            "ProjectWindow: model callback is not initialized\n"
                                        );
                                    }
                                    else if (!modInstCamData->micModelAddCallbackFunction(
                                        filePathName,
                                        true,
                                        true)) {

                                        Logger::log(
                                            1,
                                            "Unable to load model '%s'\n",
                                            filePathName.c_str()
                                        );
                                    }
                                    else {
                                        // Erst nach erfolgreichem Laden die neue Instanz übernehmen.
                                        const int index = modInstCamData->micSelectedInstance;
                                        const auto& instances = modInstCamData->micAssimpInstances;

                                        if (index >= 0 &&
                                            static_cast<std::size_t>(index) < instances.size() &&
                                            instances.at(index)) {

                                            GameObject gameObject;
                                            gameObject.name = filename;
                                            gameObject.instance = instances.at(index);

                                            hierarchyWindow.addGameObject(std::move(gameObject));
                                        }
                                        else {
                                            Logger::log(
                                                1,
                                                "ProjectWindow: loaded instance index is invalid\n"
                                            );
                                        }
                                    }
                                }
                                selectFile(filename, type.second);
                                ImGui::EndChild();
                                ImGui::End();
                                return;
                            }
                        }
                        selectFile(filename, FileType::UNDEFINED);
                    }
                }
            }
        }
    }

    ImGui::EndChild();

    ImGui::End();
}

void ProjectWindow::selectFile(std::string filename, FileType type) {
    inspectorWindow.showData(filename, type);
}