// Copyright (C) 2013 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_DEFERREDRENDERTECHNIQUE_HPP
#define NAZARA_DEFERREDRENDERTECHNIQUE_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Graphics/AbstractRenderTechnique.hpp>
#include <Nazara/Graphics/DeferredRenderQueue.hpp>
#include <Nazara/Graphics/ForwardRenderTechnique.hpp>
#include <Nazara/Math/Vector2.hpp>
#include <Nazara/Renderer/RenderStates.hpp>
#include <Nazara/Renderer/RenderTexture.hpp>
#include <Nazara/Renderer/ShaderProgram.hpp>
#include <Nazara/Renderer/Texture.hpp>
#include <Nazara/Renderer/TextureSampler.hpp>
#include <Nazara/Utility/Mesh.hpp>

class NAZARA_API NzDeferredRenderTechnique : public NzAbstractRenderTechnique, public NzRenderTarget::Listener
{
	public:
		NzDeferredRenderTechnique();
		~NzDeferredRenderTechnique();

		void Clear(const NzScene* scene);
		bool Draw(const NzScene* scene);

		NzTexture* GetGBuffer(unsigned int i) const;
		NzAbstractRenderQueue* GetRenderQueue() override;
		nzRenderTechniqueType GetType() const override;
		NzTexture* GetWorkTexture(unsigned int i) const;

		static bool IsSupported();

	private:
		void GeomPass(const NzScene* scene);
		void DirectionalLightPass(const NzScene* scene);
		void PointLightPass(const NzScene* scene);
		void SpotLightPass(const NzScene* scene);
		bool UpdateTextures() const;

		NzForwardRenderTechnique m_forwardTechnique; // Doit être initialisé avant la RenderQueue
		NzDeferredRenderQueue m_renderQueue;
		NzMeshRef m_sphere;
		NzStaticMesh* m_sphereMesh;
		mutable NzRenderTexture m_bloomRTT;
		mutable NzRenderTexture m_dofRTT;
		mutable NzRenderTexture m_geometryRTT;
		mutable NzRenderTexture m_ssaoRTT;
		NzRenderStates m_clearStates;
		NzShaderProgramRef m_aaProgram;
		NzShaderProgramRef m_blitProgram;
		NzShaderProgramRef m_bloomBrightProgram;
		NzShaderProgramRef m_bloomFinalProgram;
		NzShaderProgramRef m_clearProgram;
		NzShaderProgramRef m_directionalLightProgram;
		NzShaderProgramRef m_depthOfFieldProgram;
		NzShaderProgramRef m_gaussianBlurProgram;
		NzShaderProgramRef m_pointLightProgram;
		NzShaderProgramRef m_ssaoProgram;
		NzShaderProgramRef m_ssaoFinalProgram;
		NzShaderProgramRef m_spotLightProgram;
		mutable NzTextureRef m_bloomTextureA;
		mutable NzTextureRef m_bloomTextureB;
		mutable NzTextureRef m_dofTextureA;
		mutable NzTextureRef m_dofTextureB;
		mutable NzTextureRef m_GBuffer[4];
		mutable NzTextureRef m_ssaoTextureA;
		mutable NzTextureRef m_ssaoTextureB;
		mutable NzTextureRef m_ssaoNoiseTexture;
		mutable NzTextureRef m_workTextureA;
		mutable NzTextureRef m_workTextureB;
		NzTextureSampler m_bilinearSampler;
		NzTextureSampler m_pointSampler;
		NzTextureSampler m_ssaoSampler;
		NzVector2ui m_GBufferSize;
		const NzRenderTarget* m_viewerTarget;
		mutable bool m_texturesUpdated;
		int m_gaussianBlurProgramFilterLocation;
};

#endif // NAZARA_FORWARDRENDERTECHNIQUE_HPP
