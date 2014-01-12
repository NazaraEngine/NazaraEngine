// Copyright (C) 2014 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/DeferredBloomPass.hpp>
#include <Nazara/Renderer/Renderer.hpp>
#include <memory>
#include <Nazara/Graphics/Debug.hpp>

namespace
{
	NzShaderProgram* BuildBloomBrightProgram()
	{
		const nzUInt8 fragmentSource[] = {
			#include <Nazara/Graphics/Resources/DeferredShading/Shaders/BloomBright.frag.h>
		};

		const char* vertexSource =
		"#version 140\n"

		"in vec3 VertexPosition;\n"

		"void main()\n"
		"{\n"
		"\t" "gl_Position = vec4(VertexPosition, 1.0);" "\n"
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

		program->SendInteger(program->GetUniformLocation("ColorTexture"), 0);

		return program.release();
	}

	NzShaderProgram* BuildBloomFinalProgram()
	{
		const nzUInt8 fragmentSource[] = {
			#include <Nazara/Graphics/Resources/DeferredShading/Shaders/BloomFinal.frag.h>
		};

		const char* vertexSource =
		"#version 140\n"

		"in vec3 VertexPosition;\n"

		"void main()\n"
		"{\n"
		"\t" "gl_Position = vec4(VertexPosition, 1.0);" "\n"
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

		program->SendInteger(program->GetUniformLocation("ColorTexture"), 0);
		program->SendInteger(program->GetUniformLocation("BloomTexture"), 1);

		return program.release();
	}

	NzShaderProgram* BuildGaussianBlurProgram()
	{
		const nzUInt8 fragmentSource[] = {
			#include <Nazara/Graphics/Resources/DeferredShading/Shaders/GaussianBlur.frag.h>
		};

		const char* vertexSource =
		"#version 140\n"

		"in vec3 VertexPosition;\n"

		"void main()\n"
		"{\n"
		"\t" "gl_Position = vec4(VertexPosition, 1.0);" "\n"
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

		program->SendInteger(program->GetUniformLocation("ColorTexture"), 0);

		return program.release();
	}
}

NzDeferredBloomPass::NzDeferredBloomPass() :
m_uniformUpdated(false),
m_brightLuminance(0.8),
m_brightMiddleGrey(0.5),
m_brightThreshold(0.8),
m_blurPassCount(5)
{
	m_bilinearSampler.SetAnisotropyLevel(1);
	m_bilinearSampler.SetFilterMode(nzSamplerFilter_Bilinear);
	m_bilinearSampler.SetWrapMode(nzSamplerWrap_Clamp);

	m_bloomBrightProgram = BuildBloomBrightProgram();
	m_bloomBrightProgram->SendInteger(m_bloomBrightProgram->GetUniformLocation("ColorTexture"), 0);

	m_bloomFinalProgram = BuildBloomFinalProgram();
	m_bloomFinalProgram->SendInteger(m_bloomFinalProgram->GetUniformLocation("BloomTexture"), 1);
	m_bloomFinalProgram->SendInteger(m_bloomFinalProgram->GetUniformLocation("ColorTexture"), 0);

	m_bloomStates.parameters[nzRendererParameter_DepthBuffer] = false;

	for (unsigned int i = 0; i < 2; ++i)
	{
		m_bloomTextures[i] = new NzTexture;
		m_bloomTextures[i]->SetPersistent(false);
	}

	m_gaussianBlurProgram = BuildGaussianBlurProgram();
	m_gaussianBlurProgramFilterLocation = m_gaussianBlurProgram->GetUniformLocation("Filter");
}

NzDeferredBloomPass::~NzDeferredBloomPass() = default;

unsigned int NzDeferredBloomPass::GetBlurPassCount() const
{
	return m_blurPassCount;
}

float NzDeferredBloomPass::GetBrightLuminance() const
{
	return m_brightLuminance;
}

float NzDeferredBloomPass::GetBrightMiddleGrey() const
{
	return m_brightMiddleGrey;
}

float NzDeferredBloomPass::GetBrightThreshold() const
{
	return m_brightThreshold;
}

NzTexture* NzDeferredBloomPass::GetTexture(unsigned int i) const
{
	#if NAZARA_GRAPHICS_SAFE
	if (i >= 2)
	{
		NazaraError("Texture index out of range (" + NzString::Number(i) + " >= 2)");
		return nullptr;
	}
	#endif

	return m_bloomTextures[i];
}

bool NzDeferredBloomPass::Process(const NzScene* scene, unsigned int firstWorkTexture, unsigned secondWorkTexture) const
{
	NazaraUnused(scene);

	NzRenderer::SetRenderStates(m_bloomStates);
	NzRenderer::SetTextureSampler(0, m_bilinearSampler);
	NzRenderer::SetTextureSampler(1, m_bilinearSampler);

	m_workRTT->SetColorTarget(firstWorkTexture);
	NzRenderer::SetTarget(m_workRTT);
	NzRenderer::SetViewport(NzRecti(0, 0, m_dimensions.x, m_dimensions.y));

	NzRenderer::SetShaderProgram(m_bloomBrightProgram);
	if (!m_uniformUpdated)
	{
		m_bloomBrightProgram->SendFloat(m_bloomBrightProgram->GetUniformLocation("BrightLuminance"), m_brightLuminance);
		m_bloomBrightProgram->SendFloat(m_bloomBrightProgram->GetUniformLocation("BrightMiddleGrey"), m_brightMiddleGrey);
		m_bloomBrightProgram->SendFloat(m_bloomBrightProgram->GetUniformLocation("BrightThreshold"), m_brightThreshold);

		m_uniformUpdated = true;
	}

	NzRenderer::SetTexture(0, m_workTextures[secondWorkTexture]);
	NzRenderer::DrawFullscreenQuad();

	NzRenderer::SetTarget(&m_bloomRTT);
	NzRenderer::SetViewport(NzRecti(0, 0, m_dimensions.x/8, m_dimensions.y/8));

	NzRenderer::SetShaderProgram(m_gaussianBlurProgram);

	for (unsigned int i = 0; i < m_blurPassCount; ++i)
	{
		m_bloomRTT.SetColorTarget(0); // bloomTextureA

		m_gaussianBlurProgram->SendVector(m_gaussianBlurProgramFilterLocation, NzVector2f(1.f, 0.f));

		NzRenderer::SetTexture(0, (i == 0) ? m_workTextures[firstWorkTexture] : static_cast<const NzTexture*>(m_bloomTextures[1]));
		NzRenderer::DrawFullscreenQuad();

		m_bloomRTT.SetColorTarget(1); // bloomTextureB

		m_gaussianBlurProgram->SendVector(m_gaussianBlurProgramFilterLocation, NzVector2f(0.f, 1.f));

		NzRenderer::SetTexture(0, m_bloomTextures[0]);
		NzRenderer::DrawFullscreenQuad();
	}

	m_workRTT->SetColorTarget(firstWorkTexture);
	NzRenderer::SetTarget(m_workRTT);
	NzRenderer::SetViewport(NzRecti(0, 0, m_dimensions.x, m_dimensions.y));

	NzRenderer::SetShaderProgram(m_bloomFinalProgram);
	NzRenderer::SetTexture(0, m_bloomTextures[1]);
	NzRenderer::SetTexture(1, m_workTextures[secondWorkTexture]);
	NzRenderer::DrawFullscreenQuad();

	return true;
}

bool NzDeferredBloomPass::Resize(const NzVector2ui& dimensions)
{
	NzDeferredRenderPass::Resize(dimensions);

	m_bloomRTT.Create(true);
	for (unsigned int i = 0; i < 2; ++i)
	{
		m_bloomTextures[i]->Create(nzImageType_2D, nzPixelFormat_RGBA8, dimensions.x/8, dimensions.y/8);
		m_bloomRTT.AttachTexture(nzAttachmentPoint_Color, i, m_bloomTextures[i]);
	}
	m_bloomRTT.Unlock();

	if (!m_bloomRTT.IsComplete())
	{
		NazaraError("Incomplete RTT");
		return false;
	}

	return true;
}

void NzDeferredBloomPass::SetBlurPassCount(unsigned int passCount)
{
	m_blurPassCount = passCount; // N'est pas une uniforme
}

void NzDeferredBloomPass::SetBrightLuminance(float luminance)
{
	m_brightLuminance = luminance;
	m_uniformUpdated = false;
}

void NzDeferredBloomPass::SetBrightMiddleGrey(float middleGrey)
{
	m_brightMiddleGrey = middleGrey;
	m_uniformUpdated = false;
}

void NzDeferredBloomPass::SetBrightThreshold(float threshold)
{
	m_brightThreshold = threshold;
	m_uniformUpdated = false;
}
