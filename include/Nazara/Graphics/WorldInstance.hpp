// Copyright (C) 2021 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_GRAPHICS_WORLDINSTANCE_HPP
#define NAZARA_GRAPHICS_WORLDINSTANCE_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Graphics/Config.hpp>
#include <Nazara/Math/Matrix4.hpp>
#include <Nazara/Renderer/ShaderBinding.hpp>
#include <memory>

namespace Nz
{
	class AbstractBuffer;
	class CommandBufferBuilder;
	class MaterialSettings;
	class UploadPool;
	class WorldInstance;

	using WorldInstancePtr = std::shared_ptr<WorldInstance>;

	class NAZARA_GRAPHICS_API WorldInstance
	{
		public:
			WorldInstance();
			WorldInstance(const WorldInstance&) = delete;
			WorldInstance(WorldInstance&&) noexcept = default;
			~WorldInstance() = default;

			inline std::shared_ptr<AbstractBuffer>& GetInstanceBuffer();
			inline const std::shared_ptr<AbstractBuffer>& GetInstanceBuffer() const;
			inline const Matrix4f& GetInvWorldMatrix() const;
			inline const Matrix4f& GetWorldMatrix() const;

			void UpdateBuffers(UploadPool& uploadPool, CommandBufferBuilder& builder);
			inline void UpdateWorldMatrix(const Matrix4f& worldMatrix);
			inline void UpdateWorldMatrix(const Matrix4f& worldMatrix, const Matrix4f& invWorldMatrix);

			WorldInstance& operator=(const WorldInstance&) = delete;
			WorldInstance& operator=(WorldInstance&&) noexcept = default;

		private:
			std::shared_ptr<AbstractBuffer> m_instanceDataBuffer;
			Matrix4f m_invWorldMatrix;
			Matrix4f m_worldMatrix;
			bool m_dataInvalided;
	};
}

#include <Nazara/Graphics/WorldInstance.inl>

#endif // NAZARA_GRAPHICS_WORLDINSTANCE_HPP
