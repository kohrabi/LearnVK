#pragma once

#include "engine_model.hpp"

#include <memory>

namespace Engine {
	struct Transform2DComponent {
		glm::vec2 translations{}; // position offset
		glm::vec2 scale{ 1.0f, 1.0f };
		float rotation;

		glm::mat2 mat2() {
			const float s = glm::sin(rotation);
			const float c = glm::cos(rotation);

			glm::mat2 rotMat{ {c, s}, {-s, c} };

			glm::mat2 scaleMat{ { scale.x, 0.0f }, { 0, scale.y } };
			return rotMat * scaleMat;
		}
	};

	class EngineGameObject {
	public:
		using id_t = unsigned int;

		static EngineGameObject CreateGameObject() {
			static id_t currentId = 0;
			return EngineGameObject{currentId++};
		}

		EngineGameObject(const EngineGameObject&) = delete;
		EngineGameObject& operator= (const EngineGameObject&) = delete;
		EngineGameObject(EngineGameObject&&) = default;
		EngineGameObject& operator= (EngineGameObject&&) = default;

		id_t GetId() const { return id; }
		std::shared_ptr<EngineModel> model{};
		glm::vec3 color{};
		Transform2DComponent transform2D;

	private:
		EngineGameObject(id_t objId) : id (objId) {}

	private:
		id_t id;
	};
}