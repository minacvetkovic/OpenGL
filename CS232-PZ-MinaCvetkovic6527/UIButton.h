#pragma once
#include "CollisionUtils.h"

struct UIButton {
public:
	std::unique_ptr<GameObject> button;
	AABB box;
	bool active = true;
};