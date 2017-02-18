// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_DEFERREDGEOMETRYPASS_HPP
#define NAZARA_DEFERREDGEOMETRYPASS_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Graphics/DeferredRenderPass.hpp>
#include <Nazara/Renderer/RenderStates.hpp>
#include <Nazara/Renderer/Shader.hpp>
#include <unordered_map>

namespace Nz
{
	class NAZARA_GRAPHICS_API DeferredGeometryPass : public DeferredRenderPass
	{
		public:
			DeferredGeometryPass();
			virtual ~DeferredGeometryPass();

			bool Process(const SceneData& sceneData, unsigned int firstWorkTexture, unsigned int secondWorkTexture) const;
			bool Resize(const Vector2ui& dimensions);

		protected:
			struct ShaderUniforms;

			const ShaderUniforms* GetShaderUniforms(const Shader* shader) const;
			void OnShaderInvalidated(const Shader* shader) const;

			struct ShaderUniforms
			{
				NazaraSlot(Shader, OnShaderUniformInvalidated, shaderUniformInvalidatedSlot);
				NazaraSlot(Shader, OnShaderRelease, shaderReleaseSlot);

				int eyePosition;
				int sceneAmbient;
				int textureOverlay;
			};

			mutable std::unordered_map<const Shader*, ShaderUniforms> m_shaderUniforms;
			RenderStates m_clearStates;
			ShaderRef m_clearShader;
	};
}

#endif // NAZARA_DEFERREDGEOMETRYPASS_HPP
