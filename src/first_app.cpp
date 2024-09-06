#include "first_app.hpp"
#include "systems/simple_render_system.hpp"
#include "systems/point_light_system.hpp"
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
			.addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS)
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
		
		PointLightSystem pointLightSystem{
			engineDevice,
			engineRenderer.GetSwapChainRenderPass(),
			globalSetLayout->getDescriptorSetLayout() };
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
					globalDescriptorSets[frameIndex],
					gameObjects
				};
				// Update
				GlobalUbo ubo{};
				ubo.projection = camera.GetProjection();
				ubo.view = camera.GetView();
				ubo.inverseViewMatrix = camera.GetInverseView();
				pointLightSystem.update(frameInfo, ubo);
				uboBuffers[frameIndex]->writeToBuffer(&ubo);
				uboBuffers[frameIndex]->flush();

					
				// Render

				engineRenderer.BeginSwapChainRenderPass(commandBuffer);

				// Render solid first, transperant next
				simpleRenderSystem.RenderGameObjects(frameInfo);
				pointLightSystem.render(frameInfo);

				engineRenderer.EndSwapChainRenderPass(commandBuffer);
				engineRenderer.EndFrame();

			}
		}

		vkDeviceWaitIdle(engineDevice.device());
	}

	void FirstApp::loadGameObjects() {
		std::shared_ptr < EngineModel > engineModel = EngineModel::CreateModelFromFile(engineDevice, "models\\smooth_vase.obj");
		auto vase = EngineGameObject::CreateGameObject();
		vase.model = engineModel;
		vase.transform.translation = {-.5f, .5f, 0.f};
		vase.transform.scale = glm::vec3(3.f, 1.5f, 3.f);
		gameObjects.emplace(vase.GetId(), std::move(vase));

		engineModel = EngineModel::CreateModelFromFile(engineDevice, "models\\flat_vase.obj");
		auto flatVase = EngineGameObject::CreateGameObject();
		flatVase.model = engineModel;
		flatVase.transform.translation = {.5f, .5f, 0.f};
		flatVase.transform.scale = glm::vec3(3.f, 1.5f, 3.f);
		gameObjects.emplace(flatVase.GetId(), std::move(flatVase));

		engineModel = EngineModel::CreateModelFromFile(engineDevice, "models\\quad.obj");
		auto floor = EngineGameObject::CreateGameObject();
		floor.model = engineModel;
		floor.transform.translation = { 0.f, 0.5f, 0.f };
		floor.transform.scale = glm::vec3(3.f, 1.f, 3.f);
		gameObjects.emplace(floor.GetId(), std::move(floor));

		{
		}

		std::vector<glm::vec3> lightColors{
			{1.f, .1f, .1f},
			{.1f, .1f, 1.f},
			{.1f, 1.f, .1f},
			{1.f, 1.f, .1f},
			{.1f, 1.f, 1.f},
			{1.f, 1.f, 1.f}  //
		};

		for (int i = 0; i < lightColors.size(); i++) {

			auto pointLight = EngineGameObject::CreatePointLight(.2f, 0.1f, lightColors[i]);
			auto rotateLight = glm::rotate(glm::mat4(1.f), (i * glm::two_pi<float>()) / lightColors.size(), {0.0f, -1.0f, 0.0f});
			pointLight.transform.translation = glm::vec3(rotateLight * glm::vec4(-1.f, -1.f, -1.f, 1.f));						
			gameObjects.emplace(pointLight.GetId(), std::move(pointLight));

		}


	}

}