#pragma once

#include "engine_device.hpp"
#include "engine_window.hpp"
#include "engine_swap_chain.hpp"
#include "engine_model.hpp"

#include <memory>
#include <vector>


namespace Engine {


	class EngineRenderer {
	public:
		EngineRenderer(EngineWindow& window, EngineDevice& device);
		~EngineRenderer();
		EngineRenderer(const EngineRenderer&) = delete;
		EngineRenderer& operator=(const EngineRenderer&) = delete;

		VkRenderPass GetSwapChainRenderPass() const { return engineSwapChain->getRenderPass(); }
		bool IsFrameInProgress() const { return isFrameStarted; }
		float GetAspectRatio() const { return engineSwapChain->extentAspectRatio(); }
		VkCommandBuffer GetCurrentCommandBuffer() const {
			assert(isFrameStarted && "Cannot get command buffer when frame is not in progress");
			return commandBuffers[currentFrameIndex];
		}

		int GetFrameIndex() const { 
			assert(isFrameStarted && "Cannot get command buffer when frame is not in progress");
			return currentFrameIndex; 
		}


		VkCommandBuffer	BeginFrame();
		void EndFrame();
		void BeginSwapChainRenderPass(VkCommandBuffer commandBuffer);
		void EndSwapChainRenderPass(VkCommandBuffer commandBuffer);

	private:
		void createCommandBuffers();
		void freeCommandBuffers();
		void recreateSwapchain();

		EngineWindow& engineWindow;
		EngineDevice& engineDevice;
		std::unique_ptr<EngineSwapChain> engineSwapChain;
		std::vector<VkCommandBuffer> commandBuffers;

		uint32_t currentImageIndex;
		int currentFrameIndex{ 0 };
		bool isFrameStarted{ false };
	};
}