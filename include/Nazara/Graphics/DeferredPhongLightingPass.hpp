// Copyright (C) 2014 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_DEFERREDPHONGLIGHTINGPASS_HPP
#define NAZARA_DEFERREDPHONGLIGHTINGPASS_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Graphics/DeferredRenderPass.hpp>
#include <Nazara/Graphics/Light.hpp>
#include <Nazara/Renderer/Shader.hpp>
#include <Nazara/Renderer/TextureSampler.hpp>
#include <Nazara/Utility/Mesh.hpp>

class NzStaticMesh;

class NAZARA_API NzDeferredPhongLightingPass : public NzDeferredRenderPass
{
	public:
		NzDeferredPhongLightingPass();
		virtual ~NzDeferredPhongLightingPass();

		void EnableLightMeshesDrawing(bool enable);

		bool IsLightMeshesDrawingEnabled() const;

		bool Process(const NzScene* scene, unsigned int firstWorkTexture, unsigned secondWorkTexture) const;

	protected:
		NzLightUniforms m_directionalLightUniforms;
		NzLightUniforms m_pointSpotLightUniforms;
		NzMeshRef m_cone;
		NzMeshRef m_sphere;
		NzShaderRef m_directionalLightShader;
		NzShaderRef m_pointSpotLightShader;
		NzTextureSampler m_pointSampler;
		NzStaticMesh* m_coneMesh;
		NzStaticMesh* m_sphereMesh;
		bool m_lightMeshesDrawing;
		int m_pointSpotLightShaderDiscardLocation;
};

#endif // NAZARA_DEFERREDPHONGLIGHTINGPASS_HPP
