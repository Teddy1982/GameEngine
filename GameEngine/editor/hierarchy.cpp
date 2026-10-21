#include "hierarchy.h"
#include "imgui.h"

#include <algorithm>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <iterator>
#include <utility>

#include "user_interface_globals.h"
#include "../tools/Logger.h"

void HierarchyWindow::addGameObject(GameObject gameObject) {
    static std::uint64_t nextId = 1;

    gameObject.instance->id = nextId;
    gameObject.id = nextId++;
    gameObjects.push_back(std::move(gameObject));
}

#include <string>

std::size_t HierarchyWindow::cloneGameObject(
    std::uint64_t sourceId,
    std::size_t count
) {
    if (count == 0 ||
        !modInstCamData ||
        !modInstCamData->micInstanceCloneCallbackFunction) {
        return 0;
    }

    const auto sourceIt = std::find_if(
        gameObjects.begin(),
        gameObjects.end(),
        [sourceId](const GameObject& object) {
            return object.id == sourceId;
        }
    );

    if (sourceIt == gameObjects.end() || !sourceIt->instance) {
        Logger::log(1, "HierarchyWindow: source GameObject not found\n");
        return 0;
    }

    // Vor dem Hinzufügen kopieren, da gameObjects dadurch wachsen kann.
    const GameObject source = *sourceIt;

    const auto& currentInstances = modInstCamData->micAssimpInstances;
    if (std::find(
        currentInstances.begin(),
        currentInstances.end(),
        source.instance
    ) == currentInstances.end()) {
        Logger::log(1, "HierarchyWindow: source instance no longer exists\n");
        return 0;
    }

    std::size_t createdCount = 0;

    for (std::size_t i = 0; i < count; ++i) {
        // Der Callback legt die Kopie an und wählt sie aus.
        modInstCamData->micInstanceCloneCallbackFunction(source.instance);

        const int newIndex = modInstCamData->micSelectedInstance;
        const auto& instances = modInstCamData->micAssimpInstances;

        if (newIndex < 0 ||
            static_cast<std::size_t>(newIndex) >= instances.size()) {
            Logger::log(1, "HierarchyWindow: clone callback selected an invalid instance\n");
            continue;
        }

        const auto newInstance =
            instances[static_cast<std::size_t>(newIndex)];

        if (!newInstance || newInstance == source.instance) {
            Logger::log(1, "HierarchyWindow: clone callback did not create a new instance\n");
            continue;
        }

        GameObject clone = source;
        clone.instance = newInstance;
        clone.name = source.name;

        addGameObject(std::move(clone));
        ++createdCount;
    }

    return createdCount;
}

void HierarchyWindow::createFrame() {
    static char renameBuffer[128] = "";

    // Auswahl und Umbenennen an stabile IDs binden.
    static std::uint64_t selectedId = 0;
    static std::uint64_t renameId = 0;

    if (selectedIndex == -1) {
        selectedId = 0;
    }

    bool openRenamePopup = false;

    std::uint64_t pendingDeleteId = 0;
    pendingCloneId = 0;
    std::uint64_t pendingDragSource = 0;
    std::uint64_t pendingDragTarget = 0;

    auto findObject = [&](std::uint64_t id) {
        return std::find_if(
            gameObjects.begin(),
            gameObjects.end(),
            [id](const GameObject& object) {
                return object.id == id;
            }
        );
        };

    // Der aktuelle Index wird gesucht, nicht dauerhaft gespeichert.
    auto findInstanceIndex = [&](const GameObject& object) -> int {
        if (!modInstCamData || !object.instance) {
            return -1;
        }

        const auto& instances = modInstCamData->micAssimpInstances;

        const auto it = std::find(
            instances.begin(),
            instances.end(),
            object.instance
        );

        if (it == instances.end()) {
            return -1;
        }

        return static_cast<int>(
            std::distance(instances.begin(), it)
            );
        };

    auto selectObject = [&](const GameObject& object) {
        selectedId = object.id;

        inspectorWindow.showData(object.name, FileType::MODEL);

        if (modInstCamData) {
            modInstCamData->micSelectedInstance =
                findInstanceIndex(object);
        }
        };

    ImGui::Begin("Hierarchy");
    ImGui::BeginChild("##HierarchyChild");

    ImGui::Text("Gameobjects");
    ImGui::Separator();

    for (std::size_t i = 0; i < gameObjects.size(); ++i) {
        const auto& object = gameObjects[i];

        const std::string label =
            "(" + std::to_string(i) + "): " + object.name +
            "###GameObject_" + std::to_string(object.id);

        if (ImGui::Selectable(
            label.c_str(),
            selectedId == object.id)) {
            selectedIndex = i;
        }

        if (isHighlighting && selectedIndex == i) {
            selectObject(object);
        }
        else {
            modInstCamData->micSelectedInstance = -1;
        }

        if (ImGui::BeginDragDropSource()) {
            const std::uint64_t sourceId = object.id;

            ImGui::SetDragDropPayload(
                "HIERARCHY_ITEM",
                &sourceId,
                sizeof(sourceId)
            );

            ImGui::Text("%s", object.name.c_str());
            ImGui::EndDragDropSource();
        }

        if (ImGui::BeginDragDropTarget()) {
            if (const ImGuiPayload* payload =
                ImGui::AcceptDragDropPayload("HIERARCHY_ITEM")) {

                if (payload->DataSize ==
                    static_cast<int>(sizeof(std::uint64_t))) {

                    std::memcpy(
                        &pendingDragSource,
                        payload->Data,
                        sizeof(pendingDragSource)
                    );

                    pendingDragTarget = object.id;
                }
            }

            ImGui::EndDragDropTarget();
        }

        if (ImGui::BeginPopupContextItem()) {
            selectObject(object);

            if (ImGui::MenuItem("Umbenennen")) {
                std::snprintf(
                    renameBuffer,
                    sizeof(renameBuffer),
                    "%s",
                    object.name.c_str()
                );

                renameId = object.id;
                openRenamePopup = true;
            }

            if (ImGui::MenuItem("Duplizieren")) {
                pendingCloneId = object.id;
            }

            if (ImGui::MenuItem("Loeschen")) {
                pendingDeleteId = object.id;
            }

            ImGui::EndPopup();
        }
    }

    // Löschen nach der Darstellung.
    if (pendingDeleteId != 0) {
        auto object = findObject(pendingDeleteId);

        if (!modInstCamData ||
            !modInstCamData->micInstanceDeleteCallbackFunction) {

            Logger::log(
                1,
                "HierarchyWindow: delete callback is not initialized\n"
            );
        }
        else if (object != gameObjects.end()) {
            if (findInstanceIndex(*object) < 0) {
                Logger::log(
                    1,
                    "HierarchyWindow: instance no longer exists\n"
                );
            }
            else {
                // Kopie behalten, während der Callback den Vektor verändert.
                auto instance = object->instance;

                modInstCamData->micInstanceDeleteCallbackFunction(
                    instance,
                    true
                );

                const auto& instances =
                    modInstCamData->micAssimpInstances;

                const bool removed =
                    std::find(
                        instances.begin(),
                        instances.end(),
                        instance
                    ) == instances.end();

                if (removed) {
                    if (selectedId == pendingDeleteId) {
                        selectedId = 0;
                        const auto model = object->instance->getModel();

                        const auto instanceCount = std::count_if(
                            gameObjects.begin(),
                            gameObjects.end(),
                            [&](const GameObject& gameObject) {
                                return gameObject.instance &&
                                    gameObject.instance->getModel() == model;
                            }
                        );

                        if (instanceCount == 1) {
                            modInstCamData->micModelDeleteCallbackFunction(object->instance->getModel()->getModelFileNamePath(), true);
                        }
                        
                    }
                    gameObjects.erase(object);

                    selectedIndex = -1;
                    inspectorWindow.showData("", FileType::UNDEFINED);

                    if (renameId == pendingDeleteId) {
                        renameId = 0;
                    }
                }
                else {
                    Logger::log(
                        1,
                        "HierarchyWindow: callback did not remove instance\n"
                    );
                }
            }
        }
    }

    if (pendingCloneId != 0) {
        cloneGameObject(pendingCloneId, 1);
    }

    // Drag-and-drop verändert nur die Hierarchiereihenfolge.
    if (pendingDragSource != 0 &&
        pendingDragTarget != 0 &&
        pendingDragSource != pendingDragTarget) {

        auto source = findObject(pendingDragSource);
        auto target = findObject(pendingDragTarget);

        if (source != gameObjects.end() &&
            target != gameObjects.end()) {

            const auto sourceIndex =
                std::distance(gameObjects.begin(), source);

            auto targetIndex =
                std::distance(gameObjects.begin(), target);

            GameObject item = std::move(*source);
            gameObjects.erase(source);

            if (sourceIndex < targetIndex) {
                --targetIndex;
            }

            // Einfügen vor dem Zielobjekt.
            gameObjects.insert(
                gameObjects.begin() + targetIndex,
                std::move(item)
            );
        }
    }

    // Renderer-Auswahl nach Änderungen am Instanzvektor nachführen.
    if (modInstCamData) {
        const auto selected = findObject(selectedId);

        modInstCamData->micSelectedInstance =
            (isHighlighting && selected != gameObjects.end())
            ? findInstanceIndex(*selected)
            : -1;
    }

    if (openRenamePopup) {
        ImGui::OpenPopup("Umbenennen");
    }

    if (ImGui::BeginPopupModal(
        "Umbenennen",
        nullptr,
        ImGuiWindowFlags_AlwaysAutoResize)) {

        ImGui::Text("Neuen Namen eingeben:");
        ImGui::InputText(
            "##Name",
            renameBuffer,
            sizeof(renameBuffer)
        );

        ImGui::Separator();

        if (ImGui::Button("OK")) {
            const auto object = findObject(renameId);

            if (object != gameObjects.end()) {
                object->name = renameBuffer;

                if (selectedId == object->id) {
                    inspectorWindow.showData(
                        object->name,
                        FileType::MODEL
                    );
                }
            }

            renameId = 0;
            ImGui::CloseCurrentPopup();
        }

        ImGui::SameLine();

        if (ImGui::Button("Abbrechen")) {
            renameId = 0;
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }

    ImGui::EndChild();
    ImGui::End();
}
