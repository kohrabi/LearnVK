#pragma once

#include "engine_game_object.hpp"
#include "engine_device.hpp"
#include "engine_pipeline.hpp"
#include "engine_model.hpp"													

#include <memory>
#include <vector>


namespace Engine {


	class SimpleRenderSystem {
	public:
		SimpleRenderSystem(EngineDevice& device, VkRenderPass renderPass);
		~SimpleRenderSystem();
		SimpleRenderSystem(const SimpleRenderSystem&) = delete;
		SimpleRenderSystem& operator=(const SimpleRenderSystem&) = delete;

		void RenderGameObjects(VkCommandBuffer commandBuffer, std::vector<EngineGameObject> & gameObjects);

	private:
		void createPipelineLayout();
		void createPipeline(VkRenderPass renderPass);

		EngineDevice& engineDevice;
		std::unique_ptr<EnginePipeline> enginePipeline;
		VkPipelineLayout pipelineLayout;
	};
}