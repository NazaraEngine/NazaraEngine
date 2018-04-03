// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_DEPTHRENDERTECHNIQUE_HPP
#define NAZARA_DEPTHRENDERTECHNIQUE_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Graphics/AbstractRenderTechnique.hpp>
#include <Nazara/Graphics/Config.hpp>
#include <Nazara/Graphics/DepthRenderQueue.hpp>
#include <Nazara/Renderer/Shader.hpp>
#include <Nazara/Utility/IndexBuffer.hpp>
#include <Nazara/Utility/VertexBuffer.hpp>

namespace Nz
{
	class NAZARA_GRAPHICS_API DepthRenderTechnique : public AbstractRenderTechnique
	{
		public:
			DepthRenderTechnique();
			~DepthRenderTechnique() = default;

			void Clear(const SceneData& sceneData) const override;
			bool Draw(const SceneData& sceneData) const override;

			AbstractRenderQueue* GetRenderQueue() override;
			RenderTechniqueType GetType() const override;

			static bool Initialize();
			static void Uninitialize();

		private:
			struct ShaderUniforms;

			void DrawBasicSprites(const SceneData& sceneData, BasicRenderQueue::Layer& layer) const;
			void DrawBillboards(const SceneData& sceneData, BasicRenderQueue::Layer& layer) const;
			void DrawOpaqueModels(const SceneData& sceneData, BasicRenderQueue::Layer& layer) const;
			const ShaderUniforms* GetShaderUniforms(const Shader* shader) const;
			void OnShaderInvalidated(const Shader* shader) const;

			struct LightIndex
			{
				LightType type;
				float score;
				unsigned int index;
			};

			struct ShaderUniforms
			{
				NazaraSlot(Shader, OnShaderUniformInvalidated, shaderUniformInvalidatedSlot);
				NazaraSlot(Shader, OnShaderRelease, shaderReleaseSlot);

				// Autre uniformes
				int sceneAmbient;
				int textureOverlay;
			};

			mutable std::unordered_map<const Shader*, ShaderUniforms> m_shaderUniforms;
			Buffer m_vertexBuffer;
			mutable DepthRenderQueue m_renderQueue;
			Texture m_whiteTexture;
			VertexBuffer m_billboardPointBuffer;
			VertexBuffer m_spriteBuffer;

			static IndexBuffer s_quadIndexBuffer;
			static VertexBuffer s_quadVertexBuffer;
			static VertexDeclaration s_billboardInstanceDeclaration;
			static VertexDeclaration s_billboardVertexDeclaration;
	};
}


#include <Nazara/Graphics/DepthRenderTechnique.inl>

#endif // NAZARA_DEPTHRENDERTECHNIQUE_HPP
