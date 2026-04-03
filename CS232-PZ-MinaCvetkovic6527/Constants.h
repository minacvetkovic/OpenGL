#pragma once
#include "CollisionUtils.h"

namespace GameConstants {
	constexpr float PICKUPS_SPAWN_INTERVAL = 4.0f;
	constexpr int PICKUP_VALUE = 50;
	constexpr float PLAYER_SPEED = 70.0f;
	constexpr float BACKGROUND_SPEED = 50.0f;
	constexpr float PLAYER_UPPER_POSITION = 80.0f;
	constexpr float PLAYER_LOWER_POSITION = -10.0f;
	constexpr float PICKUP_UPPER_POSITION = 80.0f;
	constexpr float PICKUP_LOWER_POSITION = 0.0f;
	constexpr float SPAWN_MARGIN = 40.0f;
	constexpr int MAX_SPAWN_ATTEMPTS = 40;
	constexpr int MAX_ACTIVE_PICKUPS = 5;
	constexpr float LEVEL_THRESHOLD = 40.0f;
}

namespace PlayerOffsets {
	const AABB FACING_RIGHT = { 35, 0, -7, -80 };
	const AABB FACING_LEFT = { 7, 0, -35, -80 };
}

namespace PickupOffsets {
	const AABB DEFAULT = { 0, 0, 0, 0 };
}