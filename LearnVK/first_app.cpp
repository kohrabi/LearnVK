#include "first_app.hpp"
#include "simple_render_system.hpp"
#include "engine_camera.hpp"
#include "engine_buffer.hpp"
#include "keyboard_movement_controller.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <stdexcept>
#include <array>
#include <chrono>


namespace Engine {

	struct GlobalUbo {
		alignas(16) glm::mat4 projectionView{ 1.0f };
		alignas(16) glm::vec3 lightDirection = glm::normalize(glm::vec3(1.0f, 3.0f, -1.0f));
	};

	FirstApp::FirstApp() {
		globalPool = EngineDescriptorPool::Builder(engineDevice)
			.setMaxSets(EngineSwapChain::MAX_FRAMES_IN_FLIGHT)
			.addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, EngineSwapChain::MAX_FRAMES_IN_FLIGHT)
			.build();
		loadGameObjects();
	}

	FirstApp::~FirstApp() {
	}

	void FirstApp::run() {
		std::vector<std::unique_ptr<EngineBuffer> > uboBuffers{ EngineSwapChain::MAX_FRAMES_IN_FLIGHT };
		for (int i = 0; i < uboBuffers.size(); i++) {
			uboBuffers[i] = std::make_unique<EngineBuffer>(
				engineDevice,
				sizeof(GlobalUbo),
				1,
				VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
			);
			uboBuffers[i]->map();
		}

		auto globalSetLayout = EngineDescriptorSetLayout::Builder(engineDevice)
			.addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT)
			.build();

		std::vector<VkDescriptorSet> globalDescriptorSets(EngineSwapChain::MAX_FRAMES_IN_FLIGHT);
		for (int i = 0; i < globalDescriptorSets.size(); i++) {
			auto bufferInfo = uboBuffers[i]->descriptorInfo();
			EngineDescriptorWriter(*globalSetLayout, *globalPool)
				.writeBuffer(0, &bufferInfo)
				.build(globalDescriptorSets[i]);
		}

		SimpleRenderSystem simpleRenderSystem{ 
			engineDevice, 
			engineRenderer.GetSwapChainRenderPass(), 
			globalSetLayout->getDescriptorSetLayout()};
		EngineCamera camera{};
		camera.SetViewTarget(glm::vec3{ -1.f, -2.f, -20.f }, glm::vec3{ 0.0f, 0.0f, 2.5f });

		auto viewerObject = EngineGameObject::CreateGameObject();
		KeyboardMovementController cameraController{};

		auto currentTime = std::chrono::high_resolution_clock::now();

		while (!engineWindow.ShouldClose()) {
			glfwPollEvents();

			auto newTime = std::chrono::high_resolution_clock::now();
			float frameTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
			currentTime = newTime;
			cameraController.MoveInPlaneXZ(engineWindow.GetGLFWWindow(), frameTime, viewerObject);
			camera.SetViewYXZ(viewerObject.transform.translation, viewerObject.transform.rotation);

			float aspect = engineRenderer.GetAspectRatio();
			camera.SetPerspectiveProjection(glm::radians(50.f), aspect, 0.1f, 100.f);

			if (auto commandBuffer = engineRenderer.BeginFrame()) {
			
				int frameIndex = engineRenderer.GetFrameIndex();
				FrameInfo frameInfo{
					frameIndex,
					frameTime,
					commandBuffer,
					camera,
					globalDescriptorSets[frameIndex]
				};
				// Update
				GlobalUbo ubo{};
				ubo.projectionView = camera.GetProjection() * camera.GetView();
				uboBuffers[frameIndex]->writeToBuffer(&ubo);
				uboBuffers[frameIndex]->flush();

					
				// Render

				engineRenderer.BeginSwapChainRenderPass(commandBuffer);

				simpleRenderSystem.RenderGameObjects(frameInfo, gameObjects);
				engineRenderer.EndSwapChainRenderPass(commandBuffer);
				engineRenderer.EndFrame();

			}
		}

		vkDeviceWaitIdle(engineDevice.device());
	}

	void FirstApp::loadGameObjects() {
		std::shared_ptr < EngineModel > vaseModel = EngineModel::CreateModelFromFile(engineDevice, "models\\smooth_vase.obj");
		auto vase = EngineGameObject::CreateGameObject();
		vase.model = vaseModel;
		vase.transform.translation = { 0.0f, 0.5f, 2.5f };
		vase.transform.scale = glm::vec3(3.f, 1.5f, 3.f);
		gameObjects.push_back(std::move(vase));

		std::shared_ptr < EngineModel > flatVaseModel = EngineModel::CreateModelFromFile(engineDevice, "models\\flat_vase.obj");
		auto flatVase = EngineGameObject::CreateGameObject();
		flatVase.model = flatVaseModel;
		flatVase.transform.translation = { 0.5f, 0.5f, 2.5f };
		flatVase.transform.scale = glm::vec3(3.f, 1.5f, 3.f);
		gameObjects.push_back(std::move(flatVase));
	}

}