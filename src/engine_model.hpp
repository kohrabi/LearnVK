#pragma once

#include "engine_device.hpp"
#include "engine_buffer.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include <vector>
#include <memory>

namespace Engine {
	// Use to get vertex data from file and send it to GPU kinda
	class EngineModel {
	public:
		struct Vertex {
			glm::vec3 position{};
			glm::vec3 color{};
			glm::vec3 normal{};
			glm::vec2 uv{};

			static std::vector<VkVertexInputBindingDescription> GetBindingDescriptions();
			static std::vector<VkVertexInputAttributeDescription> GetAttributeDescriptions();

			bool operator==(const Vertex& other) const {
				return position == other.position &&
					color == other.color &&
					normal == other.normal &&
					uv == other.uv;
			}
		};

		struct Builder {
			std::vector<Vertex> vertices{};
			std::vector<uint32_t> indices{};

			void LoadModel(const std::string& filePath);
		};

		EngineModel(EngineDevice& device, const EngineModel::Builder& builder);
		~EngineModel();
		EngineModel(const EngineModel&) = delete;
		EngineModel& operator=(const EngineModel&) = delete;

		static std::unique_ptr<EngineModel> CreateModelFromFile(EngineDevice& device, const std::string& filePath);

		void Bind(VkCommandBuffer commandBuffer);
		void Draw(VkCommandBuffer commandBuffer);

	private:

		void createVertexBuffers(const std::vector<Vertex>& vertices);
		void createIndexBuffers(const std::vector<uint32_t>& indices); 

		EngineDevice& engineDevice;
		std::unique_ptr<EngineBuffer> vertexBuffer;
		uint32_t vertexCount;
		
		bool hasIndexBuffer = false;
		std::unique_ptr<EngineBuffer> indexBuffer;
		uint32_t indexCount;

	};
}