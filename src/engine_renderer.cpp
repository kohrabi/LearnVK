#include "engine_renderer.hpp"

#include <stdexcept>
#include <array>


namespace Engine {

	EngineRenderer::EngineRenderer(EngineWindow& window, EngineDevice& device)
		: engineWindow(window), engineDevice(device) {
		recreateSwapchain();
		createCommandBuffers();
	}

	EngineRenderer::~EngineRenderer() { 
		freeCommandBuffers();
	}

	void EngineRenderer::recreateSwapchain() {
		auto extent = engineWindow.GetExtent();
		while (extent.width == 0 || extent.height == 0) {
			extent = engineWindow.GetExtent();
			glfwWaitEvents();
		}

		vkDeviceWaitIdle(engineDevice.device());
		if (engineSwapChain == nullptr)
			engineSwapChain = std::make_unique<EngineSwapChain>(engineDevice, extent);
		else {
			std::shared_ptr<EngineSwapChain> oldSwapChain = std::move(engineSwapChain);
			engineSwapChain = std::make_unique<EngineSwapChain>(engineDevice, extent, oldSwapChain);

			if (!oldSwapChain->CompareSwapFormats(*engineSwapChain.get())) {
				throw std::runtime_error("Swapchain image(or depth) format has changed!");
			}

		}
	}


	void EngineRenderer::createCommandBuffers() {
		commandBuffers.resize(EngineSwapChain::MAX_FRAMES_IN_FLIGHT);

		VkCommandBufferAllocateInfo allocateInfo{};
		allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocateInfo.commandPool = engineDevice.getCommandPool();
		allocateInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

		if (vkAllocateCommandBuffers(engineDevice.device(), &allocateInfo, commandBuffers.data()) != VK_SUCCESS) {
			throw std::runtime_error("Failed to allocate command buffers");
		}

	}

	void EngineRenderer::freeCommandBuffers() {
		vkFreeCommandBuffers(
			engineDevice.device(),
			engineDevice.getCommandPool(),
			static_cast<uint32_t>(commandBuffers.size()),
			commandBuffers.data());

		commandBuffers.clear();

	}

	VkCommandBuffer EngineRenderer::BeginFrame() {
		assert(!isFrameStarted && "Can't call begin frame while already in progress");

		auto result = engineSwapChain->acquireNextImage(&currentImageIndex);

		if (result == VK_ERROR_OUT_OF_DATE_KHR) {
			recreateSwapchain();
			return nullptr;
		}

		if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
			throw std::runtime_error("Failed to acquire swap chain image!");
		}

		isFrameStarted = true;
		auto commandBuffer = GetCurrentCommandBuffer();

		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
			throw std::runtime_error("Failed to begin recording command buffer!");
		}

		return commandBuffer;
	}

	void EngineRenderer::EndFrame() {
		assert(isFrameStarted && "Can't call end frame while frame is not in progress");

		auto commandBuffer = GetCurrentCommandBuffer();

		if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
			throw std::runtime_error("Failed to record command buffer");
		}

		auto result = engineSwapChain->submitCommandBuffers(&commandBuffer, &currentImageIndex);

		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || engineWindow.WasWindowResized()) {
			engineWindow.ResetWindowResizedFlag();
			recreateSwapchain();
		}
		else if (result != VK_SUCCESS) {
			throw std::runtime_error("Failed to present swap chain image!");
		}

		isFrameStarted = false;
		currentFrameIndex = (currentFrameIndex + 1) % EngineSwapChain::MAX_FRAMES_IN_FLIGHT;
	}

	void EngineRenderer::BeginSwapChainRenderPass(VkCommandBuffer commandBuffer) {
		assert(isFrameStarted && "Can't call begin BeginSwapChainRenderPass while frame is not in progress");
		assert(commandBuffer == GetCurrentCommandBuffer() && "Can't begin render pass on command buffer from a different frame");

		VkRenderPassBeginInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = engineSwapChain->getRenderPass();
		renderPassInfo.framebuffer = engineSwapChain->getFrameBuffer(currentImageIndex);

		renderPassInfo.renderArea.offset = { 0,0 };
		renderPassInfo.renderArea.extent = engineSwapChain->getSwapChainExtent();

		std::array<VkClearValue, 2> clearValues{};
		clearValues[0].color = { 0.01f, 0.01f, 0.01f, 1.0f };
		clearValues[1].depthStencil = { 1.0f, 0 };
		renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
		renderPassInfo.pClearValues = clearValues.data();

		vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = static_cast<float>(engineSwapChain->getSwapChainExtent().width);
		viewport.height = static_cast<float>(engineSwapChain->getSwapChainExtent().height);
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		VkRect2D scissor{ {0, 0}, engineSwapChain->getSwapChainExtent() };
		vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
		vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

	}

	void EngineRenderer::EndSwapChainRenderPass(VkCommandBuffer commandBuffer)
	{
		assert(isFrameStarted && "Can't call begin EndSwapChainRenderPass while frame is not in progress");
		assert(commandBuffer == GetCurrentCommandBuffer() && "Can't end render pass on command buffer from a different frame");
		vkCmdEndRenderPass(commandBuffer);
	}
}