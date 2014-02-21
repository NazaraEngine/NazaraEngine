// Copyright (C) 2014 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_FORWARDRENDERTECHNIQUE_HPP
#define NAZARA_FORWARDRENDERTECHNIQUE_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/ResourceListener.hpp>
#include <Nazara/Graphics/AbstractRenderTechnique.hpp>
#include <Nazara/Graphics/ForwardRenderQueue.hpp>
#include <Nazara/Graphics/Light.hpp>
#include <Nazara/Graphics/LightManager.hpp>
#include <Nazara/Utility/IndexBuffer.hpp>
#include <Nazara/Utility/VertexBuffer.hpp>

class NAZARA_API NzForwardRenderTechnique : public NzAbstractRenderTechnique, NzResourceListener
{
	public:
		NzForwardRenderTechnique();
		~NzForwardRenderTechnique();

		void Clear(const NzScene* scene) const;
		bool Draw(const NzScene* scene) const;

		unsigned int GetMaxLightPassPerObject() const;
		NzAbstractRenderQueue* GetRenderQueue() override;
		nzRenderTechniqueType GetType() const override;

		void SetMaxLightPassPerObject(unsigned int passCount);

	private:
		struct LightUniforms;

		void DrawOpaqueModels(const NzScene* scene) const;
		void DrawSprites(const NzScene* scene) const;
		void DrawTransparentModels(const NzScene* scene) const;
		const LightUniforms* GetLightUniforms(const NzShader* shader) const;

		struct LightUniforms
		{
			NzLightUniforms uniforms;
			bool exists;
			int offset; // "Distance" entre Lights[0].type et Lights[1].type
			/// Moins coûteux en mémoire que de stocker un NzLightUniforms par index de lumière,
			/// à voir si ça fonctionne chez tout le monde
		};

		mutable std::unordered_map<const NzShader*, LightUniforms> m_lightUniforms;
		mutable NzForwardRenderQueue m_renderQueue;
		NzIndexBufferRef m_indexBuffer;
		mutable NzLightManager m_directionalLights;
		mutable NzLightManager m_lights;
		NzVertexBuffer m_spriteBuffer;
		unsigned int m_maxLightPassPerObject;
};

#endif // NAZARA_FORWARDRENDERTECHNIQUE_HPP
