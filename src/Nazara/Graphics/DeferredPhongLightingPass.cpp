// Copyright (C) 2014 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/DeferredPhongLightingPass.hpp>
#include <Nazara/Graphics/AbstractViewer.hpp>
#include <Nazara/Graphics/DeferredRenderQueue.hpp>
#include <Nazara/Graphics/Scene.hpp>
#include <Nazara/Renderer/Renderer.hpp>
#include <Nazara/Renderer/RenderTexture.hpp>
#include <Nazara/Renderer/ShaderLibrary.hpp>
#include <Nazara/Utility/StaticMesh.hpp>
#include <memory>
#include <Nazara/Graphics/Debug.hpp>

NzDeferredPhongLightingPass::NzDeferredPhongLightingPass() :
m_lightMeshesDrawing(false)
{
	m_directionalLightShader = NzShaderLibrary::Get("DeferredDirectionnalLight");

	m_directionalLightUniforms.ubo = false;
	m_directionalLightUniforms.locations.type = -1; // Type déjà connu
	m_directionalLightUniforms.locations.color = m_directionalLightShader->GetUniformLocation("LightColor");
	m_directionalLightUniforms.locations.factors = m_directionalLightShader->GetUniformLocation("LightFactors");
	m_directionalLightUniforms.locations.parameters1 = m_directionalLightShader->GetUniformLocation("LightDirection");
	m_directionalLightUniforms.locations.parameters2 = -1;
	m_directionalLightUniforms.locations.parameters3 = -1;

	m_pointSpotLightShader = NzShaderLibrary::Get("DeferredPointSpotLight");
	m_pointSpotLightShaderDiscardLocation = m_pointSpotLightShader->GetUniformLocation("Discard");

	m_pointSpotLightUniforms.ubo = false;
	m_pointSpotLightUniforms.locations.type = m_pointSpotLightShader->GetUniformLocation("LightType");
	m_pointSpotLightUniforms.locations.color = m_pointSpotLightShader->GetUniformLocation("LightColor");
	m_pointSpotLightUniforms.locations.factors = m_pointSpotLightShader->GetUniformLocation("LightFactors");
	m_pointSpotLightUniforms.locations.parameters1 = m_pointSpotLightShader->GetUniformLocation("LightParameters1");
	m_pointSpotLightUniforms.locations.parameters2 = m_pointSpotLightShader->GetUniformLocation("LightParameters2");
	m_pointSpotLightUniforms.locations.parameters3 = m_pointSpotLightShader->GetUniformLocation("LightParameters3");

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
		NzRenderer::SetShader(m_directionalLightShader);
		m_directionalLightShader->SendColor(m_directionalLightShader->GetUniformLocation(nzShaderUniform_SceneAmbient), scene->GetAmbientColor());
		m_directionalLightShader->SendVector(m_directionalLightShader->GetUniformLocation(nzShaderUniform_EyePosition), scene->GetViewer()->GetEyePosition());

		for (const NzLight* light : m_renderQueue->directionalLights)
		{
			light->Enable(m_directionalLightShader, m_directionalLightUniforms);
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

		NzRenderer::SetShader(m_pointSpotLightShader);
		m_pointSpotLightShader->SendColor(m_pointSpotLightShader->GetUniformLocation(nzShaderUniform_SceneAmbient), scene->GetAmbientColor());
		m_pointSpotLightShader->SendVector(m_pointSpotLightShader->GetUniformLocation(nzShaderUniform_EyePosition), scene->GetViewer()->GetEyePosition());

		NzMatrix4f lightMatrix;
		lightMatrix.MakeIdentity();
		if (!m_renderQueue->pointLights.empty())
		{
			const NzIndexBuffer* indexBuffer = m_sphereMesh->GetIndexBuffer();
			NzRenderer::SetIndexBuffer(indexBuffer);
			NzRenderer::SetVertexBuffer(m_sphereMesh->GetVertexBuffer());

			for (const NzLight* light : m_renderQueue->pointLights)
			{
				light->Enable(m_pointSpotLightShader, m_pointSpotLightUniforms);
				lightMatrix.SetScale(NzVector3f(light->GetRadius()*1.1f)); // Pour corriger les imperfections liées à la sphère
				lightMatrix.SetTranslation(light->GetPosition());

				NzRenderer::SetMatrix(nzMatrixType_World, lightMatrix);

				// Rendu de la sphère dans le stencil buffer
				NzRenderer::Enable(nzRendererParameter_ColorWrite, false);
				NzRenderer::Enable(nzRendererParameter_DepthBuffer, true);
				NzRenderer::Enable(nzRendererParameter_FaceCulling, false);
				NzRenderer::SetStencilCompareFunction(nzRendererComparison_Always);

				m_pointSpotLightShader->SendBoolean(m_pointSpotLightShaderDiscardLocation, true);

				NzRenderer::DrawIndexedPrimitives(nzPrimitiveMode_TriangleList, 0, indexBuffer->GetIndexCount());

				// Rendu de la sphère comme zone d'effet
				NzRenderer::Enable(nzRendererParameter_ColorWrite, true);
				NzRenderer::Enable(nzRendererParameter_DepthBuffer, false);
				NzRenderer::Enable(nzRendererParameter_FaceCulling, true);
				NzRenderer::SetStencilCompareFunction(nzRendererComparison_NotEqual, nzFaceSide_Back);
				NzRenderer::SetStencilPassOperation(nzStencilOperation_Zero, nzFaceSide_Back);

				m_pointSpotLightShader->SendBoolean(m_pointSpotLightShaderDiscardLocation, false);

				NzRenderer::DrawIndexedPrimitives(nzPrimitiveMode_TriangleList, 0, indexBuffer->GetIndexCount());
			}

			if (m_lightMeshesDrawing)
			{
				NzRenderer::Enable(nzRendererParameter_DepthBuffer, true);
				NzRenderer::Enable(nzRendererParameter_DepthWrite, true);
				NzRenderer::Enable(nzRendererParameter_FaceCulling, false);
				NzRenderer::Enable(nzRendererParameter_StencilTest, false);
				NzRenderer::SetFaceFilling(nzFaceFilling_Line);

				const NzShader* shader = NzShaderLibrary::Get("DebugSimple");
				NzRenderer::SetShader(shader);
				for (const NzLight* light : m_renderQueue->pointLights)
				{
					lightMatrix.SetScale(NzVector3f(light->GetRadius()*1.1f)); // Pour corriger les imperfections liées à la sphère
					lightMatrix.SetTranslation(light->GetPosition());

					NzRenderer::SetMatrix(nzMatrixType_World, lightMatrix);

					shader->SendColor(0, light->GetColor());

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
			const NzIndexBuffer* indexBuffer = m_coneMesh->GetIndexBuffer();
			NzRenderer::SetIndexBuffer(indexBuffer);
			NzRenderer::SetVertexBuffer(m_coneMesh->GetVertexBuffer());

			for (const NzLight* light : m_renderQueue->spotLights)
			{
				light->Enable(m_pointSpotLightShader, m_pointSpotLightUniforms);
				float radius = light->GetRadius()*std::tan(NzDegreeToRadian(light->GetOuterAngle()))*1.1f;
				lightMatrix.MakeTransform(light->GetPosition(), light->GetRotation(), NzVector3f(radius, radius, light->GetRadius()));

				NzRenderer::SetMatrix(nzMatrixType_World, lightMatrix);

				// Rendu de la sphère dans le stencil buffer
				NzRenderer::Enable(nzRendererParameter_ColorWrite, false);
				NzRenderer::Enable(nzRendererParameter_DepthBuffer, true);
				NzRenderer::Enable(nzRendererParameter_FaceCulling, false);
				NzRenderer::SetStencilCompareFunction(nzRendererComparison_Always);

				m_pointSpotLightShader->SendBoolean(m_pointSpotLightShaderDiscardLocation, true);

				NzRenderer::DrawIndexedPrimitives(nzPrimitiveMode_TriangleList, 0, indexBuffer->GetIndexCount());

				// Rendu de la sphère comme zone d'effet
				NzRenderer::Enable(nzRendererParameter_ColorWrite, true);
				NzRenderer::Enable(nzRendererParameter_DepthBuffer, false);
				NzRenderer::Enable(nzRendererParameter_FaceCulling, true);
				NzRenderer::SetFaceCulling(nzFaceSide_Front);
				NzRenderer::SetStencilCompareFunction(nzRendererComparison_NotEqual, nzFaceSide_Back);
				NzRenderer::SetStencilPassOperation(nzStencilOperation_Zero, nzFaceSide_Back);

				m_pointSpotLightShader->SendBoolean(m_pointSpotLightShaderDiscardLocation, false);

				NzRenderer::DrawIndexedPrimitives(nzPrimitiveMode_TriangleList, 0, indexBuffer->GetIndexCount());
			}

			if (m_lightMeshesDrawing)
			{
				NzRenderer::Enable(nzRendererParameter_DepthBuffer, true);
				NzRenderer::Enable(nzRendererParameter_DepthWrite, true);
				NzRenderer::Enable(nzRendererParameter_FaceCulling, false);
				NzRenderer::Enable(nzRendererParameter_StencilTest, false);
				NzRenderer::SetFaceFilling(nzFaceFilling_Line);

				const NzShader* shader = NzShaderLibrary::Get("DebugSimple");
				NzRenderer::SetShader(shader);
				for (const NzLight* light : m_renderQueue->spotLights)
				{
					float baseRadius = light->GetRadius()*std::tan(NzDegreeToRadian(light->GetOuterAngle()))*1.1f;
					lightMatrix.MakeTransform(light->GetPosition(), light->GetRotation(), NzVector3f(baseRadius, baseRadius, light->GetRadius()));

					NzRenderer::SetMatrix(nzMatrixType_World, lightMatrix);

					shader->SendColor(0, light->GetColor());

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
