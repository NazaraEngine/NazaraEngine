// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_VIEWERINSTANCE_HPP
#define NAZARA_VIEWERINSTANCE_HPP

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

	class NAZARA_GRAPHICS_API ViewerInstance
	{
		public:
			ViewerInstance();
			ViewerInstance(const ViewerInstance&) = delete;
			ViewerInstance(ViewerInstance&&) noexcept = default;
			~ViewerInstance() = default;

			inline std::shared_ptr<AbstractBuffer>& GetInstanceBuffer();
			inline const std::shared_ptr<AbstractBuffer>& GetInstanceBuffer() const;
			inline ShaderBinding& GetShaderBinding();

			void UpdateBuffers(UploadPool& uploadPool, CommandBufferBuilder& builder);
			inline void UpdateProjectionMatrix(const Matrix4f& projectionMatrix);
			inline void UpdateProjectionMatrix(const Matrix4f& projectionMatrix, const Matrix4f& invProjectionMatrix);
			inline void UpdateProjViewMatrices(const Matrix4f& projectionMatrix, const Matrix4f& viewMatrix);
			inline void UpdateProjViewMatrices(const Matrix4f& projectionMatrix, const Matrix4f& invProjectionMatrix, const Matrix4f& viewMatrix, const Matrix4f& invViewMatrix);
			inline void UpdateProjViewMatrices(const Matrix4f& projectionMatrix, const Matrix4f& invProjectionMatrix, const Matrix4f& viewMatrix, const Matrix4f& invViewMatrix, const Matrix4f& viewProjMatrix, const Matrix4f& invViewProjMatrix);
			inline void UpdateTargetSize(const Vector2f& targetSize);
			inline void UpdateViewMatrix(const Matrix4f& viewMatrix);
			inline void UpdateViewMatrix(const Matrix4f& viewMatrix, const Matrix4f& invViewMatrix);

			ViewerInstance& operator=(const ViewerInstance&) = delete;
			ViewerInstance& operator=(ViewerInstance&&) noexcept = default;

		private:
			std::shared_ptr<AbstractBuffer> m_viewerDataBuffer;
			Matrix4f m_invProjectionMatrix;
			Matrix4f m_invViewProjMatrix;
			Matrix4f m_invViewMatrix;
			Matrix4f m_projectionMatrix;
			Matrix4f m_viewProjMatrix;
			Matrix4f m_viewMatrix;
			ShaderBindingPtr m_shaderBinding;
			Vector2f m_targetSize;
			bool m_dataInvalided;
	};
}

#include <Nazara/Graphics/ViewerInstance.inl>

#endif // NAZARA_VIEWERINSTANCE_HPP
