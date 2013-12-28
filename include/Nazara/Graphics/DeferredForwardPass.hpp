// Copyright (C) 2013 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_DEFERREDFORWARDPASS_HPP
#define NAZARA_DEFERREDFORWARDPASS_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Graphics/DeferredRenderPass.hpp>

class NzForwardRenderTechnique;

class NAZARA_API NzDeferredForwardPass : public NzDeferredRenderPass
{
	public:
		NzDeferredForwardPass();
		virtual ~NzDeferredForwardPass();

		void Initialize(NzDeferredRenderTechnique* technique);
		bool Process(const NzScene* scene, unsigned int workTexture, unsigned sceneTexture) const;

	protected:
		const NzForwardRenderTechnique* m_forwardTechnique;
};

#endif // NAZARA_DEFERREDFORWARDPASS_HPP
