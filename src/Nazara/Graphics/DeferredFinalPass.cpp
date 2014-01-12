// Copyright (C) 2014 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/DeferredFinalPass.hpp>
#include <Nazara/Graphics/AbstractViewer.hpp>
#include <Nazara/Graphics/Scene.hpp>
#include <Nazara/Renderer/Renderer.hpp>
#include <Nazara/Renderer/RenderTexture.hpp>
#include <memory>
#include <Nazara/Graphics/Debug.hpp>

namespace
{
	NzShaderProgram* BuildBlitProgram()
	{
		const nzUInt8 fragmentSource[] = {
			#include <Nazara/Graphics/Resources/DeferredShading/Shaders/Blit.frag.h>
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

NzDeferredFinalPass::NzDeferredFinalPass()
{
	m_program = BuildBlitProgram();
	m_program->SendInteger(m_program->GetUniformLocation("ColorTexture"), 0);

	m_pointSampler.SetAnisotropyLevel(1);
	m_pointSampler.SetFilterMode(nzSamplerFilter_Nearest);
	m_pointSampler.SetWrapMode(nzSamplerWrap_Clamp);

	m_states.parameters[nzRendererParameter_DepthBuffer] = false;
}

NzDeferredFinalPass::~NzDeferredFinalPass() = default;

bool NzDeferredFinalPass::Process(const NzScene* scene, unsigned int firstWorkTexture, unsigned secondWorkTexture) const
{
	NazaraUnused(firstWorkTexture);

	scene->GetViewer()->ApplyView();

	NzRenderer::SetRenderStates(m_states);
	NzRenderer::SetShaderProgram(m_program);
	NzRenderer::SetTexture(0, m_workTextures[secondWorkTexture]);
	NzRenderer::SetTextureSampler(0, m_pointSampler);

	NzRenderer::DrawFullscreenQuad();

	return false;
}
