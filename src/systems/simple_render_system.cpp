#include "simple_render_system.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <stdexcept>
#include <array>


namespace Engine {

	struct SimplePushConstantData {
		glm::mat4 modelMatrix{ 1.0f };
		glm::mat4 normalMatrix{ 1.0f };
	};

	SimpleRenderSystem::SimpleRenderSystem(EngineDevice& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetlayout)
		: engineDevice(device) {
		createPipelineLayout(globalSetlayout);
		createPipeline(renderPass);
	}

	SimpleRenderSystem::~SimpleRenderSystem() {
		vkDestroyPipelineLayout(engineDevice.device(), pipelineLayout, nullptr);
	}

	void SimpleRenderSystem::createPipeline(VkRenderPass renderPass) {
		assert(pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout");

		PipelineConfigInfo pipelineConfig{};
		EnginePipeline::DefaultPipelineConfigInfo(pipelineConfig);

		pipelineConfig.renderPass = renderPass;
		pipelineConfig.pipelineLayout = pipelineLayout;
		enginePipeline = std::make_unique<EnginePipeline>(
			engineDevice,
			"shaders/simple_shader.vert.spv",
			"shaders/simple_shader.frag.spv",
			pipelineConfig
		);
	}

	void SimpleRenderSystem::RenderGameObjects(FrameInfo& frameInfo) {

		enginePipeline->Bind(frameInfo.commandBuffer);

		vkCmdBindDescriptorSets(
			frameInfo.commandBuffer,
			VK_PIPELINE_BIND_POINT_GRAPHICS,
			pipelineLayout,
			0, 1,
			&frameInfo.globalDescriptorSet,
			0, nullptr
		);

		for (auto& keyVal : frameInfo.gameObjects) {
			//obj.transform.rotation = glm::mod(obj.transform.rotation + 0.01f, glm::two_pi<float>());
			auto& obj = keyVal.second;
			if (obj.model == nullptr) continue;
			for (int j = 0; j < 4; j++) {
				SimplePushConstantData push{};
				push.modelMatrix = obj.transform.mat4();
				push.normalMatrix = obj.transform.normalMatrix();

				vkCmdPushConstants(
					frameInfo.commandBuffer,
					pipelineLayout,
					VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
					0,
					sizeof(SimplePushConstantData),
					&push);
			}
			obj.model->Bind(frameInfo.commandBuffer);
			obj.model->Draw(frameInfo.commandBuffer);

		}
	}

	void SimpleRenderSystem::createPipelineLayout(VkDescriptorSetLayout globalSetlayout) {

		VkPushConstantRange pushConstantRange{  };
		pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
		pushConstantRange.offset = 0;
		pushConstantRange.size = sizeof(SimplePushConstantData);

		std::vector<VkDescriptorSetLayout> descriptorSetLayouts{ globalSetlayout };

		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(descriptorSetLayouts.size());
		pipelineLayoutInfo.pSetLayouts = descriptorSetLayouts.data(); // Pass datas ex: Textures, uniform buffers
		pipelineLayoutInfo.pushConstantRangeCount = 1;
		pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange; // efficiently send small datas to shaders
		if (vkCreatePipelineLayout(engineDevice.device(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
			throw std::runtime_error("Failed to create pipeline layout!");
		}

	}
}