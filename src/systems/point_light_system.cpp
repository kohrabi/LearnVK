#include "point_light_system.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <stdexcept>
#include <array>
#include <map>

namespace Engine {

	struct PointLightPushConstants{
		glm::vec4 position{};
		glm::vec4 color {};
		float radius;
	};

	PointLightSystem::PointLightSystem(EngineDevice& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetlayout)
		: engineDevice(device) {
		createPipelineLayout(globalSetlayout);
		createPipeline(renderPass);
	}

	PointLightSystem::~PointLightSystem() {
		vkDestroyPipelineLayout(engineDevice.device(), pipelineLayout, nullptr);
	}

	void PointLightSystem::createPipeline(VkRenderPass renderPass) {
		assert(pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout");

		PipelineConfigInfo pipelineConfig{};
		EnginePipeline::DefaultPipelineConfigInfo(pipelineConfig);
		EnginePipeline::EnableAlphaBlending(pipelineConfig);
		pipelineConfig.attributeDescriptions.clear();
		pipelineConfig.bindingDescriptions.clear();
		pipelineConfig.renderPass = renderPass;
		pipelineConfig.pipelineLayout = pipelineLayout;
		enginePipeline = std::make_unique<EnginePipeline>(
			engineDevice,
			"shaders/point_light.vert.spv",
			"shaders/point_light.frag.spv",
			pipelineConfig
		);
	}

	void PointLightSystem::update(FrameInfo& frameInfo, GlobalUbo& ubo) {
		int lightIndex = 0;
		auto rotateLight = glm::rotate(glm::mat4(1.f), frameInfo.frameTime, {0.0f, -1.0f, 0.0f});
		for (auto& kv : frameInfo.gameObjects) {
			auto& obj = kv.second;
			if (obj.pointLight == nullptr)
				continue;

			assert(lightIndex < MAX_LIGHTS && "Point lights exceed maximum specified");
			// obj.transform.translation = glm::vec3(rotateLight * glm::vec4(obj.transform.translation, 1.0f));

			ubo.pointLights[lightIndex].position = glm::vec4(obj.transform.translation, 1.0f);
			ubo.pointLights[lightIndex].color = glm::vec4(obj.color, obj.pointLight->lightIntensity);
			lightIndex++;
		}
		ubo.numLights = lightIndex;
	
	} 

	void PointLightSystem::render(FrameInfo& frameInfo) {
		std::map<float, EngineGameObject::id_t> sorted;
		for (auto& kv : frameInfo.gameObjects) {
			auto& obj = kv.second;
			if (obj.pointLight == nullptr) continue;
			// calculate distance
			auto offset = frameInfo.camera.GetPosition() - obj.transform.translation;
			float distSquared = glm::dot(offset, offset);
			sorted[distSquared] = obj.GetId();
		}


		enginePipeline->Bind(frameInfo.commandBuffer);

		vkCmdBindDescriptorSets(
			frameInfo.commandBuffer,
			VK_PIPELINE_BIND_POINT_GRAPHICS,
			pipelineLayout,
			0, 1,
			&frameInfo.globalDescriptorSet,
			0, nullptr
		);

		// iterate through sorted lights in reverse order
		for (auto it = sorted.rbegin(); it != sorted.rend(); it++) {
			auto& obj = frameInfo.gameObjects.at(it->second);
			PointLightPushConstants push{};
			push.position = glm::vec4(obj.transform.translation, 1.0f);
			push.color = glm::vec4(obj.color, obj.pointLight-> lightIntensity);
			push.radius = obj.transform.scale.x;
			vkCmdPushConstants(
				frameInfo.commandBuffer, 
				pipelineLayout,
				VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 
				0, 
				sizeof(PointLightPushConstants), 
				&push);
			vkCmdDraw(frameInfo.commandBuffer, 6, 1, 0, 0);
		}
	}

	void PointLightSystem::createPipelineLayout(VkDescriptorSetLayout globalSetlayout) {

		VkPushConstantRange pushConstantRange{};
		pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
		pushConstantRange.offset = 0;
		pushConstantRange.size = sizeof(PointLightPushConstants);

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