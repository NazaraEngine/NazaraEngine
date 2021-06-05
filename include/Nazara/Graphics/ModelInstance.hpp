// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_MODELINSTANCE_HPP
#define NAZARA_MODELINSTANCE_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Math/Matrix4.hpp>
#include <Nazara/Graphics/Config.hpp>
#include <Nazara/Renderer/ShaderBinding.hpp>
#include <memory>

namespace Nz
{
	class AbstractBuffer;
	class CommandBufferBuilder;
	class MaterialSettings;
	class UploadPool;

	class NAZARA_GRAPHICS_API ModelInstance
	{
		public:
			ModelInstance(const std::shared_ptr<const MaterialSettings>& settings);
			ModelInstance(const ModelInstance&) = delete;
			ModelInstance(ModelInstance&&) noexcept = default;
			~ModelInstance() = default;

			inline std::shared_ptr<AbstractBuffer>& GetInstanceBuffer();
			inline const std::shared_ptr<AbstractBuffer>& GetInstanceBuffer() const;
			inline ShaderBinding& GetShaderBinding();

			void UpdateBuffers(UploadPool& uploadPool, CommandBufferBuilder& builder);
			inline void UpdateWorldMatrix(const Matrix4f& worldMatrix);
			inline void UpdateWorldMatrix(const Matrix4f& worldMatrix, const Matrix4f& invWorldMatrix);

			ModelInstance& operator=(const ModelInstance&) = delete;
			ModelInstance& operator=(ModelInstance&&) noexcept = default;

		private:
			std::shared_ptr<AbstractBuffer> m_instanceDataBuffer;
			Matrix4f m_invWorldMatrix;
			Matrix4f m_worldMatrix;
			ShaderBindingPtr m_shaderBinding;
			bool m_dataInvalided;
	};
}

#include <Nazara/Graphics/ModelInstance.inl>

#endif // NAZARA_MODELINSTANCE_HPP
