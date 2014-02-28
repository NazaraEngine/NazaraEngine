// Copyright (C) 2014 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/DeferredDOFPass.hpp>
#include <Nazara/Graphics/AbstractViewer.hpp>
#include <Nazara/Graphics/Scene.hpp>
#include <Nazara/Renderer/Renderer.hpp>
#include <Nazara/Renderer/RenderTexture.hpp>
#include <Nazara/Renderer/ShaderLibrary.hpp>
#include <memory>
#include <Nazara/Graphics/Debug.hpp>

namespace
{
	// http://digitalerr0r.wordpress.com/2009/05/16/xna-shader-programming-tutorial-20-depth-of-field/
	NzShader* BuildDepthOfFieldShader()
	{
		const char* fragmentSource =
		"#version 140\n"

		"out vec4 RenderTarget0;\n"

		"uniform sampler2D BlurTexture;\n"
		"uniform sampler2D ColorTexture;\n"
		"uniform sampler2D GBuffer1;\n"
		"uniform vec2 InvTargetSize;" "\n"

		"float Distance = 30.0;\n"
		"float Range = 10.0;\n"
		"float Near = 0.1;\n"
		"float Far = (1000.0) / (1000.0 - 0.1);\n"
		//"float Far = 50.0;\n"

		"void main()\n"
		"{\n"
			"vec2 texCoord = gl_FragCoord.xy * InvTargetSize;\n"

			"// Get our original pixel from ColorMap\n"
			"vec3 color = textureLod(ColorTexture, texCoord, 0.0).rgb;\n"

			"// Get our bloom pixel from bloom texture\n"
			"vec3 blur = textureLod(BlurTexture, texCoord, 0.0).rgb;\n"

			"float depth = textureLod(GBuffer1, texCoord, 0.0).w;\n"
			"depth = (2.0 * 0.1) / (1000.0 + 0.1 - depth * (1000.0 - 0.1));"
			"depth = 1.0 - depth;\n"

			"float fSceneZ = ( -Near * Far ) / ( depth - Far);\n"
			"float blurFactor = clamp(abs(fSceneZ - Distance)/Range, 0.0, 1.0);\n"

			"RenderTarget0 = vec4(mix(color, blur, blurFactor), 1.0);\n"
		"}\n";

		const char* vertexSource =
		"#version 140\n"

		"in vec3 VertexPosition;\n"

		"void main()\n"
		"{\n"
		"\t" "gl_Position = vec4(VertexPosition, 1.0);" "\n"
		"}\n";

		///TODO: Remplacer ça par des ShaderNode
		std::unique_ptr<NzShader> shader(new NzShader);
		shader->SetPersistent(false);

		if (!shader->Create())
		{
				NazaraError("Failed to load create shader");
				return nullptr;
		}

		if (!shader->AttachStageFromSource(nzShaderStage_Fragment, fragmentSource))
		{
				NazaraError("Failed to load fragment shader");
				return nullptr;
		}

		if (!shader->AttachStageFromSource(nzShaderStage_Vertex, vertexSource))
		{
				NazaraError("Failed to load vertex shader");
				return nullptr;
		}

		if (!shader->Link())
		{
				NazaraError("Failed to link shader");
				return nullptr;
		}

		return shader.release();
	}
}

NzDeferredDOFPass::NzDeferredDOFPass()
{
	m_dofShader = BuildDepthOfFieldShader();
	m_dofShader->SendInteger(m_dofShader->GetUniformLocation("ColorTexture"), 0);
	m_dofShader->SendInteger(m_dofShader->GetUniformLocation("BlurTexture"), 1);
	m_dofShader->SendInteger(m_dofShader->GetUniformLocation("GBuffer1"), 2);

	m_gaussianBlurShader = NzShaderLibrary::Get("DeferredGaussianBlur");
	m_gaussianBlurShaderFilterLocation = m_gaussianBlurShader->GetUniformLocation("Filter");

	for (unsigned int i = 0; i < 2; ++i)
	{
		m_dofTextures[i] = new NzTexture;
		m_dofTextures[i]->SetPersistent(false);
	}

	m_bilinearSampler.SetAnisotropyLevel(1);
	m_bilinearSampler.SetFilterMode(nzSamplerFilter_Bilinear);
	m_bilinearSampler.SetWrapMode(nzSamplerWrap_Clamp);

	m_pointSampler.SetAnisotropyLevel(1);
	m_pointSampler.SetFilterMode(nzSamplerFilter_Nearest);
	m_pointSampler.SetWrapMode(nzSamplerWrap_Clamp);

	m_states.parameters[nzRendererParameter_DepthBuffer] = false;
}

NzDeferredDOFPass::~NzDeferredDOFPass() = default;

bool NzDeferredDOFPass::Process(const NzScene* scene, unsigned int firstWorkTexture, unsigned secondWorkTexture) const
{
	NzRenderer::SetTextureSampler(0, m_pointSampler);
	NzRenderer::SetTextureSampler(1, m_bilinearSampler);
	NzRenderer::SetTextureSampler(2, m_pointSampler);

	NzRenderer::SetTarget(&m_dofRTT);
	NzRenderer::SetViewport(NzRecti(0, 0, m_dimensions.x/4, m_dimensions.y/4));

	NzRenderer::SetShader(m_gaussianBlurShader);

	const unsigned int dofBlurPass = 2;
	for (unsigned int i = 0; i < dofBlurPass; ++i)
	{
		m_dofRTT.SetColorTarget(0); // dofTextureA

		m_gaussianBlurShader->SendVector(m_gaussianBlurShaderFilterLocation, NzVector2f(1.f, 0.f));

		NzRenderer::SetTexture(0, (i == 0) ? m_workTextures[secondWorkTexture] : static_cast<const NzTexture*>(m_dofTextures[1]));
		NzRenderer::DrawFullscreenQuad();

		m_dofRTT.SetColorTarget(1); // dofTextureB

		m_gaussianBlurShader->SendVector(m_gaussianBlurShaderFilterLocation, NzVector2f(0.f, 1.f));

		NzRenderer::SetTexture(0, m_dofTextures[0]);
		NzRenderer::DrawFullscreenQuad();
	}

	m_workRTT->SetColorTarget(firstWorkTexture);
	NzRenderer::SetTarget(m_workRTT);
	NzRenderer::SetViewport(NzRecti(0, 0, m_dimensions.x, m_dimensions.y));

	NzRenderer::SetShader(m_dofShader);
	NzRenderer::SetTexture(0, m_workTextures[secondWorkTexture]);
	NzRenderer::SetTexture(1, m_dofTextures[1]);
	NzRenderer::SetTexture(2, m_GBuffer[1]);
	NzRenderer::DrawFullscreenQuad();

	return true;
}

bool NzDeferredDOFPass::Resize(const NzVector2ui& dimensions)
{
	NzDeferredRenderPass::Resize(dimensions);

	m_dofRTT.Create(true);
	for (unsigned int i = 0; i < 2; ++i)
	{
		m_dofTextures[i]->Create(nzImageType_2D, nzPixelFormat_RGBA8, dimensions.x/4, dimensions.y/4);
		m_dofRTT.AttachTexture(nzAttachmentPoint_Color, i, m_dofTextures[i]);
	}
	m_dofRTT.Unlock();

	if (!m_dofRTT.IsComplete())
	{
		NazaraError("Incomplete RTT");
		return false;
	}

	return true;
}
