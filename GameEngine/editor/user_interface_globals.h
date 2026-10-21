#pragma once

#include "../Editor/hierarchy.h"
#include "../Editor/inspector.h"
#include "../Editor/project.h"
#include "../Editor/enums.h"

extern std::vector<std::pair<std::vector<std::string>, FileType>> fileDataTypes;

extern HierarchyWindow hierarchyWindow;
extern InspectorWindow inspectorWindow;
extern ProjectWindow projectWindow;