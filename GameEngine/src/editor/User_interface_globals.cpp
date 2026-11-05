#include "User_interface_globals.h"

#include <string>
#include <vector>

Controls gControls;
RebindContext rebindContext;

std::vector<std::pair<std::vector<std::string>, FileType>> fileDataTypes = {
	{{".h", ".hpp", ".cpp"},FileType::SCRIPT },
	{{".gltf",".glb",".obj",".fbx",".dae",".mdl",".md3",".pk3"},FileType::MODEL },
	{{".bmp", ".png"},FileType::TEXTURE },
	{{".wav", ".mp3"},FileType::AUDIO }
};

HierarchyWindow hierarchyWindow;
InspectorWindow inspectorWindow;
ProjectWindow projectWindow;