// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_DEFERREDGEOMETRYPASS_HPP
#define NAZARA_DEFERREDGEOMETRYPASS_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Graphics/BasicRenderQueue.hpp>
#include <Nazara/Graphics/DeferredRenderPass.hpp>
#include <Nazara/Renderer/RenderStates.hpp>
#include <Nazara/Renderer/Shader.hpp>
#include <unordered_map>

namespace Nz
{
	class NAZARA_GRAPHICS_API DeferredGeometryPass : public DeferredRenderPass
	{
		friend class DeferredRenderTechnique;

		public:
			DeferredGeometryPass();
			virtual ~DeferredGeometryPass();

			bool Process(const SceneData& sceneData, unsigned int firstWorkTexture, unsigned int secondWorkTexture) const override;
			bool Resize(const Vector2ui& dimensions) override;

		protected:
			struct ShaderUniforms;

			void DrawBillboards(const SceneData& sceneData, const BasicRenderQueue& renderQueue, const RenderQueue<BasicRenderQueue::Billboard>& billboards) const;
			void DrawBillboards(const SceneData& sceneData, const BasicRenderQueue& renderQueue, const RenderQueue<BasicRenderQueue::BillboardChain>& billboards) const;
			void DrawModels(const SceneData& sceneData, const BasicRenderQueue& renderQueue, const RenderQueue<BasicRenderQueue::Model>& models) const;
			void DrawSprites(const SceneData& sceneData, const BasicRenderQueue& renderQueue, const RenderQueue<BasicRenderQueue::SpriteChain>& sprites) const;

			const ShaderUniforms* GetShaderUniforms(const Shader* shader) const;
			void OnShaderInvalidated(const Shader* shader) const;

			static bool Initialize();
			static void Uninitialize();

			struct ShaderUniforms
			{
				NazaraSlot(Shader, OnShaderUniformInvalidated, shaderUniformInvalidatedSlot);
				NazaraSlot(Shader, OnShaderRelease, shaderReleaseSlot);

				int eyePosition;
				int sceneAmbient;
				int textureOverlay;
			};

			mutable std::unordered_map<const Shader*, ShaderUniforms> m_shaderUniforms;
			mutable std::vector<std::pair<const VertexStruct_XYZ_Color_UV*, std::size_t>> m_spriteChains;
			Buffer m_vertexBuffer;
			RenderStates m_clearStates;
			ShaderRef m_clearShader;
			TextureRef m_whiteTexture;
			VertexBuffer m_billboardPointBuffer;
			VertexBuffer m_spriteBuffer;

			static IndexBuffer s_quadIndexBuffer;
			static VertexBuffer s_quadVertexBuffer;
			static VertexDeclaration s_billboardInstanceDeclaration;
			static VertexDeclaration s_billboardVertexDeclaration;
	};
}

#endif // NAZARA_DEFERREDGEOMETRYPASS_HPP
