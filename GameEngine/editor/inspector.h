/* Dear ImGui */
#pragma once
#include <string>
#include <vector>

#include <imgui.h>

#include "../vulkan/VkRenderData.h"
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

#include "enums.h"

class InspectorWindow {
public:

    void showData(std::string filename, FileType type);

    bool init(VkRenderData& rData);

    void setModelInstanceCamData(ModelInstanceCamData& data) {
        modInstCamData = &data;
    }

    void createFrame();
    void resetPositionWindowOctreeView();

    std::string selGameObject = "";
    std::string selDataType = "";

    float mFramesPerSecond = 0.0f;
    /* averaging speed */
    float mAveragingAlpha = 0.96f;

    std::vector<float> mFPSValues{};
    int mNumFPSValues = 90;

    std::vector<float> mFrameTimeValues{};
    int mNumFrameTimeValues = 90;

    std::vector<float> mModelUploadValues{};
    int mNumModelUploadValues = 90;

    std::vector<float> mMatrixGenerationValues{};
    int mNumMatrixGenerationValues = 90;

    std::vector<float> mMatrixUploadValues{};
    int mNumMatrixUploadValues = 90;

    std::vector<float> mMatrixDownloadValues{};
    int mNumMatrixDownloadValues = 90;

    std::vector<float> mUiGenValues{};
    int mNumUiGenValues = 90;

    std::vector<float> mUiDrawValues{};
    int mNumUiDrawValues = 90;

    std::vector<float> mCollisionDebugDrawValues{};
    int mNumCollisionDebugDrawValues = 90;

    std::vector<float> mCollisionCheckValues{};
    int mNumCollisionCheckValues = 90;

    std::vector<float> mNumCollisionsValues{};
    int mNumNumCollisionValues = 90;

    std::vector<float> mBehaviorManagerValues{};
    int mNumBehaviorManagerValues = 90;

    std::vector<float> mInteractionValues{};
    int mNumInteractionValues = 90;

    std::vector<float> mFaceAnimValues{};
    int mNumFaceAnimValues = 90;

    std::vector<float> mLevelCollisionCheckValues{};
    int mNumLevelCollisionCheckValues = 90;

    std::vector<float> mIKValues{};
    int mNumIKValues = 90;

    std::vector<float> mLevelGroundNeighborUpdateValues{};
    int mNumLevelGroundNeighborUpdateValues = 90;

    std::vector<float> mPathFindingValues{};
    int mNumPathFindingValues = 90;

    float mNewFps = 0.0f;
    double mUpdateTime = 0.0;

    int mFpsOffset = 0;
    int mFrameTimeOffset = 0;
    int mModelUploadOffset = 0;
    int mMatrixGenOffset = 0;
    int mMatrixUploadOffset = 0;
    int mMatrixDownloadOffset = 0;
    int mUiGenOffset = 0;
    int mUiDrawOffset = 0;
    int mCollisionDebugDrawOffset = 0;
    int mCollisionCheckOffset = 0;
    int mNumCollisionOffset = 0;
    int mBehaviorManagerOffset = 0;
    int mInteractionOffset = 0;
    int mFaceAnimOffset = 0;
    int mLevelCollisionOffset = 0;
    int mIkOffset = 0;
    int mLevelGroundNeighborOffset = 0;
    int mPathFindingOffset = 0;

    int mManyInstanceCreateNum = 1;
    int mManyInstanceCloneNum = 1;

    InstanceSettings mSavedInstanceSettings{};
    std::shared_ptr<AssimpInstance> mCurrentInstance = nullptr;
    std::shared_ptr<AssimpModel> mCurrentModel = nullptr;

    CameraSettings mSavedCameraSettings{};
    std::shared_ptr<Camera> mCurrentCamera = nullptr;
    bool mShowDuplicateCamNameDialog = false;

    std::vector<std::string> mBoneNames{};

    std::string mNewTreeName = "Tree1";
    std::string mSelectedTreeName;
    std::string mTreeToDelete;
    std::shared_ptr<SingleInstanceBehavior> mBehaviorManager = nullptr;

    bool mModelHasFaceAnims = false;

    std::shared_ptr<AssimpLevel> mCurrentLevel = nullptr;

    int mSelectedNavTarget = 0;

    int mCurrentPlaylistPos = 0;

    static int nameInputFilter(ImGuiInputTextCallbackData* data);

    VkLineMesh mOctreeLines{};
    float mOctreeZoomFactor = 1.0f;
    glm::vec3 mOctreeRotation = glm::vec3(0.0f);
    glm::vec3 mOctreeTranslation = glm::vec3(0.0f);

    glm::mat4 mScaleMat = glm::mat4(1.0f);
    glm::mat4 mRotationMat = glm::mat4(1.0f);
    glm::mat3 mOctreeViewMat = glm::mat3(1.0f);

    VkRenderData* renderData = nullptr;
    ModelInstanceCamData* modInstCamData = nullptr;

private:

    bool InputFloatWithArrows(const char* label, float* value, float step = 0.1f)
    {
        bool changed = false;

        // Gibt jedem Feld eigene ImGui-IDs, auch für die ArrowButtons.
        ImGui::PushID(label);

        ImGui::AlignTextToFramePadding();
        ImGui::TextUnformatted(label);
        ImGui::SameLine();

        ImGui::SetNextItemWidth(120.0f);
        changed |= ImGui::InputFloat("##value", value, 0.0f, 0.0f, "%.3f");

        ImGui::SameLine(0.0f, 2.0f);
        if (ImGui::ArrowButton("##up", ImGuiDir_Up)) {
            *value += step;
            changed = true;
        }

        ImGui::SameLine(0.0f, 2.0f);
        if (ImGui::ArrowButton("##down", ImGuiDir_Down)) {
            *value -= step;
            changed = true;
        }

        ImGui::PopID();
        return changed;
    }
/*
    void debugCheckAabbPoint(
        const char* name,
        const glm::vec3& localPoint,
        const glm::mat4& worldTransform,
        const AABB& worldAabb)
    {
        const glm::vec3 worldPoint =
            glm::vec3(worldTransform * glm::vec4(localPoint, 1.0f));

        const glm::vec3 minPos = worldAabb.getMinPos();
        const glm::vec3 maxPos = worldAabb.getMaxPos();

        constexpr float epsilon = 1e-3f;
        const glm::vec3 e(epsilon);

        const bool inside =
            glm::all(glm::greaterThanEqual(worldPoint, minPos - e)) &&
            glm::all(glm::lessThanEqual(worldPoint, maxPos + e));

        Logger::log(1,
            "%s lokal=(%.3f, %.3f, %.3f), Welt=(%.3f, %.3f, %.3f), "
            "innerhalb=%s\n",
            name,
            static_cast<double>(localPoint.x),
            static_cast<double>(localPoint.y),
            static_cast<double>(localPoint.z),
            static_cast<double>(worldPoint.x),
            static_cast<double>(worldPoint.y),
            static_cast<double>(worldPoint.z),
            inside ? "ja" : "NEIN");
    }
*/
};
