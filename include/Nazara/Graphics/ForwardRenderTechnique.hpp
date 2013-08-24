// Copyright (C) 2013 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_FORWARDRENDERTECHNIQUE_HPP
#define NAZARA_FORWARDRENDERTECHNIQUE_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Graphics/AbstractRenderTechnique.hpp>
#include <Nazara/Graphics/ForwardRenderQueue.hpp>
#include <Nazara/Graphics/LightManager.hpp>
#include <Nazara/Utility/IndexBuffer.hpp>
#include <Nazara/Utility/VertexBuffer.hpp>

class NAZARA_API NzForwardRenderTechnique : public NzAbstractRenderTechnique
{
	public:
		NzForwardRenderTechnique();
		~NzForwardRenderTechnique();

		void Clear(const NzScene* scene);
		void Draw(const NzScene* scene);

		unsigned int GetMaxLightsPerObject() const;
		NzAbstractRenderQueue* GetRenderQueue() override;

		void SetMaxLightsPerObject(unsigned int lightCount);

	private:
		void DrawOpaqueModels(const NzScene* scene, NzForwardRenderQueue::BatchedModelContainer& opaqueModels);
		void DrawSprites(const NzScene* scene, NzForwardRenderQueue::BatchedSpriteContainer& sprites);
		void DrawTransparentModels(const NzScene* scene, NzForwardRenderQueue::TransparentModelContainer& transparentModels);

		NzForwardRenderQueue m_renderQueue;
		NzIndexBufferRef m_indexBuffer;
		NzLightManager m_directionnalLights;
		NzLightManager m_lights;
		NzVertexBuffer m_spriteBuffer;
		unsigned int m_maxLightsPerObject;
};

#endif // NAZARA_FORWARDRENDERTECHNIQUE_HPP
