#pragma once

#include "engine_camera.hpp"
#include "engine_game_object.hpp"

#include <vulkan/vulkan.h>

namespace Engine {
	struct FrameInfo {
		int frameIndex;
		float frameTime;
		VkCommandBuffer commandBuffer;
		EngineCamera& camera;
		VkDescriptorSet globalDescriptorSet;
		EngineGameObject::Map& gameObjects;
	};
}