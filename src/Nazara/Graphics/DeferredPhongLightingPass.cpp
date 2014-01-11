// Copyright (C) 2013 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/DeferredPhongLightingPass.hpp>
#include <Nazara/Graphics/AbstractViewer.hpp>
#include <Nazara/Graphics/DeferredRenderQueue.hpp>
#include <Nazara/Graphics/Light.hpp>
#include <Nazara/Graphics/Scene.hpp>
#include <Nazara/Renderer/Renderer.hpp>
#include <Nazara/Renderer/RenderTexture.hpp>
#include <Nazara/Renderer/ShaderProgramManager.hpp>
#include <Nazara/Utility/StaticMesh.hpp>
#include <memory>
#include <Nazara/Renderer/OpenGL.hpp> // Supprimer
#include <Nazara/Graphics/Debug.hpp>

namespace
{
	NzShaderProgram* BuildDirectionalLightProgram()
	{
		const nzUInt8 fragmentSource[] = {
			#include <Nazara/Graphics/Resources/DeferredShading/Shaders/DirectionalLight.frag.h>
		};

		const char* vertexSource =
		"#version 140\n"

		"in vec2 VertexPosition;\n"

		"void main()\n"
		"{\n"
		"\t" "gl_Position = vec4(VertexPosition, 0.0, 1.0);" "\n"
		"}\n";

		///TODO: Remplacer ça par des ShaderNode
		std::unique_ptr<NzShaderProgram> program(new NzShaderProgram(nzShaderLanguage_GLSL));
		program->SetPersistent(false);

		if (!program->LoadShader(nzShaderType_Fragment, NzString(reinterpret_cast<const char*>(fragmentSource), sizeof(fragmentSource))))
		{
			NazaraError("Failed to load fragment shader");
			return nullptr;
		}

		if (!program->LoadShader(nzShaderType_Vertex, vertexSource))
		{
			NazaraError("Failed to load vertex shader");
			return nullptr;
		}

		if (!program->Compile())
		{
			NazaraError("Failed to compile program");
			return nullptr;
		}

		program->SendInteger(program->GetUniformLocation("GBuffer0"), 0);
		program->SendInteger(program->GetUniformLocation("GBuffer1"), 1);
		program->SendInteger(program->GetUniformLocation("GBuffer2"), 2);

		return program.release();
	}

	NzShaderProgram* BuildPointSpotLightProgram()
	{
		const nzUInt8 fragmentSource[] = {
			#include <Nazara/Graphics/Resources/DeferredShading/Shaders/PointSpotLight.frag.h>
		};

		const char* vertexSource =
		"#version 140\n"

		"in vec3 VertexPosition;\n"

		"uniform mat4 WorldViewProjMatrix;\n"

		"void main()\n"
		"{\n"
		"\t" "gl_Position = WorldViewProjMatrix * vec4(VertexPosition, 1.0);" "\n"
		"}\n";

		///TODO: Remplacer ça par des ShaderNode
		std::unique_ptr<NzShaderProgram> program(new NzShaderProgram(nzShaderLanguage_GLSL));
		program->SetPersistent(false);

		if (!program->LoadShader(nzShaderType_Fragment, NzString(reinterpret_cast<const char*>(fragmentSource), sizeof(fragmentSource))))
		{
			NazaraError("Failed to load fragment shader");
			return nullptr;
		}

		if (!program->LoadShader(nzShaderType_Vertex, vertexSource))
		{
			NazaraError("Failed to load vertex shader");
			return nullptr;
		}

		if (!program->Compile())
		{
			NazaraError("Failed to compile program");
			return nullptr;
		}

		program->SendInteger(program->GetUniformLocation("GBuffer0"), 0);
		program->SendInteger(program->GetUniformLocation("GBuffer1"), 1);
		program->SendInteger(program->GetUniformLocation("GBuffer2"), 2);

		return program.release();
	}
}

NzDeferredPhongLightingPass::NzDeferredPhongLightingPass() :
m_lightMeshesDrawing(false)
{
	m_directionalLightProgram = BuildDirectionalLightProgram();
	m_pointSpotLightProgram = BuildPointSpotLightProgram();

	m_pointSpotLightProgramDiscardLocation = m_pointSpotLightProgram->GetUniformLocation("Discard");
	m_pointSpotLightProgramSpotLightLocation = m_pointSpotLightProgram->GetUniformLocation("SpotLight");

	m_pointSampler.SetAnisotropyLevel(1);
	m_pointSampler.SetFilterMode(nzSamplerFilter_Nearest);
	m_pointSampler.SetWrapMode(nzSamplerWrap_Clamp);

	m_cone = new NzMesh;
	m_cone->SetPersistent(false);
	m_cone->CreateStatic();
	m_coneMesh = static_cast<NzStaticMesh*>(m_cone->BuildSubMesh(NzPrimitive::Cone(1.f, 1.f, 16, NzMatrix4f::Rotate(NzEulerAnglesf(90.f, 0.f, 0.f)))));

	m_sphere = new NzMesh;
	m_sphere->SetPersistent(false);
	m_sphere->CreateStatic();
	m_sphereMesh = static_cast<NzStaticMesh*>(m_sphere->BuildSubMesh(NzPrimitive::IcoSphere(1.f, 1)));
}

NzDeferredPhongLightingPass::~NzDeferredPhongLightingPass() = default;

void NzDeferredPhongLightingPass::EnableLightMeshesDrawing(bool enable)
{
	m_lightMeshesDrawing = enable;
}

bool NzDeferredPhongLightingPass::IsLightMeshesDrawingEnabled() const
{
	return m_lightMeshesDrawing;
}

bool NzDeferredPhongLightingPass::Process(const NzScene* scene, unsigned int firstWorkTexture, unsigned secondWorkTexture) const
{
	NazaraUnused(secondWorkTexture);

	m_workRTT->SetColorTarget(firstWorkTexture);
	NzRenderer::SetTarget(m_workRTT);
	NzRenderer::SetViewport(NzRecti(0, 0, m_dimensions.x, m_dimensions.y));

	NzRenderer::SetTexture(0, m_GBuffer[0]);
	NzRenderer::SetTextureSampler(0, m_pointSampler);

	NzRenderer::SetTexture(1, m_GBuffer[1]);
	NzRenderer::SetTextureSampler(1, m_pointSampler);

	NzRenderer::SetTexture(2, m_GBuffer[2]);
	NzRenderer::SetTextureSampler(2, m_pointSampler);

	NzRenderer::SetClearColor(NzColor::Black);
	NzRenderer::Clear(nzRendererBuffer_Color);

	NzRenderStates lightStates;
	lightStates.dstBlend = nzBlendFunc_One;
	lightStates.srcBlend = nzBlendFunc_One;
	lightStates.parameters[nzRendererParameter_Blend] = true;
	lightStates.parameters[nzRendererParameter_DepthBuffer] = false;
	lightStates.parameters[nzRendererParameter_DepthWrite] = false;

	// Directional lights
	if (!m_renderQueue->directionalLights.empty())
	{
		NzRenderer::SetRenderStates(lightStates);
		NzRenderer::SetShaderProgram(m_directionalLightProgram);
		m_directionalLightProgram->SendColor(m_directionalLightProgram->GetUniformLocation(nzShaderUniform_SceneAmbient), scene->GetAmbientColor());
		m_directionalLightProgram->SendVector(m_directionalLightProgram->GetUniformLocation(nzShaderUniform_EyePosition), scene->GetViewer()->GetEyePosition());

		for (const NzLight* light : m_renderQueue->directionalLights)
		{
			light->Enable(m_directionalLightProgram, 0);
			NzRenderer::DrawFullscreenQuad();
		}
	}

	// Point lights/Spot lights
	if (!m_renderQueue->pointLights.empty() || !m_renderQueue->spotLights.empty())
	{
		// http://www.altdevblogaday.com/2011/08/08/stencil-buffer-optimisation-for-deferred-lights/
		lightStates.parameters[nzRendererParameter_StencilTest] = true;
		lightStates.faceCulling = nzFaceSide_Front;
		lightStates.backFace.stencilMask = 0xFF;
		lightStates.backFace.stencilReference = 0;
		lightStates.backFace.stencilFail = nzStencilOperation_Keep;
		lightStates.backFace.stencilPass = nzStencilOperation_Keep;
		lightStates.backFace.stencilZFail = nzStencilOperation_Invert;
		lightStates.frontFace.stencilMask = 0xFF;
		lightStates.frontFace.stencilReference = 0;
		lightStates.frontFace.stencilFail = nzStencilOperation_Keep;
		lightStates.frontFace.stencilPass = nzStencilOperation_Keep;
		lightStates.frontFace.stencilZFail = nzStencilOperation_Invert;

		NzRenderer::SetRenderStates(lightStates);

		NzRenderer::SetShaderProgram(m_pointSpotLightProgram);
		m_pointSpotLightProgram->SendColor(m_pointSpotLightProgram->GetUniformLocation(nzShaderUniform_SceneAmbient), scene->GetAmbientColor());
		m_pointSpotLightProgram->SendVector(m_pointSpotLightProgram->GetUniformLocation(nzShaderUniform_EyePosition), scene->GetViewer()->GetEyePosition());

		NzMatrix4f lightMatrix;
		lightMatrix.MakeIdentity();
		if (!m_renderQueue->pointLights.empty())
		{
			m_pointSpotLightProgram->SendBoolean(m_pointSpotLightProgramSpotLightLocation, false);

			const NzIndexBuffer* indexBuffer = m_sphereMesh->GetIndexBuffer();
			NzRenderer::SetIndexBuffer(indexBuffer);
			NzRenderer::SetVertexBuffer(m_sphereMesh->GetVertexBuffer());

			for (const NzLight* light : m_renderQueue->pointLights)
			{
				light->Enable(m_pointSpotLightProgram, 0);
				lightMatrix.SetScale(NzVector3f(light->GetRadius()*1.1f)); // Pour corriger les imperfections liées à la sphère
				lightMatrix.SetTranslation(light->GetPosition());

				NzRenderer::SetMatrix(nzMatrixType_World, lightMatrix);

				// Rendu de la sphère dans le stencil buffer
				NzRenderer::Enable(nzRendererParameter_ColorWrite, false);
				NzRenderer::Enable(nzRendererParameter_DepthBuffer, true);
				NzRenderer::Enable(nzRendererParameter_FaceCulling, false);
				NzRenderer::SetStencilCompareFunction(nzRendererComparison_Always);

				m_pointSpotLightProgram->SendBoolean(m_pointSpotLightProgramDiscardLocation, true);

				NzRenderer::DrawIndexedPrimitives(nzPrimitiveMode_TriangleList, 0, indexBuffer->GetIndexCount());

				// Rendu de la sphère comme zone d'effet
				NzRenderer::Enable(nzRendererParameter_ColorWrite, true);
				NzRenderer::Enable(nzRendererParameter_DepthBuffer, false);
				NzRenderer::Enable(nzRendererParameter_FaceCulling, true);
				NzRenderer::SetStencilCompareFunction(nzRendererComparison_NotEqual, nzFaceSide_Back);
				NzRenderer::SetStencilPassOperation(nzStencilOperation_Zero, nzFaceSide_Back);

				m_pointSpotLightProgram->SendBoolean(m_pointSpotLightProgramDiscardLocation, false);

				NzRenderer::DrawIndexedPrimitives(nzPrimitiveMode_TriangleList, 0, indexBuffer->GetIndexCount());
			}

			if (m_lightMeshesDrawing)
			{
				NzRenderer::Enable(nzRendererParameter_DepthBuffer, true);
				NzRenderer::Enable(nzRendererParameter_DepthWrite, true);
				NzRenderer::Enable(nzRendererParameter_FaceCulling, false);
				NzRenderer::Enable(nzRendererParameter_StencilTest, false);
				NzRenderer::SetFaceFilling(nzFaceFilling_Line);

				NzShaderProgramManagerParams params;
				params.flags = nzShaderFlags_None;
				params.target = nzShaderTarget_Model;
				params.model.alphaMapping = false;
				params.model.alphaTest = false;
				params.model.diffuseMapping = false;
				params.model.emissiveMapping = false;
				params.model.lighting = false;
				params.model.normalMapping = false;
				params.model.parallaxMapping = false;
				params.model.specularMapping = false;

				const NzShaderProgram* program = NzShaderProgramManager::Get(params);
				NzRenderer::SetShaderProgram(program);
				for (const NzLight* light : m_renderQueue->pointLights)
				{
					lightMatrix.SetScale(NzVector3f(light->GetRadius()*1.1f)); // Pour corriger les imperfections liées à la sphère
					lightMatrix.SetTranslation(light->GetPosition());

					NzRenderer::SetMatrix(nzMatrixType_World, lightMatrix);

					program->SendColor(program->GetUniformLocation(nzShaderUniform_MaterialDiffuse), light->GetColor());

					NzRenderer::DrawIndexedPrimitives(nzPrimitiveMode_TriangleList, 0, indexBuffer->GetIndexCount());
				}

				NzRenderer::Enable(nzRendererParameter_DepthBuffer, false);
				NzRenderer::Enable(nzRendererParameter_DepthWrite, false);
				NzRenderer::Enable(nzRendererParameter_FaceCulling, true);
				NzRenderer::Enable(nzRendererParameter_StencilTest, true);
				NzRenderer::SetFaceFilling(nzFaceFilling_Fill);
			}
		}

		if (!m_renderQueue->spotLights.empty())
		{
			m_pointSpotLightProgram->SendBoolean(m_pointSpotLightProgramSpotLightLocation, true);

			const NzIndexBuffer* indexBuffer = m_coneMesh->GetIndexBuffer();
			NzRenderer::SetIndexBuffer(indexBuffer);
			NzRenderer::SetVertexBuffer(m_coneMesh->GetVertexBuffer());

			for (const NzLight* light : m_renderQueue->spotLights)
			{
				light->Enable(m_pointSpotLightProgram, 0);
				float radius = light->GetRadius()*std::tan(NzDegreeToRadian(light->GetOuterAngle()))*1.1f;
				lightMatrix.MakeTransform(light->GetPosition(), light->GetRotation(), NzVector3f(radius, radius, light->GetRadius()));

				NzRenderer::SetMatrix(nzMatrixType_World, lightMatrix);

				// Rendu de la sphère dans le stencil buffer
				NzRenderer::Enable(nzRendererParameter_ColorWrite, false);
				NzRenderer::Enable(nzRendererParameter_DepthBuffer, true);
				NzRenderer::Enable(nzRendererParameter_FaceCulling, false);
				NzRenderer::SetStencilCompareFunction(nzRendererComparison_Always);

				m_pointSpotLightProgram->SendBoolean(m_pointSpotLightProgramDiscardLocation, true);

				NzRenderer::DrawIndexedPrimitives(nzPrimitiveMode_TriangleList, 0, indexBuffer->GetIndexCount());

				// Rendu de la sphère comme zone d'effet
				NzRenderer::Enable(nzRendererParameter_ColorWrite, true);
				NzRenderer::Enable(nzRendererParameter_DepthBuffer, false);
				NzRenderer::Enable(nzRendererParameter_FaceCulling, true);
				NzRenderer::SetFaceCulling(nzFaceSide_Front);
				NzRenderer::SetStencilCompareFunction(nzRendererComparison_NotEqual, nzFaceSide_Back);
				NzRenderer::SetStencilPassOperation(nzStencilOperation_Zero, nzFaceSide_Back);

				m_pointSpotLightProgram->SendBoolean(m_pointSpotLightProgramDiscardLocation, false);

				NzRenderer::DrawIndexedPrimitives(nzPrimitiveMode_TriangleList, 0, indexBuffer->GetIndexCount());
			}

			if (m_lightMeshesDrawing)
			{
				NzRenderer::Enable(nzRendererParameter_DepthBuffer, true);
				NzRenderer::Enable(nzRendererParameter_DepthWrite, true);
				NzRenderer::Enable(nzRendererParameter_FaceCulling, false);
				NzRenderer::Enable(nzRendererParameter_StencilTest, false);
				NzRenderer::SetFaceFilling(nzFaceFilling_Line);

				NzShaderProgramManagerParams params;
				params.flags = nzShaderFlags_None;
				params.target = nzShaderTarget_Model;
				params.model.alphaMapping = false;
				params.model.alphaTest = false;
				params.model.diffuseMapping = false;
				params.model.emissiveMapping = false;
				params.model.lighting = false;
				params.model.normalMapping = false;
				params.model.parallaxMapping = false;
				params.model.specularMapping = false;

				const NzShaderProgram* program = NzShaderProgramManager::Get(params);
				NzRenderer::SetShaderProgram(program);
				for (const NzLight* light : m_renderQueue->spotLights)
				{
					float baseRadius = light->GetRadius()*std::tan(NzDegreeToRadian(light->GetOuterAngle()))*1.1f;
					lightMatrix.MakeTransform(light->GetPosition(), light->GetRotation(), NzVector3f(baseRadius, baseRadius, light->GetRadius()));

					NzRenderer::SetMatrix(nzMatrixType_World, lightMatrix);

					program->SendColor(program->GetUniformLocation(nzShaderUniform_MaterialDiffuse), light->GetColor());

					NzRenderer::DrawIndexedPrimitives(nzPrimitiveMode_TriangleList, 0, indexBuffer->GetIndexCount());
				}

				NzRenderer::Enable(nzRendererParameter_DepthBuffer, false);
				NzRenderer::Enable(nzRendererParameter_DepthWrite, false);
				NzRenderer::Enable(nzRendererParameter_FaceCulling, true);
				NzRenderer::Enable(nzRendererParameter_StencilTest, true);
				NzRenderer::SetFaceFilling(nzFaceFilling_Fill);
			}
		}

		NzRenderer::Enable(nzRendererParameter_StencilTest, false);
	}

	return true;
}
