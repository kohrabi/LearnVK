#pragma once

#include "engine_game_object.hpp"
#include "engine_device.hpp"
#include "engine_pipeline.hpp"
#include "engine_frame_info.hpp"
#include "engine_model.hpp"													
#include "engine_camera.hpp"
#include "first_app.hpp"

#include <memory>
#include <vector>


namespace Engine {


	class PointLightSystem {
	public:
		PointLightSystem(EngineDevice& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout);
		~PointLightSystem();
		PointLightSystem(const PointLightSystem&) = delete;
		PointLightSystem& operator=(const PointLightSystem&) = delete;

		void update(FrameInfo& frameInfo, GlobalUbo& ubo); 
		void render(FrameInfo& frameInfo);

	private:
		void createPipelineLayout(VkDescriptorSetLayout globalSetlayout);
		void createPipeline(VkRenderPass renderPass);

		EngineDevice& engineDevice;
		std::unique_ptr<EnginePipeline> enginePipeline;
		VkPipelineLayout pipelineLayout;
	};
}