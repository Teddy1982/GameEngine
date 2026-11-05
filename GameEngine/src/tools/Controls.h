#pragma once
#include <string>
#include <fstream>
#include <vector>
#include <algorithm>
#include <sstream>
#include <filesystem>

#include "Logger.h"

#include <GLFW/glfw3.h>
#include <unordered_map>

// Strukturen für Tastensteuerung

struct RebindContext {
    bool awaitingKey = false;
    std::string actionName;
    int* targetKey = nullptr;
};

struct InputState {
    bool cameraUp = false;
    bool cameraDown = false;
    bool cameraLeft = false;
    bool cameraRight = false;
    bool cameraForward = false;
    bool cameraBackward = false;
    bool moveUp = false;
    bool moveDown = false;
    bool moveLeft = false;
    bool moveRight = false;
    bool moveForward = false;
    bool moveBackward = false;
    bool action = false;

    bool applicationMode;
    bool appModeEdit;
    bool toggleFullscreen;
    bool showControlsHelpRequest;
    bool instanceEditModeMove;
    bool instanceEditModeRotate;
    bool instanceEditModeScale;
    bool undoLastOperation;
    bool redoLastOperation;
    bool newConfigRequest;
    bool loadConfigRequest;
    bool saveConfigRequest;
    bool exitApplication;
    bool instanceMoveYAxisWhenMouseIsMoved_1;
    bool instanceMoveYAxisWhenMouseIsMoved_2;
    bool prevSelectedCamera;
    bool nextSelectedCamera;
    bool zoomUpMovement;
    bool speedUpMovement_1;
    bool speedUpMovement_2;
    
    bool moveStateRoll;
    bool moveStatePunch;
    bool moveStateKick ;
    bool moveStateWave;
    bool moveStateInteract;
    bool moveStatePick;
    bool moveStateJumpOrHop;
};

struct KeyBindings {
    int cameraUp = GLFW_KEY_KP_9;
    int cameraDown = GLFW_KEY_KP_7;
    int cameraLeft = GLFW_KEY_KP_4;
    int cameraRight = GLFW_KEY_KP_6;
    int cameraForward = GLFW_KEY_KP_8;
    int cameraBackward = GLFW_KEY_KP_5;

    int up = GLFW_KEY_E;
    int down = GLFW_KEY_Q;
    int left = GLFW_KEY_A;
    int right = GLFW_KEY_D;
    int forward = GLFW_KEY_W;
    int backward = GLFW_KEY_S;

    int action = GLFW_KEY_SPACE;

    int applicationMode = GLFW_KEY_F10;
    int appModeEdit = GLFW_KEY_ESCAPE;
    int toggleFullscreen = GLFW_KEY_F11;
    int showControlsHelpRequest = GLFW_KEY_F1;
    int instanceEditModeMove = GLFW_KEY_1;
    int instanceEditModeRotate = GLFW_KEY_2;
    int instanceEditModeScale = GLFW_KEY_3;
    int undoLastOperation = GLFW_KEY_Z;
    int redoLastOperation = GLFW_KEY_Y;
    int newConfigRequest = GLFW_KEY_N;
    int loadConfigRequest = GLFW_KEY_L;
    int saveConfigRequest = GLFW_KEY_S;
    int exitApplication = GLFW_KEY_Q;
    int instanceMoveYAxisWhenMouseIsMoved_1 = GLFW_KEY_LEFT_SHIFT;
    int instanceMoveYAxisWhenMouseIsMoved_2 = GLFW_KEY_RIGHT_SHIFT;
    int prevSelectedCamera = GLFW_KEY_LEFT_BRACKET;
    int nextSelectedCamera = GLFW_KEY_RIGHT_BRACKET;
    int zoomUpMovement = GLFW_KEY_LEFT_SHIFT;
    int speedUpMovement_1 = GLFW_KEY_LEFT_SHIFT;
    int speedUpMovement_2 = GLFW_KEY_RIGHT_SHIFT;

    int moveStateRoll = GLFW_KEY_R;
    int moveStatePunch = GLFW_KEY_E;
    int moveStateKick = GLFW_KEY_Q;
    int moveStateWave = GLFW_KEY_F;
    int moveStateInteract = GLFW_KEY_U;
    int moveStatePick = GLFW_KEY_P;
    int moveStateJumpOrHop = GLFW_KEY_SPACE;
};

//Klasse für Funktionen rundum die Tastensteuerung mit Speichern- und Ladenmöglichkeit
class Controls {
public:
    Controls();

    //void keyCallback(int key, int action);
    //void clearFrameTriggers();
    const KeyBindings& getKeyBindings() const;
    void setKeyBindings(const KeyBindings& bindings);
    //const InputState& getInputState();
    const char* getKeyDisplayName(int key);
    void update(GLFWwindow* window);

    void loadBindings();
    void saveBindings();

private:
    KeyBindings keyMap;
    InputState state;

    std::string filename;
};