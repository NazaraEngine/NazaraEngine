// Copyright (C) 2013 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_DEFERREDRENDERPASS_HPP
#define NAZARA_DEFERREDRENDERPASS_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Graphics/Enums.hpp>
#include <Nazara/Math/Vector2.hpp>

class NzDeferredRenderTechnique;
class NzDeferredRenderQueue;
class NzRenderBuffer;
class NzRenderTexture;
class NzScene;
class NzTexture;

class NAZARA_API NzDeferredRenderPass
{
	friend NzDeferredRenderTechnique;

	public:
		NzDeferredRenderPass();
		virtual ~NzDeferredRenderPass();

		void Enable(bool enable);

		virtual void Initialize(NzDeferredRenderTechnique* technique);

		bool IsEnabled() const;

		virtual bool Process(const NzScene* scene, unsigned int workTexture, unsigned sceneTexture) const = 0;
		virtual bool Resize(const NzVector2ui& GBufferSize);

	protected:
		NzVector2ui m_dimensions;
		NzDeferredRenderTechnique* m_deferredTechnique;
		NzDeferredRenderQueue* m_renderQueue;
		NzRenderBuffer* m_depthStencilBuffer;
		NzRenderTexture* m_GBufferRTT;
		NzRenderTexture* m_workRTT;
		NzTexture* m_GBuffer[4];
		NzTexture* m_workTextures[2];

	private:
		bool m_enabled;
};

#endif // NAZARA_DEFERREDRENDERPASS_HPP
