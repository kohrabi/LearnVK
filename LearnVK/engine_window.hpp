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
		bool WasWindowResized() { return frameBufferResized; }

		void ResetWindowResizedFlag() { frameBufferResized = false; }

		void CreateWindowSurface(VkInstance instance, VkSurfaceKHR* surface);

	private:
		static void frameBufferResizeCallback(GLFWwindow* window, int width, int height);
		void initWindow();
		
		int width;
		int height;
		bool frameBufferResized = false;
			
		std::string windowName;
		GLFWwindow* window;
	};
}