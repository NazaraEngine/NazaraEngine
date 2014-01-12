// Copyright (C) 2014 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/DeferredFXAAPass.hpp>
#include <Nazara/Renderer/Renderer.hpp>
#include <Nazara/Renderer/RenderTexture.hpp>
#include <memory>
#include <Nazara/Graphics/Debug.hpp>

namespace
{
	NzShaderProgram* BuildFXAAProgram()
	{
		const nzUInt8 fragmentSource[] = {
				#include <Nazara/Graphics/Resources/DeferredShading/Shaders/FXAA.frag.h>
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

		return program.release();
	}
}

NzDeferredFXAAPass::NzDeferredFXAAPass()
{
	m_fxaaProgram = BuildFXAAProgram();
	m_fxaaProgram->SendInteger(m_fxaaProgram->GetUniformLocation("ColorTexture"), 0);

	m_pointSampler.SetAnisotropyLevel(1);
	m_pointSampler.SetFilterMode(nzSamplerFilter_Nearest);
	m_pointSampler.SetWrapMode(nzSamplerWrap_Clamp);

	m_states.parameters[nzRendererParameter_DepthBuffer] = false;
}

NzDeferredFXAAPass::~NzDeferredFXAAPass() = default;

bool NzDeferredFXAAPass::Process(const NzScene* scene, unsigned int firstWorkTexture, unsigned secondWorkTexture) const
{
	NazaraUnused(scene);

	m_workRTT->SetColorTarget(firstWorkTexture);
	NzRenderer::SetTarget(m_workRTT);
	NzRenderer::SetViewport(NzRecti(0, 0, m_dimensions.x, m_dimensions.y));

	NzRenderer::SetRenderStates(m_states);
	NzRenderer::SetShaderProgram(m_fxaaProgram);
	NzRenderer::SetTexture(0, m_workTextures[secondWorkTexture]);
	NzRenderer::SetTextureSampler(0, m_pointSampler);
	NzRenderer::DrawFullscreenQuad();

	return true;
}
