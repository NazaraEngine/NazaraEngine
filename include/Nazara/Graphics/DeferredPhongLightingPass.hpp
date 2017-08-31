// Copyright (C) 2017 Jérôme Leclercq
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

namespace Nz
{
	class StaticMesh;

	class NAZARA_GRAPHICS_API DeferredPhongLightingPass : public DeferredRenderPass
	{
		public:
			DeferredPhongLightingPass();
			virtual ~DeferredPhongLightingPass();

			void EnableLightMeshesDrawing(bool enable);

			bool IsLightMeshesDrawingEnabled() const;

			bool Process(const SceneData& sceneData, unsigned int firstWorkTexture, unsigned int secondWorkTexture) const override;

		protected:
			LightUniforms m_directionalLightUniforms;
			LightUniforms m_pointSpotLightUniforms;
			MeshRef m_cone;
			MeshRef m_sphere;
			ShaderRef m_directionalLightShader;
			ShaderRef m_pointSpotLightShader;
			TextureSampler m_pointSampler;
			StaticMesh* m_coneMesh;
			StaticMesh* m_sphereMesh;
			bool m_lightMeshesDrawing;
			int m_directionalLightShaderEyePositionLocation;
			int m_directionalLightShaderSceneAmbientLocation;
			int m_pointSpotLightShaderDiscardLocation;
			int m_pointSpotLightShaderEyePositionLocation;
			int m_pointSpotLightShaderSceneAmbientLocation;
	};
}

#endif // NAZARA_DEFERREDPHONGLIGHTINGPASS_HPP
