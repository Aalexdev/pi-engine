#pragma once

#include "../../core.hpp"
#include "../ECS.hpp"
#include "../Entity.hpp"
#include "../../dataStruct/Set.hpp"

namespace engine{
	class Scene;
}

namespace engine::ECS::systems{
	class ENGINE_API Sprite : public System{
		public:
			Sprite(Scene *scene) : scene{scene}{}
			virtual void init() override;

			std::unordered_map<uint16_t, std::list<engine::Entity>> sort(std::set<uint16_t> &ids);
		private:
			Scene *scene;
	};
}