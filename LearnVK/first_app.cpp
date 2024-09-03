#include "first_app.hpp"
#include "simple_render_system.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <stdexcept>
#include <array>


namespace Engine {

	FirstApp::FirstApp() {
		loadGameObjects();
	}

	FirstApp::~FirstApp() {
	}

	void FirstApp::run() {
		SimpleRenderSystem simpleRenderSystem{ engineDevice, engineRenderer.GetSwapChainRenderPass() };

		while (!engineWindow.ShouldClose()) {
			glfwPollEvents();

			if (auto commandBuffer = engineRenderer.BeginFrame()) {
				
				// begin offscreen shadow passs
				// render shadow casting objects
				// end offscreen shadow pass

				engineRenderer.BeginSwapChainRenderPass(commandBuffer);

				simpleRenderSystem.RenderGameObjects(commandBuffer, gameObjects);
				engineRenderer.EndSwapChainRenderPass(commandBuffer);
				engineRenderer.EndFrame();

			}
		}

		//vkDeviceWaitIdle(engineDevice.device());
	}

	void FirstApp::loadGameObjects() {
		
		std::vector<glm::vec3> colors{
			{1.f, .7f, .73f},
			{1.f, .87f, .73f},
			{1.f, 1.f, .73f},
			{.73f, 1.f, .8f},
			{.73, .88f, 1.f}  //
		};


		//for (auto& color : colors) {
		//	color = glm::pow(color, glm::vec3{ 2.2f });
		//}
		std::vector<EngineModel::Vertex> vertices(0);
		createSierpinskiTriangle(vertices, 0, { 0.0f, -0.5f }, { -0.5f, 0.5f }, { 0.5f, 0.5f });

		auto model = std::make_shared<EngineModel>(engineDevice, vertices);
		for (int i = 0; i < 1; i++) {
			auto triangle = EngineGameObject::CreateGameObject();
			triangle.model = model;
			triangle.transform2D.scale = glm::vec2(.5f) + i * 0.025f;
			//triangle.transform2D.rotation = i * glm::pi<float>() * .025f;
			triangle.color = colors[i % colors.size()];
			gameObjects.push_back(std::move(triangle));
		}
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
			vertices.push_back({ top, glm::vec3(1.0f, 0.0f, 0.0f) });
			vertices.push_back({ right, glm::vec3(0.0f, 1.0f, 0.0f) });
			vertices.push_back({ left, glm::vec3(0.0f, 0.0f, 1.0f) });
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
}