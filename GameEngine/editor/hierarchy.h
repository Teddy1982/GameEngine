#pragma once

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>
#include "../gameobject/gameObject.h"
#include "../vulkan/ModelInstanceCamData.h"

class HierarchyWindow {
public:

	void setModelInstanceCamData(ModelInstanceCamData& data) {
		modInstCamData = &data;
	}

	void addGameObject(GameObject gameObject);

	std::size_t cloneGameObject(std::uint64_t sourceId, std::size_t count);
	void createFrame();

	std::vector<GameObject> gameObjects;

	bool isHighlighting = true;
	int selectedIndex = -1;

	std::uint64_t pendingCloneId = 0;

private:

	ModelInstanceCamData* modInstCamData = nullptr;

};