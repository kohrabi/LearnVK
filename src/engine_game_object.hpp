#pragma once

#include "engine_model.hpp"

#include <glm/gtc/matrix_transform.hpp>

#include <memory>
#include <unordered_map>

namespace Engine {
	struct PointLightComponent {
		float lightIntensity = 1.0f;
	};

    struct TransformComponent {
        glm::vec3 translation{}; // position offset
        glm::vec3 scale{ 1.0f, 1.0f, 1.0f };
        glm::vec3 rotation{};


        // Matrix corrsponds to Translate * Ry * Rx * Rz * Scale
        // Rotations correspond to Tait-bryan angles of Y(1), X(2), Z(3)
        // https://en.wikipedia.org/wiki/Euler_angles#Rotation_matrix
		glm::mat4 mat4();
		glm::mat3 normalMatrix();

	};

	class EngineGameObject {
	public:
		using id_t = unsigned int;
		using Map = std::unordered_map<id_t, EngineGameObject>;

		static EngineGameObject CreateGameObject() {
			static id_t currentId = 0;
			return EngineGameObject{currentId++};
		}
		static EngineGameObject CreatePointLight(
			float intensity = 10.f, 
			float radius = 0.1f, 
			glm::vec3 color = glm::vec3(1.0f));

		EngineGameObject(const EngineGameObject&) = delete;
		EngineGameObject& operator= (const EngineGameObject&) = delete;
		EngineGameObject(EngineGameObject&&) = default;
		EngineGameObject& operator= (EngineGameObject&&) = default;

		id_t GetId() const { return id; }
		std::shared_ptr<EngineModel> model{};
		glm::vec3 color{};
		TransformComponent transform;

		// Optional
		std::unique_ptr<PointLightComponent> pointLight = nullptr;

	private:
		EngineGameObject(id_t objId) : id (objId) {}

	private:
		id_t id;
	};
}