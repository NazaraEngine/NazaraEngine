// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequisites.hpp

#pragma once

#ifndef NDK_SYSTEMS_DEBUGSYSTEM_HPP
#define NDK_SYSTEMS_DEBUGSYSTEM_HPP

#include <Nazara/Graphics/InstancedRenderable.hpp>
#include <Nazara/Utility/IndexBuffer.hpp>
#include <Nazara/Utility/Mesh.hpp>
#include <Nazara/Utility/VertexBuffer.hpp>
#include <NDK/ClientPrerequisites.hpp>
#include <NDK/System.hpp>

namespace Ndk
{
	class NDK_CLIENT_API DebugSystem : public System<DebugSystem>
	{
		public:
			DebugSystem();
			~DebugSystem() = default;

			void EnableDepthBuffer(bool enable);

			inline bool IsDepthBufferEnabled() const;

			static SystemIndex systemIndex;

		private:
			Nz::InstancedRenderableRef GenerateBox(Nz::Boxf box);
			Nz::InstancedRenderableRef GenerateCollision2DMesh(Entity* entity, Nz::Vector3f* offset);
			Nz::InstancedRenderableRef GenerateCollision3DMesh(Entity* entity);

			std::pair<Nz::IndexBufferRef, Nz::VertexBufferRef> GetBoxMesh();
			Nz::MaterialRef GetCollisionMaterial();
			Nz::MaterialRef GetGlobalAABBMaterial();
			Nz::MaterialRef GetLocalAABBMaterial();
			Nz::MaterialRef GetOBBMaterial();

			void OnEntityValidation(Entity* entity, bool justAdded) override;
			void OnUpdate(float elapsedTime) override;

			Nz::MaterialRef m_globalAabbMaterial;
			Nz::MaterialRef m_localAabbMaterial;
			Nz::MaterialRef m_collisionMaterial;
			Nz::MaterialRef m_obbMaterial;
			Nz::IndexBufferRef m_boxMeshIndexBuffer;
			Nz::VertexBufferRef m_boxMeshVertexBuffer;
			bool m_isDepthBufferEnabled;
	};
}

#include <NDK/Systems/DebugSystem.inl>

#endif // NDK_SYSTEMS_DEBUGSYSTEM_HPP
