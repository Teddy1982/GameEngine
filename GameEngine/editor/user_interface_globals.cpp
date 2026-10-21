#include "user_interface_globals.h"

#include <string>
#include <vector>

std::vector<std::pair<std::vector<std::string>, FileType>> fileDataTypes = {
	{{".h", ".hpp", ".cpp"},FileType::SCRIPT },
	{{".gltf", ".glb"},FileType::MODEL },
	{{".bmp", ".png"},FileType::TEXTURE },
	{{".wav", ".mp3"},FileType::AUDIO }
};

HierarchyWindow hierarchyWindow;
InspectorWindow inspectorWindow;
ProjectWindow projectWindow;