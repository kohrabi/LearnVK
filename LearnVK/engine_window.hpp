#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <string>

namespace Engine {

	class EngineWindow {
	public:
		EngineWindow(int w, int h, std::string name);
		~EngineWindow();

		
		EngineWindow(const EngineWindow&) = delete;
		EngineWindow& operator=(const EngineWindow&) = delete;

		bool ShouldClose() { return glfwWindowShouldClose(window); }
		VkExtent2D GetExtent() { return { static_cast<uint32_t>(width), static_cast<uint32_t>(height) }; }

		void CreateWindowSurface(VkInstance instance, VkSurfaceKHR* surface);

	private:

		void initWindow();
		
		const int width;
		const int height;
			
		std::string windowName;
		GLFWwindow* window;
	};
}