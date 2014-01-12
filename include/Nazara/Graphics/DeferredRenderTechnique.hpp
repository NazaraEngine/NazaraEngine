// Copyright (C) 2014 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_DEFERREDRENDERTECHNIQUE_HPP
#define NAZARA_DEFERREDRENDERTECHNIQUE_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Graphics/AbstractRenderTechnique.hpp>
#include <Nazara/Graphics/DeferredRenderPass.hpp>
#include <Nazara/Graphics/DeferredRenderQueue.hpp>
#include <Nazara/Graphics/ForwardRenderTechnique.hpp>
#include <Nazara/Math/Vector2.hpp>
#include <Nazara/Renderer/RenderBuffer.hpp>
#include <Nazara/Renderer/RenderStates.hpp>
#include <Nazara/Renderer/RenderTexture.hpp>
#include <Nazara/Renderer/ShaderProgram.hpp>
#include <Nazara/Renderer/Texture.hpp>
#include <Nazara/Renderer/TextureSampler.hpp>
#include <Nazara/Utility/Mesh.hpp>
#include <map>
#include <memory>

class NAZARA_API NzDeferredRenderTechnique : public NzAbstractRenderTechnique, public NzRenderTarget::Listener
{
	public:
		NzDeferredRenderTechnique();
		~NzDeferredRenderTechnique();

		void Clear(const NzScene* scene) const;
		bool Draw(const NzScene* scene) const;

		void EnablePass(nzRenderPassType renderPass, int position, bool enable);

		NzRenderBuffer* GetDepthStencilBuffer() const;
		NzTexture* GetGBuffer(unsigned int i) const;
		NzRenderTexture* GetGBufferRTT() const;
		const NzForwardRenderTechnique* GetForwardTechnique() const;
		NzDeferredRenderPass* GetPass(nzRenderPassType renderPass, int position = 0);
		NzAbstractRenderQueue* GetRenderQueue() override;
		nzRenderTechniqueType GetType() const override;
		NzRenderTexture* GetWorkRTT() const;
		NzTexture* GetWorkTexture(unsigned int i) const;

		bool IsPassEnabled(nzRenderPassType renderPass, int position);

		NzDeferredRenderPass* ResetPass(nzRenderPassType renderPass, int position);

		void SetPass(nzRenderPassType relativeTo, int position, NzDeferredRenderPass* pass);

		static bool IsSupported();

	private:
		bool Resize(const NzVector2ui& dimensions) const;

		struct RenderPassComparator
		{
			bool operator()(nzRenderPassType pass1, nzRenderPassType pass2);
		};

		std::map<nzRenderPassType, std::map<int, std::unique_ptr<NzDeferredRenderPass>>, RenderPassComparator> m_passes;
		NzForwardRenderTechnique m_forwardTechnique; // Doit être initialisé avant la RenderQueue
		NzDeferredRenderQueue m_renderQueue;
		mutable NzRenderBufferRef m_depthStencilBuffer;
		mutable NzRenderTexture m_GBufferRTT;
		mutable NzRenderTexture m_workRTT;
		mutable NzTextureRef m_GBuffer[4];
		mutable NzTextureRef m_workTextures[2];
		mutable NzVector2ui m_GBufferSize;
		const NzRenderTarget* m_viewerTarget;
};

#endif // NAZARA_FORWARDRENDERTECHNIQUE_HPP
