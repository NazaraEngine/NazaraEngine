// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_FORWARDRENDERTECHNIQUE_HPP
#define NAZARA_FORWARDRENDERTECHNIQUE_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Graphics/AbstractRenderTechnique.hpp>
#include <Nazara/Graphics/ForwardRenderQueue.hpp>
#include <Nazara/Graphics/Light.hpp>
#include <Nazara/Renderer/Shader.hpp>
#include <Nazara/Utility/IndexBuffer.hpp>
#include <Nazara/Utility/VertexBuffer.hpp>

class NAZARA_API NzForwardRenderTechnique : public NzAbstractRenderTechnique
{
	public:
		NzForwardRenderTechnique();
		~NzForwardRenderTechnique() = default;

		bool Draw(const NzSceneData& sceneData) const override;

		unsigned int GetMaxLightPassPerObject() const;
		NzAbstractRenderQueue* GetRenderQueue() override;
		nzRenderTechniqueType GetType() const override;

		void SetMaxLightPassPerObject(unsigned int passCount);

		static bool Initialize();
		static void Uninitialize();

	private:
		struct ShaderUniforms;

		bool ChooseLights(const NzSpheref& object, bool includeDirectionalLights = true) const;
		void DrawBasicSprites(const NzSceneData& sceneData) const;
		void DrawBillboards(const NzSceneData& sceneData) const;
		void DrawOpaqueModels(const NzSceneData& sceneData) const;
		void DrawTransparentModels(const NzSceneData& sceneData) const;
		const ShaderUniforms* GetShaderUniforms(const NzShader* shader) const;
		void OnShaderInvalidated(const NzShader* shader) const;
		void SendLightUniforms(const NzShader* shader, const NzLightUniforms& uniforms, unsigned int index, unsigned int uniformOffset) const;

		static float ComputeDirectionalLightScore(const NzSpheref& object, const NzAbstractRenderQueue::DirectionalLight& light);
		static float ComputePointLightScore(const NzSpheref& object, const NzAbstractRenderQueue::PointLight& light);
		static float ComputeSpotLightScore(const NzSpheref& object, const NzAbstractRenderQueue::SpotLight& light);
		static bool IsDirectionalLightSuitable(const NzSpheref& object, const NzAbstractRenderQueue::DirectionalLight& light);
		static bool IsPointLightSuitable(const NzSpheref& object, const NzAbstractRenderQueue::PointLight& light);
		static bool IsSpotLightSuitable(const NzSpheref& object, const NzAbstractRenderQueue::SpotLight& light);

		struct LightIndex
		{
			nzLightType type;
			float score;
			unsigned int index;
		};

		struct ShaderUniforms
		{
			NazaraSlot(NzShader, OnShaderUniformInvalidated, shaderUniformInvalidatedSlot);
			NazaraSlot(NzShader, OnShaderRelease, shaderReleaseSlot);

			NzLightUniforms lightUniforms;
			bool hasLightUniforms;

			/// Moins coûteux en mémoire que de stocker un NzLightUniforms par index de lumière,
			/// à voir si ça fonctionne chez tout le monde
			int lightOffset; // "Distance" entre Lights[0].type et Lights[1].type

			// Autre uniformes
			int eyePosition;
			int sceneAmbient;
			int textureOverlay;
		};

		mutable std::unordered_map<const NzShader*, ShaderUniforms> m_shaderUniforms;
		mutable std::vector<LightIndex> m_lights;
		NzBuffer m_vertexBuffer;
		mutable NzForwardRenderQueue m_renderQueue;
		NzVertexBuffer m_billboardPointBuffer;
		NzVertexBuffer m_spriteBuffer;
		unsigned int m_maxLightPassPerObject;

		static NzIndexBuffer s_quadIndexBuffer;
		static NzVertexBuffer s_quadVertexBuffer;
		static NzVertexDeclaration s_billboardInstanceDeclaration;
		static NzVertexDeclaration s_billboardVertexDeclaration;
};

#include <Nazara/Graphics/ForwardRenderTechnique.inl>

#endif // NAZARA_FORWARDRENDERTECHNIQUE_HPP
