#pragma once

#include "../Editor/Hierarchy.h"
#include "../Editor/Inspector.h"
#include "../Editor/Project.h"
#include "../tools/Controls.h"

extern Controls gControls;
extern RebindContext rebindContext;

extern std::vector<std::pair<std::vector<std::string>, FileType>> fileDataTypes;

extern HierarchyWindow hierarchyWindow;
extern InspectorWindow inspectorWindow;
extern ProjectWindow projectWindow;