#pragma once

#include "engine_device.hpp"
#include "engine_window.hpp"
#include "engine_pipeline.hpp"
#include "engine_swap_chain.hpp"
#include "engine_model.hpp"

#include <memory>
#include <vector>

namespace Engine {
	class FirstApp {
	public:
		static constexpr int WIDTH = 800;
		static constexpr int HEIGHT = 800;

		FirstApp();
		~FirstApp();
		FirstApp(const FirstApp&) = delete;
		FirstApp& operator=(const FirstApp&) = delete;

		void run();


	private:
		void createPipelineLayout();
		void createPipeline();
		void createCommandBuffers();
		void drawFrame();
		void loadModels();
		void createSierpinskiTriangle(
			std::vector<EngineModel::Vertex>& vertices,
			int depth,
			glm::vec2 top,
			glm::vec2 left,
			glm::vec2 right);

		EngineWindow engineWindow{ WIDTH, HEIGHT, "Hello Vulkan!" };
		EngineDevice engineDevice{ engineWindow };
		EngineSwapChain engineSwapChain{ engineDevice, engineWindow.GetExtent() };
		std::unique_ptr<EnginePipeline> enginePipeline;
		VkPipelineLayout pipelineLayout;
		std::vector<VkCommandBuffer> commandBuffers;
		std::unique_ptr<EngineModel> engineModel;
	};
}