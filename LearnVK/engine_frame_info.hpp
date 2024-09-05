#pragma once

#include "engine_camera.hpp"

#include <vulkan/vulkan.h>

namespace Engine {
	struct FrameInfo {
		int frameIndex;
		float frameTime;
		VkCommandBuffer commandBuffer;
		EngineCamera& camera;
		VkDescriptorSet globalDescriptorSet;
	};
}