#pragma once

#include "engine_camera.hpp"
#include "engine_game_object.hpp"

#include <vulkan/vulkan.h>

namespace Engine {

	#define MAX_LIGHTS 10
	struct PointLight{
		glm::vec4 position{};
		glm::vec4 color{};
	};

	struct GlobalUbo {
		glm::mat4 projection{ 1.0f };
		glm::mat4 view{ 1.0f };
		glm::mat4 inverseViewMatrix { 1.0f };
		glm::vec4 ambientLightColor;
		PointLight pointLights[MAX_LIGHTS];
		int numLights;
	};

	struct FrameInfo {
		int frameIndex;
		float frameTime;
		VkCommandBuffer commandBuffer;
		EngineCamera& camera;
		VkDescriptorSet globalDescriptorSet;
		EngineGameObject::Map& gameObjects;
	};
}