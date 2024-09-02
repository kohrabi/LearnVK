#include "first_app.hpp"

#include <stdexcept>
#include <array>

namespace Engine {
	FirstApp::FirstApp() {
		loadModels();
		createPipelineLayout();
		createPipeline();
		createCommandBuffers();
	}

	FirstApp::~FirstApp() {
		vkDestroyPipelineLayout(engineDevice.device(), pipelineLayout, nullptr);
	}

	void FirstApp::run() {
		while (!engineWindow.ShouldClose()) {
			glfwPollEvents();
			drawFrame();
		}

		vkDeviceWaitIdle(engineDevice.device());
	}

	void FirstApp::loadModels() {
		
		std::vector<EngineModel::Vertex> vertices(0);
		createSierpinskiTriangle(vertices, 1, { 0.0f, -0.5f }, { -0.5f, 0.5f }, { 0.5f, 0.5f });

		engineModel = std::make_unique<EngineModel>(engineDevice, vertices);
	}

	void FirstApp::createSierpinskiTriangle(
		std::vector<EngineModel::Vertex>& vertices, 
		int depth, 
		glm::vec2 top, 
		glm::vec2 left, 
		glm::vec2 right)
	{
		// OMG I WASTED TOO LONG NOT DOING A FOR LOOP
		if (depth <= 0) {
			vertices.push_back({ top });
			vertices.push_back({ right });
			vertices.push_back({ left });
		}
		else {
			glm::vec2 leftMid = (top + left) * 0.5f;
			glm::vec2 rightMid = (top + right) * 0.5f;
			glm::vec2 bottomMid = (left + right) * 0.5f;
			createSierpinskiTriangle(vertices, depth - 1, top, leftMid, rightMid);
			createSierpinskiTriangle(vertices, depth - 1, rightMid, bottomMid, right);
			createSierpinskiTriangle(vertices, depth - 1, leftMid, left, bottomMid);
		}
		// depth = 3;
		// i = 1
		// j = 0
		// powerOfTwo = 2^1 = 2 / 2 = 1
		
		/*
		// aint gonna lie
		// this is kinda retarded
		// this doesn't work
		float depthPowerOfTwo = pow(2, depth);
		int powerCount = 0;
		for (int i = 0; i < pow(2, depth - 1); i++) {
			if (((i + 1) & ((i + 1) - 1)) == 0) { // Is power of two
				powerCount = 0;
				for (int j = 0; j < i + 1; j++) {
					float xPos = -i / depthPowerOfTwo + 2 * j / depthPowerOfTwo;
					vertices.push_back({{ xPos, -0.5f + ((i) * 2 / depthPowerOfTwo) }});
					vertices.push_back({{ xPos + (1 / depthPowerOfTwo), -0.5f + ((i + 1) * 2 / depthPowerOfTwo) }});
					vertices.push_back({{ xPos - (1 / depthPowerOfTwo), -0.5f + ((i + 1) * 2 / depthPowerOfTwo) }});
				}
			}
			else {
				powerCount++;
				for (int j = 0; j < powerCount; j++) {
					if (((powerCount + 1) & ((powerCount + 1) - 1)) == 0 && j == 1) {
						continue;
					}
					float xPos = -i / depthPowerOfTwo + 2 * j / depthPowerOfTwo;
					vertices.push_back({ { xPos, -0.5f + ((i) * 2 / depthPowerOfTwo) } });
					vertices.push_back({ { xPos + (1 / depthPowerOfTwo), -0.5f + ((i + 1) * 2 / depthPowerOfTwo) } });
					vertices.push_back({ { xPos - (1 / depthPowerOfTwo), -0.5f + ((i + 1) * 2 / depthPowerOfTwo) } });

					float leftXPos = -i / depthPowerOfTwo + 2 * (i - j) / depthPowerOfTwo;
					
					vertices.push_back({ { leftXPos, -0.5f + ((i) * 2 / depthPowerOfTwo) } });
					vertices.push_back({ { leftXPos + (1 / depthPowerOfTwo), -0.5f + ((i + 1) * 2 / depthPowerOfTwo) } });
					vertices.push_back({ { leftXPos - (1 / depthPowerOfTwo), -0.5f + ((i + 1) * 2 / depthPowerOfTwo) } });
				}
			}
		}
		*/
	}

	void FirstApp::createPipelineLayout() {
		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};

		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = 0;
		pipelineLayoutInfo.pSetLayouts = nullptr; // Pass datas ex: Textures, uniform buffers
		pipelineLayoutInfo.pushConstantRangeCount = 0;
		pipelineLayoutInfo.pPushConstantRanges = nullptr; // efficiently send small datas to shaders
		if (vkCreatePipelineLayout(engineDevice.device(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
			throw std::runtime_error("Failed to create pipeline layout!");
		}
		
	}

	void FirstApp::createPipeline() {
		PipelineConfigInfo pipelineConfig{};
		EnginePipeline::DefaultPipelineConfigInfo(pipelineConfig, engineSwapChain.width(), engineSwapChain.height());

		pipelineConfig.renderPass = engineSwapChain.getRenderPass();
		pipelineConfig.pipelineLayout = pipelineLayout;
		enginePipeline = std::make_unique<EnginePipeline>(
			engineDevice,
			"shaders/simple_shader.vert.spv",
			"shaders/simple_shader.frag.spv",
			pipelineConfig
		);
	}
	
	void FirstApp::createCommandBuffers() {
		commandBuffers.resize(engineSwapChain.imageCount());

		VkCommandBufferAllocateInfo allocateInfo{};
		allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocateInfo.commandPool = engineDevice.getCommandPool();
		allocateInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

		if (vkAllocateCommandBuffers(engineDevice.device(), &allocateInfo, commandBuffers.data()) != VK_SUCCESS) {
			throw std::runtime_error("Failed to allocate command buffers");
		}

		for (int i = 0; i < commandBuffers.size(); i++) {
			VkCommandBufferBeginInfo beginInfo{};
			beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			if (vkBeginCommandBuffer(commandBuffers[i], &beginInfo) != VK_SUCCESS) {
				throw std::runtime_error("Failed to begin recording command buffer!");
			}
			
			VkRenderPassBeginInfo renderPassInfo{};
			renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
			renderPassInfo.renderPass = engineSwapChain.getRenderPass();
			renderPassInfo.framebuffer = engineSwapChain.getFrameBuffer(i);

			renderPassInfo.renderArea.offset = { 0,0 };
			renderPassInfo.renderArea.extent = engineSwapChain.getSwapChainExtent();

			std::array<VkClearValue, 2> clearValues{};
			clearValues[0].color = { 0.1f, 0.1f, 0.1f, 1.0f };
			clearValues[1].depthStencil = { 1.0f, 0 };
			renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
			renderPassInfo.pClearValues = clearValues.data();
		
			vkCmdBeginRenderPass(commandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

			enginePipeline->Bind(commandBuffers[i]);

			engineModel->Bind(commandBuffers[i]);
			engineModel->Draw(commandBuffers[i]);

			vkCmdEndRenderPass(commandBuffers[i]);

			if (vkEndCommandBuffer(commandBuffers[i]) != VK_SUCCESS) {
				throw std::runtime_error("Failed to record command buffer");
			}
		}
	}
	
	void FirstApp::drawFrame() {
		uint32_t imageIndex;
		auto result = engineSwapChain.acquireNextImage(&imageIndex);

		if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
			throw std::runtime_error("Failed to acquire swap chain image!");
		}
		result = engineSwapChain.submitCommandBuffers(&commandBuffers[imageIndex], &imageIndex);
		if (result != VK_SUCCESS) {
			throw std::runtime_error("Failed to present swap chain image!");
			
		}
	}
}