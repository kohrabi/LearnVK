#pragma once

#include "engine_game_object.hpp"
#include "engine_device.hpp"
#include "engine_pipeline.hpp"
#include "engine_frame_info.hpp"
#include "engine_model.hpp"													
#include "engine_camera.hpp"

#include <memory>
#include <vector>


namespace Engine {


	class SimpleRenderSystem {
	public:
		SimpleRenderSystem(EngineDevice& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout);
		~SimpleRenderSystem();
		SimpleRenderSystem(const SimpleRenderSystem&) = delete;
		SimpleRenderSystem& operator=(const SimpleRenderSystem&) = delete;

		void RenderGameObjects(FrameInfo& frameInfo, std::vector<EngineGameObject> & gameObjects);

	private:
		void createPipelineLayout(VkDescriptorSetLayout globalSetlayout);
		void createPipeline(VkRenderPass renderPass);

		EngineDevice& engineDevice;
		std::unique_ptr<EnginePipeline> enginePipeline;
		VkPipelineLayout pipelineLayout;
	};
}