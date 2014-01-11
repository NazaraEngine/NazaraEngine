// Copyright (C) 2013 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_DEFERREDPHONGLIGHTINGPASS_HPP
#define NAZARA_DEFERREDPHONGLIGHTINGPASS_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Graphics/DeferredRenderPass.hpp>
#include <Nazara/Renderer/ShaderProgram.hpp>
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
		NzMeshRef m_cone;
		NzMeshRef m_sphere;
		NzShaderProgramRef m_directionalLightProgram;
		NzShaderProgramRef m_pointSpotLightProgram;
		NzTextureSampler m_pointSampler;
		NzStaticMesh* m_coneMesh;
		NzStaticMesh* m_sphereMesh;
		bool m_lightMeshesDrawing;
		int m_pointSpotLightProgramDiscardLocation;
		int m_pointSpotLightProgramSpotLightLocation;
};

#endif // NAZARA_DEFERREDPHONGLIGHTINGPASS_HPP
