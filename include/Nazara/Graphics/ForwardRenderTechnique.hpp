// Copyright (C) 2013 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_FORWARDRENDERTECHNIQUE_HPP
#define NAZARA_FORWARDRENDERTECHNIQUE_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Graphics/AbstractRenderTechnique.hpp>
#include <Nazara/Graphics/ForwardRenderQueue.hpp>

class NAZARA_API NzForwardRenderTechnique : public NzAbstractRenderTechnique
{
	public:
		NzForwardRenderTechnique();
		~NzForwardRenderTechnique() = default;

		void Clear(const NzScene* scene);
		void Draw(const NzScene* scene);

		unsigned int GetMaxLightsPerObject() const;
		NzAbstractRenderQueue* GetRenderQueue() override;

		void SetMaxLightsPerObject(unsigned int lightCount);

	private:
		NzForwardRenderQueue m_renderQueue;
		unsigned int m_maxLightsPerObject;
};

#endif // NAZARA_FORWARDRENDERTECHNIQUE_HPP
