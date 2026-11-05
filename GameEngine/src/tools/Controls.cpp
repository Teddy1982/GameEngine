#include "Controls.h"

Controls::Controls() {
    // Erstellung einer Steuerungkonfigurationsdatei, falls noch nicht vorhanden
    filename = "data/controls.txt";
    if (!std::filesystem::exists(filename)) {
        if (!std::filesystem::exists("data")) {
            std::filesystem::create_directory("data");
        }
        std::ofstream ofs(filename);
        if (!ofs) {
            Logger::log(2, "error while creating '%s'\n", filename.c_str());
        }

        saveBindings();
    }

    loadBindings();
}

// gibt die Tastenbelegung zurück
const KeyBindings& Controls::getKeyBindings() const {
    return keyMap;
}

// überschreibt die Tastenbelegung
void Controls::setKeyBindings(const KeyBindings& bindings) {
    keyMap = bindings;
}

// gibt Bezeichnung einer Taste anhand ihres Key-Codes zurück
const char* Controls::getKeyDisplayName(int key) {
    const char* name = glfwGetKeyName(key, 0);
    if (name) return name;

    switch (key) {
    case GLFW_KEY_UP: return "<PFEIL_OBEN>";
    case GLFW_KEY_DOWN: return "<PFEIL_UNTEN>";
    case GLFW_KEY_LEFT: return "<PFEIL_LINKS>";
    case GLFW_KEY_RIGHT: return "<PFEIL_RECHTS>";
    case GLFW_KEY_SPACE: return "<LEERTASTE>";
    case GLFW_KEY_ENTER: return "<EINGABETASTE>";
    case GLFW_KEY_ESCAPE: return "<ESC>";
    case GLFW_KEY_F1: return "<F1>";
    case GLFW_KEY_F2: return "<F2>";
    case GLFW_KEY_F3: return "<F3>";
    case GLFW_KEY_F4: return "<F4>";
    case GLFW_KEY_F5: return "<F5>";
    case GLFW_KEY_F6: return "<F6>";
    case GLFW_KEY_F7: return "<F7>";
    case GLFW_KEY_F8: return "<F8>";
    case GLFW_KEY_F9: return "<F9>";
    case GLFW_KEY_F10: return "<F10>";
    case GLFW_KEY_F11: return "<F11>";
    case GLFW_KEY_F12: return "<F12>";
    case GLFW_KEY_INSERT: return "<EINFG>";
    case GLFW_KEY_DELETE: return "<ENTF>";
    case GLFW_KEY_HOME: return "<POS1>";
    case GLFW_KEY_PAGE_UP: return "<BILD ^>";
    case GLFW_KEY_PAGE_DOWN: return "<BILD v>";
    case GLFW_KEY_TAB: return "<TAB>";
    case GLFW_KEY_CAPS_LOCK: return "<FESTSTELLTASTE>";
    case GLFW_KEY_LEFT_SHIFT: return "<LINKS_UMSCHALTTASTE>";
    case GLFW_KEY_RIGHT_SHIFT: return "<RECHTS_UMSCHALTTASTE>";
    case GLFW_KEY_LEFT_CONTROL: return "<LINKS_STRG>";
    case GLFW_KEY_RIGHT_CONTROL: return "<RECHTS_STRG>";
    case GLFW_KEY_LEFT_ALT: return "<LINKS_ALT>";
    case GLFW_KEY_RIGHT_ALT: return "<RECHTS_ALT_GR>";
    case GLFW_KEY_BACKSPACE: return "<RUECKTASTE>";
    case GLFW_KEY_NUM_LOCK: return "<NUM_SPERRE>";
    case GLFW_KEY_KP_0: return "<NUM_BLOCK_0>";
    case GLFW_KEY_KP_1: return "<NUM_BLOCK_1>";
    case GLFW_KEY_KP_2: return "<NUM_BLOCK_2>";
    case GLFW_KEY_KP_3: return "<NUM_BLOCK_3>";
    case GLFW_KEY_KP_4: return "<NUM_BLOCK_4>";
    case GLFW_KEY_KP_5: return "<NUM_BLOCK_5>";
    case GLFW_KEY_KP_6: return "<NUM_BLOCK_6>";
    case GLFW_KEY_KP_7: return "<NUM_BLOCK_7>";
    case GLFW_KEY_KP_8: return "<NUM_BLOCK_8>";
    case GLFW_KEY_KP_9: return "<NUM_BLOCK_9>";
    case GLFW_KEY_KP_DECIMAL: return "<NUM_BLOCK_DEZIMAL>";
    case GLFW_KEY_KP_DIVIDE: return "<NUM_BLOCK_/>";
    case GLFW_KEY_KP_MULTIPLY: return "<NUM_BLOCK_*>";
    case GLFW_KEY_KP_SUBTRACT: return "<NUM_BLOCK_->";
    case GLFW_KEY_KP_ADD: return "<NUM_BLOCK_+>";
    case GLFW_KEY_KP_ENTER: return "<NUM_BLOCK_ENTER>";
    case GLFW_KEY_KP_EQUAL: return "<NUM_BLOCK_=>";
    default: return "???";
    }
}

// lädt Tastenbelegung aus Steuerungkonfigurationsdatei
void Controls::loadBindings() {
    std::vector<int> entries;
    std::ifstream file(filename);
    std::string line;

    while (std::getline(file, line)) {
        std::istringstream iss(line);
        int entry;
        entry = std::stoi(line);
        entries.push_back(entry);
    }

    KeyBindings bindings;
    bindings.cameraUp = entries[0];
    bindings.cameraDown = entries[1];
    bindings.cameraLeft = entries[2];
    bindings.cameraRight = entries[3];
    bindings.cameraForward = entries[4];
    bindings.cameraBackward = entries[5];
    bindings.up = entries[6];
    bindings.down = entries[7];
    bindings.left = entries[8];
    bindings.right = entries[9];
    bindings.forward = entries[10];
    bindings.backward = entries[11];
    bindings.action = entries[12];
    bindings.applicationMode = entries[13];
    bindings.appModeEdit = entries[14];
    bindings.toggleFullscreen = entries[15];
    bindings.showControlsHelpRequest = entries[16];
    bindings.instanceEditModeMove = entries[17];
    bindings.instanceEditModeRotate = entries[18];
    bindings.instanceEditModeScale = entries[19];
    bindings.undoLastOperation = entries[20];
    bindings.redoLastOperation = entries[21];
    bindings.newConfigRequest = entries[22];
    bindings.loadConfigRequest = entries[23];
    bindings.saveConfigRequest = entries[24];
    bindings.exitApplication = entries[25];
    bindings.instanceMoveYAxisWhenMouseIsMoved_1 = entries[26];
    bindings.instanceMoveYAxisWhenMouseIsMoved_2 = entries[27];
    bindings.prevSelectedCamera = entries[28];
    bindings.nextSelectedCamera = entries[29];
    bindings.zoomUpMovement = entries[30];
    bindings.speedUpMovement_1 = entries[31];
    bindings.speedUpMovement_2 = entries[32];
    bindings.moveStateRoll = entries[33];
    bindings.moveStatePunch = entries[34];
    bindings.moveStateKick = entries[35];
    bindings.moveStateWave = entries[36];
    bindings.moveStateInteract = entries[37];
    bindings.moveStatePick = entries[38];
    bindings.moveStateJumpOrHop = entries[39];

    setKeyBindings(bindings);
}

// speichert Tastenbelegung in Steuerungkonfigurationsdatei
void Controls::saveBindings() {
    std::ofstream file(filename, std::ios::trunc);
    file << keyMap.cameraUp << "\n";
    file << keyMap.cameraDown << "\n";
    file << keyMap.cameraLeft << "\n";
    file << keyMap.cameraRight << "\n";
    file << keyMap.cameraForward << "\n";
    file << keyMap.cameraBackward << "\n";
    file << keyMap.up << "\n";
    file << keyMap.down << "\n";
    file << keyMap.left << "\n";
    file << keyMap.right << "\n";
    file << keyMap.forward << "\n";
    file << keyMap.backward << "\n";
    file << keyMap.action << "\n";
    file << keyMap.applicationMode << "\n";
    file << keyMap.appModeEdit << "\n";
    file << keyMap.toggleFullscreen << "\n";
    file << keyMap.showControlsHelpRequest << "\n";
    file << keyMap.instanceEditModeMove << "\n";
    file << keyMap.instanceEditModeRotate << "\n";
    file << keyMap.instanceEditModeScale << "\n";
    file << keyMap.undoLastOperation << "\n";
    file << keyMap.redoLastOperation << "\n";
    file << keyMap.newConfigRequest << "\n";
    file << keyMap.loadConfigRequest << "\n";
    file << keyMap.saveConfigRequest << "\n";
    file << keyMap.exitApplication << "\n";
    file << keyMap.instanceMoveYAxisWhenMouseIsMoved_1 << "\n";
    file << keyMap.instanceMoveYAxisWhenMouseIsMoved_2 << "\n";
    file << keyMap.prevSelectedCamera << "\n";
    file << keyMap.nextSelectedCamera << "\n";
    file << keyMap.zoomUpMovement << "\n";
    file << keyMap.speedUpMovement_1 << "\n";
    file << keyMap.speedUpMovement_2 << "\n";
    file << keyMap.moveStateRoll << "\n";
    file << keyMap.moveStatePunch << "\n";
    file << keyMap.moveStateKick << "\n";
    file << keyMap.moveStateWave << "\n";
    file << keyMap.moveStateInteract << "\n";
    file << keyMap.moveStatePick << "\n";
    file << keyMap.moveStateJumpOrHop << "\n";

    file.close();
}
