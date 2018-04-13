// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequisites.hpp

#pragma once

#ifndef NDK_SERVER
#ifndef NDK_SYSTEMS_DEBUGSYSTEM_HPP
#define NDK_SYSTEMS_DEBUGSYSTEM_HPP

#include <Nazara/Graphics/InstancedRenderable.hpp>
#include <Nazara/Utility/IndexBuffer.hpp>
#include <Nazara/Utility/Mesh.hpp>
#include <Nazara/Utility/VertexBuffer.hpp>
#include <NDK/System.hpp>

namespace Ndk
{
	class NDK_API DebugSystem : public System<DebugSystem>
	{
		public:
			DebugSystem();
			~DebugSystem() = default;

			static SystemIndex systemIndex;

		private:
			Nz::InstancedRenderableRef GenerateBox(Nz::Boxf box);
			Nz::InstancedRenderableRef GenerateCollision3DMesh(Entity* entity);

			Nz::MaterialRef GetAABBMaterial();
			Nz::MaterialRef GetCollisionMaterial();
			Nz::MaterialRef GetOBBMaterial();
			std::pair<Nz::IndexBufferRef, Nz::VertexBufferRef> GetBoxMesh();

			void OnEntityValidation(Entity* entity, bool justAdded) override;

			void OnUpdate(float elapsedTime) override;

			Nz::MaterialRef m_aabbMaterial;
			Nz::MaterialRef m_collisionMaterial;
			Nz::MaterialRef m_obbMaterial;
			Nz::IndexBufferRef m_boxMeshIndexBuffer;
			Nz::VertexBufferRef m_boxMeshVertexBuffer;
	};
}

#include <NDK/Systems/DebugSystem.inl>

#endif // NDK_SYSTEMS_DEBUGSYSTEM_HPP
#endif // NDK_SERVER
