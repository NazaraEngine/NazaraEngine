// Copyright (C) 2013 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/DeferredRenderTechnique.hpp>
#include <Nazara/Core/ErrorFlags.hpp>
#include <Nazara/Graphics/AbstractBackground.hpp>
#include <Nazara/Graphics/Camera.hpp>
#include <Nazara/Graphics/Drawable.hpp>
#include <Nazara/Graphics/Light.hpp>
#include <Nazara/Graphics/Sprite.hpp>
#include <Nazara/Renderer/Config.hpp>
#include <Nazara/Renderer/Material.hpp>
#include <Nazara/Renderer/OpenGL.hpp>
#include <Nazara/Renderer/Renderer.hpp>
#include <Nazara/Renderer/ShaderProgramManager.hpp>
#include <Nazara/Utility/BufferMapper.hpp>
#include <Nazara/Utility/StaticMesh.hpp>
#include <Nazara/Utility/VertexStruct.hpp>
#include <limits>
#include <memory>
#include <random>
#include <Nazara/Graphics/Debug.hpp>

namespace
{
	NzShaderProgram* BuildClearProgram()
	{
		const char fragmentSource[] = {
			#include <Nazara/Graphics/Resources/DeferredShading/Shaders/ClearGBuffer.frag.h>
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

		if (!program->LoadShader(nzShaderType_Fragment, NzString(fragmentSource, sizeof(fragmentSource))))
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

	NzShaderProgram* BuildDirectionalLightProgram()
	{
		const char fragmentSource[] = {
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

		if (!program->LoadShader(nzShaderType_Fragment, NzString(fragmentSource, sizeof(fragmentSource))))
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

	NzShaderProgram* BuildPointLightProgram()
	{
		const char fragmentSource[] = {
			#include <Nazara/Graphics/Resources/DeferredShading/Shaders/PointLight.frag.h>
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

		if (!program->LoadShader(nzShaderType_Fragment, NzString(fragmentSource, sizeof(fragmentSource))))
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

	NzShaderProgram* BuildSpotLightProgram()
	{
		const char fragmentSource[] = {
			#include <Nazara/Graphics/Resources/DeferredShading/Shaders/SpotLight.frag.h>
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

		if (!program->LoadShader(nzShaderType_Fragment, NzString(fragmentSource, sizeof(fragmentSource))))
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

	NzShaderProgram* BuildBloomBrightProgram()
	{
		const char fragmentSource[] = {
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

		if (!program->LoadShader(nzShaderType_Fragment, NzString(fragmentSource, sizeof(fragmentSource))))
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

	NzShaderProgram* BuildGaussianBlurProgram()
	{
		const char fragmentSource[] = {
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

		if (!program->LoadShader(nzShaderType_Fragment, NzString(fragmentSource, sizeof(fragmentSource))))
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
		const char fragmentSource[] = {
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

		if (!program->LoadShader(nzShaderType_Fragment, NzString(fragmentSource, sizeof(fragmentSource))))
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

	NzShaderProgram* BuildAAProgram()
	{
		const char fragmentSource[] = {
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

		if (!program->LoadShader(nzShaderType_Fragment, NzString(fragmentSource, sizeof(fragmentSource))))
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

		program->SendInteger(program->GetUniformLocation("GBuffer1"), 0);
		program->SendInteger(program->GetUniformLocation("NoiseTexture"), 1);

		return program.release();
	}

	NzShaderProgram* BuildSSAOProgram()
	{
		const char fragmentSource[] = {
			#include <Nazara/Graphics/Resources/DeferredShading/Shaders/SSAO.frag.h>
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

		if (!program->LoadShader(nzShaderType_Fragment, NzString(fragmentSource, sizeof(fragmentSource))))
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

		program->SendInteger(program->GetUniformLocation("GBuffer1"), 0);
		program->SendInteger(program->GetUniformLocation("NoiseTexture"), 1);

		return program.release();
	}

	NzShaderProgram* BuildSSAOFinalProgram()
	{
		const char fragmentSource[] = {
			#include <Nazara/Graphics/Resources/DeferredShading/Shaders/SSAOFinal.frag.h>
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

		if (!program->LoadShader(nzShaderType_Fragment, NzString(fragmentSource, sizeof(fragmentSource))))
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
		program->SendInteger(program->GetUniformLocation("SSAOTexture"), 1);

		return program.release();
	}

	// http://digitalerr0r.wordpress.com/2009/05/16/xna-shader-programming-tutorial-20-depth-of-field/
	NzShaderProgram* BuildDepthOfFieldProgram()
	{
		const char* fragmentSource =
		"#version 140\n"

		"out vec4 RenderTarget0;\n"

		"uniform sampler2D BlurTexture;\n"
		"uniform sampler2D ColorTexture;\n"
		"uniform sampler2D GBuffer1;\n"
		"uniform vec2 InvTargetSize;" "\n"

		"float Distance = 30.0;\n"
		"float Range = 100.0;\n"
		"float Near = 0.1;\n"
		"float Far = (5000.0) / (5000.0 - 0.1);\n"
		//"float Far = 50.0;\n"

		"void main()\n"
		"{\n"
			"vec2 texCoord = gl_FragCoord.xy * InvTargetSize;\n"

			"// Get our original pixel from ColorMap\n"
			"vec3 color = textureLod(ColorTexture, texCoord, 0.0).rgb;\n"

			"// Get our bloom pixel from bloom texture\n"
			"vec3 blur = textureLod(BlurTexture, texCoord, 0.0).rgb;\n"

			"float depth = textureLod(GBuffer1, texCoord, 0.0).w;\n"
			"depth = (2.0 * 0.1) / (5000.0 + 0.1 - depth * (5000.0 - 0.1));"
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
		std::unique_ptr<NzShaderProgram> program(new NzShaderProgram(nzShaderLanguage_GLSL));
		program->SetPersistent(false);

		if (!program->LoadShader(nzShaderType_Fragment, fragmentSource))
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
		program->SendInteger(program->GetUniformLocation("BlurTexture"), 1);
		program->SendInteger(program->GetUniformLocation("GBuffer1"), 2);

		return program.release();
	}

	NzShaderProgram* BuildBlitProgram()
	{
		const char fragmentSource[] = {
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

		if (!program->LoadShader(nzShaderType_Fragment, NzString(fragmentSource, sizeof(fragmentSource))))
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

NzDeferredRenderTechnique::NzDeferredRenderTechnique() :
m_renderQueue(static_cast<NzForwardRenderQueue*>(m_forwardTechnique.GetRenderQueue())),
m_GBufferSize(0, 0),
m_texturesUpdated(false)
{
	m_aaProgram = BuildAAProgram();

	m_blitProgram = BuildBlitProgram();

	m_bloomBrightProgram = BuildBloomBrightProgram();
	m_bloomFinalProgram = BuildBloomFinalProgram();

	m_clearProgram = BuildClearProgram();

	m_clearStates.parameters[nzRendererParameter_DepthBuffer] = true;
	m_clearStates.parameters[nzRendererParameter_FaceCulling] = true;
	m_clearStates.parameters[nzRendererParameter_StencilTest] = true;
	m_clearStates.depthFunc = nzRendererComparison_Always;
	m_clearStates.frontFace.stencilCompare = nzRendererComparison_Always;
	m_clearStates.frontFace.stencilPass = nzStencilOperation_Zero;

	m_gaussianBlurProgram = BuildGaussianBlurProgram();
	m_gaussianBlurProgramFilterLocation = m_gaussianBlurProgram->GetUniformLocation("Filter");

	m_directionalLightProgram = BuildDirectionalLightProgram();
	m_pointLightProgram = BuildPointLightProgram();
	m_spotLightProgram = BuildSpotLightProgram();

	m_depthOfFieldProgram = BuildDepthOfFieldProgram();

	m_bilinearSampler.SetAnisotropyLevel(1);
	m_bilinearSampler.SetFilterMode(nzSamplerFilter_Bilinear);
	m_bilinearSampler.SetWrapMode(nzSamplerWrap_Clamp);

	m_pointSampler.SetAnisotropyLevel(1);
	m_pointSampler.SetFilterMode(nzSamplerFilter_Nearest);
	m_pointSampler.SetWrapMode(nzSamplerWrap_Clamp);

	m_ssaoSampler.SetAnisotropyLevel(1);
	m_ssaoSampler.SetFilterMode(nzSamplerFilter_Nearest);
	m_ssaoSampler.SetWrapMode(nzSamplerWrap_Repeat);

	/*const unsigned int kernelSize = 16;

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dis(-1.f, 1.f);
    std::uniform_real_distribution<float> dis2(0.f, 1.f);

	std::vector<NzVector3f> kernel(kernelSize);
	for (unsigned int i = 0; i < kernelSize; ++i)
	{
		kernel[i].Set(dis(gen), dis(gen), dis2(gen));
		kernel[i].Normalize();

		float scale = static_cast<float>(i) / kernelSize;
		kernel[i] *= NzLerp(0.1f, 1.0f, scale * scale);
	}

	int noiseDataSize = 4 * 4;

	std::vector<nzUInt8> noiseData(noiseDataSize*2);
	for (int i = 0; i < noiseDataSize; ++i)
	{
		NzVector2f vec(dis(gen), dis(gen));
		vec.Normalize();

		noiseData[i*2 + 0] = static_cast<nzUInt8>((vec.x*0.5f + 0.5f) * 0xFF);
		noiseData[i*2 + 1] = static_cast<nzUInt8>((vec.y*0.5f + 0.5f) * 0xFF);
	}

	m_ssaoNoiseTexture = new NzTexture;
	m_ssaoNoiseTexture->SetPersistent(false);
	m_ssaoNoiseTexture->Create(nzImageType_2D, nzPixelFormat_RG8, 4, 4);
	m_ssaoNoiseTexture->Update(&noiseData[0]);*/

	const char noiseTexture[] = {
		#include <Nazara/Graphics/Resources/DeferredShading/Textures/ssaoNoise.jpg.h>
	};

	m_ssaoNoiseTexture = new NzTexture;
	m_ssaoNoiseTexture->SetPersistent(false);
	m_ssaoNoiseTexture->LoadFromMemory(noiseTexture, sizeof(noiseTexture));

	m_ssaoProgram = BuildSSAOProgram();
	m_ssaoProgram->SendInteger(m_ssaoProgram->GetUniformLocation("NoiseTextureSize"), m_ssaoNoiseTexture->GetWidth());
	//m_ssaoProgram->SendInteger(m_ssaoProgram->GetUniformLocation("ssaoKernelSize"), kernelSize);
	//m_ssaoProgram->SendVectorArray(m_ssaoProgram->GetUniformLocation("ssaoKernelOffsets"), &kernel[0], kernelSize);

	m_ssaoFinalProgram = BuildSSAOFinalProgram();

	m_sphere = new NzMesh;
	m_sphere->SetPersistent(false);
	m_sphere->CreateStatic();
	m_sphereMesh = static_cast<NzStaticMesh*>(m_sphere->BuildSubMesh(NzPrimitive::IcoSphere(1.f, 1)));

	m_bloomTextureA = new NzTexture;
	m_bloomTextureA->SetPersistent(false);

	m_bloomTextureB = new NzTexture;
	m_bloomTextureB->SetPersistent(false);

	m_dofTextureA = new NzTexture;
	m_dofTextureA->SetPersistent(false);

	m_dofTextureB = new NzTexture;
	m_dofTextureB->SetPersistent(false);

	m_ssaoTextureA = new NzTexture;
	m_ssaoTextureA->SetPersistent(false);

	m_ssaoTextureB = new NzTexture;
	m_ssaoTextureB->SetPersistent(false);

	m_workTextureA = new NzTexture;
	m_workTextureA->SetPersistent(false);

	m_workTextureB = new NzTexture;
	m_workTextureB->SetPersistent(false);

	for (unsigned int i = 0; i < 3; ++i)
	{
		m_GBuffer[i] = new NzTexture;
		m_GBuffer[i]->SetPersistent(false);
	}
}

NzDeferredRenderTechnique::~NzDeferredRenderTechnique()
{
}

void NzDeferredRenderTechnique::Clear(const NzScene* scene)
{
	NazaraUnused(scene);
}

bool NzDeferredRenderTechnique::Draw(const NzScene* scene)
{
	NzAbstractViewer* viewer = scene->GetViewer();
	NzRecti viewerViewport = NzRenderer::GetViewport();

	if (static_cast<unsigned int>(viewerViewport.width) != m_GBufferSize.x || static_cast<unsigned int>(viewerViewport.height) != m_GBufferSize.y)
	{
		m_GBufferSize.Set(viewerViewport.width, viewerViewport.height);
		m_texturesUpdated = false;
	}

	if (!m_texturesUpdated && !UpdateTextures())
	{
		NazaraError("Failed to update RTT");
		return false;
	}

	NzRenderer::SetTarget(&m_geometryRTT);
	NzRenderer::SetViewport(NzRecti(0, 0, m_GBufferSize.x, m_GBufferSize.y));

	/****************************Passe géométrique****************************/
	m_geometryRTT.SetColorTargets({0, 1, 2}); // G-Buffer

	NzRenderer::SetRenderStates(m_clearStates);
	NzRenderer::SetShaderProgram(m_clearProgram);
	NzRenderer::DrawFullscreenQuad();

	NzRenderer::SetMatrix(nzMatrixType_Projection, viewer->GetProjectionMatrix());
	NzRenderer::SetMatrix(nzMatrixType_View, viewer->GetViewMatrix());

	GeomPass(scene);

	/****************************Passe d'éclairage****************************/
	m_geometryRTT.SetColorTarget(4); // workTextureA

	NzRenderer::SetTexture(0, m_GBuffer[0]);
	NzRenderer::SetTextureSampler(0, m_pointSampler);

	NzRenderer::SetTexture(1, m_GBuffer[1]);
	NzRenderer::SetTextureSampler(1, m_pointSampler);

	NzRenderer::SetTexture(2, m_GBuffer[2]);
	NzRenderer::SetTextureSampler(2, m_pointSampler);

	NzRenderer::SetClearColor(NzColor::Black);
	NzRenderer::Clear(nzRendererClear_Color);

	NzRenderStates lightStates;
	lightStates.dstBlend = nzBlendFunc_One;
	lightStates.srcBlend = nzBlendFunc_One;
	lightStates.parameters[nzRendererParameter_Blend] = true;
	lightStates.parameters[nzRendererParameter_DepthBuffer] = true;
	lightStates.parameters[nzRendererParameter_DepthWrite] = false;

	// Directional lights
	if (!m_renderQueue.directionalLights.empty())
	{
		NzRenderer::SetRenderStates(lightStates);
		DirectionalLightPass(scene);
	}

	// Point lights/Spot lights
	if (!m_renderQueue.pointLights.empty() || !m_renderQueue.spotLights.empty())
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

		if (!m_renderQueue.pointLights.empty())
			PointLightPass(scene);

		if (!m_renderQueue.spotLights.empty())
			SpotLightPass(scene);

		NzRenderer::Enable(nzRendererParameter_StencilTest, false);
	}

	/******************************Passe forward******************************/
	NzAbstractBackground* background = scene->GetBackground();
	if (background)
		background->Draw(scene);

	NzRenderer::SetMatrix(nzMatrixType_Projection, viewer->GetProjectionMatrix());
	NzRenderer::SetMatrix(nzMatrixType_View, viewer->GetViewMatrix());

	m_forwardTechnique.Draw(scene);

	NzRenderStates states;
	states.parameters[nzRendererParameter_DepthBuffer] = false;

	NzRenderer::SetRenderStates(states);

	/****************************SSAO***************************/
/*	NzRenderer::SetShaderProgram(m_ssaoProgram);

	NzRenderer::SetTarget(&m_ssaoRTT);
	NzRenderer::SetViewport(NzRecti(0, 0, m_GBufferSize.x/4, m_GBufferSize.y/4));

	m_ssaoRTT.SetColorTarget(0); // ssaoTextureA

	NzRenderer::SetTexture(0, m_GBuffer[1]);
	NzRenderer::SetTexture(1, m_ssaoNoiseTexture);
	NzRenderer::SetTextureSampler(0, m_pointSampler);
	NzRenderer::SetTextureSampler(1, m_ssaoSampler);
	NzRenderer::DrawFullscreenQuad();

	NzRenderer::SetShaderProgram(m_gaussianBlurProgram);

	const unsigned int ssaoBlurPass = 2;
	for (unsigned int i = 0; i < ssaoBlurPass; ++i)
	{
		m_ssaoRTT.SetColorTarget(1); // ssaoTextureB

		m_gaussianBlurProgram->SendVector(m_gaussianBlurProgramFilterLocation, NzVector2f(1.f, 0.f));

		NzRenderer::SetTexture(0, m_ssaoTextureA);
		NzRenderer::DrawFullscreenQuad();

		m_ssaoRTT.SetColorTarget(0); // ssaoTextureA

		m_gaussianBlurProgram->SendVector(m_gaussianBlurProgramFilterLocation, NzVector2f(0.f, 1.f));

		NzRenderer::SetTexture(0, m_ssaoTextureB);
		NzRenderer::DrawFullscreenQuad();
	}

	NzRenderer::SetTarget(&m_geometryRTT);
	NzRenderer::SetViewport(NzRecti(0, 0, m_GBufferSize.x, m_GBufferSize.y));
	m_geometryRTT.SetColorTarget(5); // workTextureB

	NzRenderer::SetShaderProgram(m_ssaoFinalProgram);
	NzRenderer::SetTexture(0, m_workTextureA);
	NzRenderer::SetTexture(1, m_ssaoTextureA);
	NzRenderer::SetTextureSampler(0, m_pointSampler);
	NzRenderer::SetTextureSampler(1, m_bilinearSampler);
	NzRenderer::DrawFullscreenQuad();
*/
	/****************************AA***************************/
	NzRenderer::SetShaderProgram(m_aaProgram);

	m_geometryRTT.SetColorTarget(5); // workTextureB

	NzRenderer::SetTexture(0, m_workTextureA);
	NzRenderer::SetTextureSampler(0, m_pointSampler);
	NzRenderer::DrawFullscreenQuad();

	/****************************Bloom***************************/
	NzRenderer::SetTextureSampler(0, m_bilinearSampler);
	NzRenderer::SetTextureSampler(1, m_bilinearSampler);

	m_geometryRTT.SetColorTarget(4); // workTextureA

	NzRenderer::SetShaderProgram(m_bloomBrightProgram);

	NzRenderer::SetTexture(0, m_workTextureA);
	NzRenderer::DrawFullscreenQuad();

	NzRenderer::SetTarget(&m_bloomRTT);
	NzRenderer::SetViewport(NzRecti(0, 0, m_GBufferSize.x/8, m_GBufferSize.y/8));

	NzRenderer::SetShaderProgram(m_gaussianBlurProgram);

	const unsigned int bloomBlurPass = 5;
	for (unsigned int i = 0; i < bloomBlurPass; ++i)
	{
		m_bloomRTT.SetColorTarget(0); // bloomTextureA

		m_gaussianBlurProgram->SendVector(m_gaussianBlurProgramFilterLocation, NzVector2f(1.f, 0.f));

		NzRenderer::SetTexture(0, (i == 0) ? m_workTextureB : m_bloomTextureB);
		NzRenderer::DrawFullscreenQuad();

		m_bloomRTT.SetColorTarget(1); // bloomTextureB

		m_gaussianBlurProgram->SendVector(m_gaussianBlurProgramFilterLocation, NzVector2f(0.f, 1.f));

		NzRenderer::SetTexture(0, m_bloomTextureA);
		NzRenderer::DrawFullscreenQuad();
	}

	NzRenderer::SetTarget(&m_geometryRTT);
	NzRenderer::SetViewport(NzRecti(0, 0, m_GBufferSize.x, m_GBufferSize.y));
	m_geometryRTT.SetColorTarget(4); // workTextureA

	NzRenderer::SetShaderProgram(m_bloomFinalProgram);
	NzRenderer::SetTexture(0, m_workTextureB);
	NzRenderer::SetTexture(1, m_bloomTextureB);
	NzRenderer::DrawFullscreenQuad();

	/****************************Depth-of-Field***************************/
/*	NzRenderer::SetTextureSampler(0, m_pointSampler);
	NzRenderer::SetTextureSampler(1, m_bilinearSampler);
	NzRenderer::SetTextureSampler(2, m_pointSampler);

	NzRenderer::SetTarget(&m_dofRTT);
	NzRenderer::SetViewport(NzRecti(0, 0, m_GBufferSize.x/4, m_GBufferSize.y/4));

	NzRenderer::SetShaderProgram(m_gaussianBlurProgram);

	const unsigned int dofBlurPass = 2;
	for (unsigned int i = 0; i < dofBlurPass; ++i)
	{
		m_dofRTT.SetColorTarget(0); // dofTextureA

		m_gaussianBlurProgram->SendVector(m_gaussianBlurProgramFilterLocation, NzVector2f(1.f, 0.f));

		NzRenderer::SetTexture(0, (i == 0) ? m_workTextureA : m_dofTextureB);
		NzRenderer::DrawFullscreenQuad();

		m_dofRTT.SetColorTarget(1); // dofTextureB

		m_gaussianBlurProgram->SendVector(m_gaussianBlurProgramFilterLocation, NzVector2f(0.f, 1.f));

		NzRenderer::SetTexture(0, m_dofTextureA);
		NzRenderer::DrawFullscreenQuad();
	}

	NzRenderer::SetTarget(&m_geometryRTT);
	NzRenderer::SetViewport(NzRecti(0, 0, m_GBufferSize.x, m_GBufferSize.y));
	m_geometryRTT.SetColorTarget(5); // workTextureB

	NzRenderer::SetShaderProgram(m_depthOfFieldProgram);
	NzRenderer::SetTexture(0, m_workTextureA);
	NzRenderer::SetTexture(1, m_dofTextureB);
	NzRenderer::SetTexture(2, m_GBuffer[1]);
	NzRenderer::DrawFullscreenQuad();*/

	/*******************************Passe finale******************************/
	scene->GetViewer()->ApplyView();

	NzRenderer::SetRenderStates(states);
	NzRenderer::SetShaderProgram(m_blitProgram);
	NzRenderer::SetTexture(0, m_workTextureA);
	NzRenderer::SetTextureSampler(0, m_pointSampler);

	NzRenderer::DrawFullscreenQuad();

	return true;
}

NzTexture* NzDeferredRenderTechnique::GetGBuffer(unsigned int i) const
{
	#if NAZARA_GRAPHICS_SAFE
	if (i >= 3)
	{
		NazaraError("GBuffer texture index out of range (" + NzString::Number(i) + " >= 3)");
		return nullptr;
	}
	#endif

	return m_GBuffer[i];
}

NzAbstractRenderQueue* NzDeferredRenderTechnique::GetRenderQueue()
{
	return &m_renderQueue;
}

nzRenderTechniqueType NzDeferredRenderTechnique::GetType() const
{
	return nzRenderTechniqueType_DeferredShading;
}

NzTexture* NzDeferredRenderTechnique::GetWorkTexture(unsigned int i) const
{
	#if NAZARA_GRAPHICS_SAFE
	if (i >= 2)
	{
		NazaraError("GBuffer texture index out of range (" + NzString::Number(i) + " >= 3)");
		return nullptr;
	}
	#endif

	return (i == 0) ? m_workTextureA : m_workTextureB;

}

bool NzDeferredRenderTechnique::IsSupported()
{
	// On ne va pas s'embêter à écrire un Deferred Renderer qui ne passe pas par le MRT, ce serait lent et inutile (OpenGL 2 garanti cette fonctionnalité en plus)
	return NzRenderer::HasCapability(nzRendererCap_RenderTexture) &&
	       NzRenderer::HasCapability(nzRendererCap_MultipleRenderTargets) &&
	       NzRenderer::GetMaxColorAttachments() >= 6 && //FIXME: Repasser à quatre
	       NzRenderer::GetMaxRenderTargets() >= 4 &&
	       NzTexture::IsFormatSupported(nzPixelFormat_RGBA32F);
}

void NzDeferredRenderTechnique::GeomPass(const NzScene* scene)
{
	NzAbstractViewer* viewer = scene->GetViewer();
	const NzShaderProgram* lastProgram = nullptr;

	for (auto& matIt : m_renderQueue.opaqueModels)
	{
		bool& used = std::get<0>(matIt.second);
		if (used)
		{
			bool& renderQueueInstancing = std::get<1>(matIt.second);
			NzDeferredRenderQueue::BatchedSkeletalMeshContainer& skeletalContainer = std::get<2>(matIt.second);
			NzDeferredRenderQueue::BatchedStaticMeshContainer& staticContainer = std::get<3>(matIt.second);

			if (!skeletalContainer.empty() || !staticContainer.empty())
			{
				const NzMaterial* material = matIt.first;

				// Nous utilisons de l'instancing que lorsqu'aucune lumière (autre que directionnelle) n'est active
				// Ceci car l'instancing n'est pas compatible avec la recherche des lumières les plus proches
				// (Le deferred shading n'a pas ce problème)
				bool instancing = m_instancingEnabled && renderQueueInstancing;

				// On commence par récupérer le programme du matériau
				nzUInt32 flags = nzShaderFlags_Deferred;
				if (instancing)
					flags |= nzShaderFlags_Instancing;

				const NzShaderProgram* program = material->GetShaderProgram(nzShaderTarget_Model, flags);

				// Les uniformes sont conservées au sein d'un programme, inutile de les renvoyer tant qu'il ne change pas
				if (program != lastProgram)
				{
					NzRenderer::SetShaderProgram(program);

					// Couleur ambiante de la scène
					program->SendColor(program->GetUniformLocation(nzShaderUniform_SceneAmbient), scene->GetAmbientColor());
					// Position de la caméra
					program->SendVector(program->GetUniformLocation(nzShaderUniform_EyePosition), viewer->GetEyePosition());

					lastProgram = program;
				}

				material->Apply(program);

				// Meshs squelettiques
				/*if (!skeletalContainer.empty())
				{
					NzRenderer::SetVertexBuffer(m_skinningBuffer); // Vertex buffer commun
					for (auto& subMeshIt : container)
					{
						///TODO
					}
				}*/

				// Meshs statiques
				for (auto& subMeshIt : staticContainer)
				{
					const NzStaticMesh* mesh = subMeshIt.first;
					std::vector<NzDeferredRenderQueue::StaticData>& staticData = subMeshIt.second;

					if (!staticData.empty())
					{
						const NzIndexBuffer* indexBuffer = mesh->GetIndexBuffer();
						const NzVertexBuffer* vertexBuffer = mesh->GetVertexBuffer();

						// Gestion du draw call avant la boucle de rendu
						std::function<void(nzPrimitiveMode, unsigned int, unsigned int)> DrawFunc;
						std::function<void(unsigned int, nzPrimitiveMode, unsigned int, unsigned int)> InstancedDrawFunc;
						unsigned int indexCount;

						if (indexBuffer)
						{
							DrawFunc = NzRenderer::DrawIndexedPrimitives;
							InstancedDrawFunc = NzRenderer::DrawIndexedPrimitivesInstanced;
							indexCount = indexBuffer->GetIndexCount();
						}
						else
						{
							DrawFunc = NzRenderer::DrawPrimitives;
							InstancedDrawFunc = NzRenderer::DrawPrimitivesInstanced;
							indexCount = vertexBuffer->GetVertexCount();
						}

						NzRenderer::SetIndexBuffer(indexBuffer);
						NzRenderer::SetVertexBuffer(vertexBuffer);

						nzPrimitiveMode primitiveMode = mesh->GetPrimitiveMode();
						if (instancing)
						{
							NzVertexBuffer* instanceBuffer = NzRenderer::GetInstanceBuffer();

							instanceBuffer->SetVertexDeclaration(NzVertexDeclaration::Get(nzVertexLayout_Matrix4));

							unsigned int stride = instanceBuffer->GetStride();

							const NzDeferredRenderQueue::StaticData* data = &staticData[0];
							unsigned int instanceCount = staticData.size();
							unsigned int maxInstanceCount = instanceBuffer->GetVertexCount();

							while (instanceCount > 0)
							{
								unsigned int renderedInstanceCount = std::min(instanceCount, maxInstanceCount);
								instanceCount -= renderedInstanceCount;

								NzBufferMapper<NzVertexBuffer> mapper(instanceBuffer, nzBufferAccess_DiscardAndWrite, 0, renderedInstanceCount);
								nzUInt8* ptr = reinterpret_cast<nzUInt8*>(mapper.GetPointer());

								for (unsigned int i = 0; i < renderedInstanceCount; ++i)
								{
									std::memcpy(ptr, data->transformMatrix, sizeof(float)*16);

									data++;
									ptr += stride;
								}

								mapper.Unmap();

								InstancedDrawFunc(renderedInstanceCount, primitiveMode, 0, indexCount);
							}
						}
						else
						{
							for (const NzDeferredRenderQueue::StaticData& data : staticData)
							{
								NzRenderer::SetMatrix(nzMatrixType_World, data.transformMatrix);
								DrawFunc(primitiveMode, 0, indexCount);
							}
						}
						staticData.clear();
					}
				}
			}

			// Et on remet à zéro les données
			renderQueueInstancing = false;
			used = false;
		}
	}
}

void NzDeferredRenderTechnique::DirectionalLightPass(const NzScene* scene)
{
	NzRenderer::SetShaderProgram(m_directionalLightProgram);
	m_directionalLightProgram->SendColor(m_directionalLightProgram->GetUniformLocation(nzShaderUniform_SceneAmbient), scene->GetAmbientColor());
	m_directionalLightProgram->SendVector(m_directionalLightProgram->GetUniformLocation(nzShaderUniform_EyePosition), scene->GetViewer()->GetEyePosition());

	for (const NzLight* light : m_renderQueue.directionalLights)
	{
		light->Enable(m_directionalLightProgram, 0);
		NzRenderer::DrawFullscreenQuad();
	}
}

void NzDeferredRenderTechnique::PointLightPass(const NzScene* scene)
{
	NzRenderer::SetShaderProgram(m_pointLightProgram);
	m_pointLightProgram->SendColor(m_pointLightProgram->GetUniformLocation(nzShaderUniform_SceneAmbient), scene->GetAmbientColor());
	m_pointLightProgram->SendVector(m_pointLightProgram->GetUniformLocation(nzShaderUniform_EyePosition), scene->GetViewer()->GetEyePosition());

	const NzIndexBuffer* indexBuffer = m_sphereMesh->GetIndexBuffer();
	NzRenderer::SetIndexBuffer(indexBuffer);
	NzRenderer::SetVertexBuffer(m_sphereMesh->GetVertexBuffer());

	NzMatrix4f lightMatrix;
	lightMatrix.MakeIdentity();
	for (const NzLight* light : m_renderQueue.pointLights)
	{
		light->Enable(m_pointLightProgram, 0);
		lightMatrix.SetScale(NzVector3f(light->GetRadius()*1.1f));
		lightMatrix.SetTranslation(light->GetPosition());

		NzRenderer::SetMatrix(nzMatrixType_World, lightMatrix);

		// Rendu de la sphère dans le stencil buffer
		NzRenderer::Enable(nzRendererParameter_ColorWrite, false);
		NzRenderer::Enable(nzRendererParameter_DepthBuffer, true);
		NzRenderer::Enable(nzRendererParameter_FaceCulling, false);
		NzRenderer::SetStencilCompareFunction(nzRendererComparison_Always);

		NzRenderer::DrawIndexedPrimitives(nzPrimitiveMode_TriangleList, 0, indexBuffer->GetIndexCount());

		// Rendu de la sphère comme zone d'effet
		NzRenderer::Enable(nzRendererParameter_ColorWrite, true);
		NzRenderer::Enable(nzRendererParameter_DepthBuffer, false);
		NzRenderer::Enable(nzRendererParameter_FaceCulling, true);
		NzRenderer::SetFaceCulling(nzFaceSide_Front);
		NzRenderer::SetStencilCompareFunction(nzRendererComparison_NotEqual, nzFaceSide_Back);
		NzRenderer::SetStencilPassOperation(nzStencilOperation_Zero, nzFaceSide_Back);

		NzRenderer::DrawIndexedPrimitives(nzPrimitiveMode_TriangleList, 0, indexBuffer->GetIndexCount());
	}
}

void NzDeferredRenderTechnique::SpotLightPass(const NzScene* scene)
{
	NzRenderer::SetShaderProgram(m_spotLightProgram);
	m_spotLightProgram->SendColor(m_spotLightProgram->GetUniformLocation(nzShaderUniform_SceneAmbient), scene->GetAmbientColor());
	m_spotLightProgram->SendVector(m_spotLightProgram->GetUniformLocation(nzShaderUniform_EyePosition), scene->GetViewer()->GetEyePosition());

	const NzIndexBuffer* indexBuffer = m_sphereMesh->GetIndexBuffer();
	NzRenderer::SetIndexBuffer(indexBuffer);
	NzRenderer::SetVertexBuffer(m_sphereMesh->GetVertexBuffer());

	for (const NzLight* light : m_renderQueue.spotLights)
	{
		light->Enable(m_spotLightProgram, 0);
		NzMatrix4f lightMatrix;
		lightMatrix.MakeIdentity();
		lightMatrix.SetScale(NzVector3f(light->GetRadius()*1.1f));
		lightMatrix.SetTranslation(light->GetPosition());

		NzRenderer::SetMatrix(nzMatrixType_World, lightMatrix);

		// Rendu de la sphère dans le stencil buffer
		NzRenderer::Enable(nzRendererParameter_ColorWrite, false);
		NzRenderer::Enable(nzRendererParameter_DepthBuffer, true);
		NzRenderer::Enable(nzRendererParameter_FaceCulling, false);
		NzRenderer::SetStencilCompareFunction(nzRendererComparison_Always);

		NzRenderer::DrawIndexedPrimitives(nzPrimitiveMode_TriangleList, 0, indexBuffer->GetIndexCount());

		// Rendu de la sphère comme zone d'effet
		NzRenderer::Enable(nzRendererParameter_ColorWrite, true);
		NzRenderer::Enable(nzRendererParameter_DepthBuffer, false);
		NzRenderer::Enable(nzRendererParameter_FaceCulling, true);
		NzRenderer::SetFaceCulling(nzFaceSide_Front);
		NzRenderer::SetStencilCompareFunction(nzRendererComparison_NotEqual, nzFaceSide_Back);
		NzRenderer::SetStencilPassOperation(nzStencilOperation_Zero, nzFaceSide_Back);

		NzRenderer::DrawIndexedPrimitives(nzPrimitiveMode_TriangleList, 0, indexBuffer->GetIndexCount());
	}
}

bool NzDeferredRenderTechnique::UpdateTextures() const
{
	/*
	G-Buffer:
	Texture0: Diffuse Color + Flags
	Texture1: Normal map + Depth
	Texture2: Specular value + Shininess
	Texture3: N/A
	*/

	try
	{
		NzErrorFlags errFlags(nzErrorFlag_ThrowException);

		unsigned int width = m_GBufferSize.x;
		unsigned int height = m_GBufferSize.y;

		m_GBuffer[0]->Create(nzImageType_2D, nzPixelFormat_RGBA8, width, height);
		m_GBuffer[1]->Create(nzImageType_2D, nzPixelFormat_RGBA32F, width, height);
		m_GBuffer[2]->Create(nzImageType_2D, nzPixelFormat_RGBA8, width, height);
		m_bloomTextureA->Create(nzImageType_2D, nzPixelFormat_RGBA8, width/8, height/8);
		m_bloomTextureB->Create(nzImageType_2D, nzPixelFormat_RGBA8, width/8, height/8);
		m_dofTextureA->Create(nzImageType_2D, nzPixelFormat_RGB8, width/4, height/4);
		m_dofTextureB->Create(nzImageType_2D, nzPixelFormat_RGB8, width/4, height/4);
		m_ssaoTextureA->Create(nzImageType_2D, nzPixelFormat_R8, width/4, height/4);
		m_ssaoTextureB->Create(nzImageType_2D, nzPixelFormat_R8, width/4, height/4);
		m_workTextureA->Create(nzImageType_2D, nzPixelFormat_RGBA8, width, height);
		m_workTextureB->Create(nzImageType_2D, nzPixelFormat_RGBA8, width, height);

		m_geometryRTT.Create(true);

		// Texture 0 : Diffuse Color + Flags
		m_geometryRTT.AttachTexture(nzAttachmentPoint_Color, 0, m_GBuffer[0]);

		// Texture 1 : Normal map + Depth
		m_geometryRTT.AttachTexture(nzAttachmentPoint_Color, 1, m_GBuffer[1]);

		// Texture 2 : Specular value + Shininess
		m_geometryRTT.AttachTexture(nzAttachmentPoint_Color, 2, m_GBuffer[2]);

		// Texture 3 : Emission map ?

		// Texture 4 : Target A
		m_geometryRTT.AttachTexture(nzAttachmentPoint_Color, 4, m_workTextureA);

		// Texture 5 : Target B
		m_geometryRTT.AttachTexture(nzAttachmentPoint_Color, 5, m_workTextureB);

		// Depth/stencil buffer
		m_geometryRTT.AttachBuffer(nzAttachmentPoint_DepthStencil, 0, nzPixelFormat_Depth24Stencil8, width, height);

		m_geometryRTT.Unlock();

		m_bloomRTT.Create(true);
		m_bloomRTT.AttachTexture(nzAttachmentPoint_Color, 0, m_bloomTextureA);
		m_bloomRTT.AttachTexture(nzAttachmentPoint_Color, 1, m_bloomTextureB);
		m_bloomRTT.Unlock();

		m_dofRTT.Create(true);
		m_dofRTT.AttachTexture(nzAttachmentPoint_Color, 0, m_dofTextureA);
		m_dofRTT.AttachTexture(nzAttachmentPoint_Color, 1, m_dofTextureB);
		m_dofRTT.Unlock();

		m_ssaoRTT.Create(true);
		m_ssaoRTT.AttachTexture(nzAttachmentPoint_Color, 0, m_ssaoTextureA);
		m_ssaoRTT.AttachTexture(nzAttachmentPoint_Color, 1, m_ssaoTextureB);
		m_ssaoRTT.Unlock();

		if (!m_bloomRTT.IsComplete() || !m_dofRTT.IsComplete() || !m_geometryRTT.IsComplete() || !m_ssaoRTT.IsComplete())
		{
			NazaraError("Incomplete RTT");
			return false;
		}

		m_texturesUpdated = true;

		return true;
	}
	catch (const std::exception& e)
	{
		NazaraError("Failed to create work RTT/G-Buffer");
		return false;
	}
}
