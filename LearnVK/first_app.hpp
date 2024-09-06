#pragma once

#include "engine_game_object.hpp"
#include "engine_device.hpp"
#include "engine_window.hpp"
#include "engine_model.hpp"
#include "engine_renderer.hpp"														
#include "engine_descriptors.hpp"

#include <memory>
#include <vector>


namespace Engine {
	#define MAX_LIGHTS 10

	struct PointLight {
		glm::vec4 position;
		glm::vec4 color; // w is intensity
	};

	struct GlobalUbo {
		alignas(16) glm::mat4 projectionView{ 1.0f };
		glm::vec4 ambientLightColor{ 0.0f, 0.f, 8.f, .02f }; // w is intensity
		glm::vec3 lightPosition{ -1.f };
		alignas(16) glm::vec4 lightColor{ 1.f };  // w is light intensity
	};

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
		void loadGameObjects();

		EngineWindow engineWindow{ WIDTH, HEIGHT, "Hello Vulkan!" };
		EngineDevice engineDevice{ engineWindow };
		EngineRenderer engineRenderer{ engineWindow, engineDevice };

		std::unique_ptr<EngineDescriptorPool> globalPool{};
		EngineGameObject::Map gameObjects;
	};
}