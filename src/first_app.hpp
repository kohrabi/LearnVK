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