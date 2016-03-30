#include <Nazara/Renderer/RenderWindow.hpp>
#include <NDK/Components/NodeComponent.hpp>
#include <NDK/Components/CameraComponent.hpp>
#include <NDK/EntityHandle.hpp>
#include <NDK/Systems/RenderSystem.hpp>
#include <NDK/World.hpp>

namespace Tet {
	namespace Utils {
		inline bool InitializeGame(Nz::RenderWindow& window, Ndk::World& world) {
			world.GetSystem<Ndk::RenderSystem>().SetGlobalUp(Nz::Vector3f::Down());

			Ndk::EntityHandle view{ world.CreateEntity() };
			view->AddComponent<Ndk::NodeComponent>();
			auto& cam = view->AddComponent<Ndk::CameraComponent>();
			cam.SetProjectionType(Nz::ProjectionType_Orthogonal); // 2D
			cam.SetTarget(&window);

            return true;
		}
	}
}