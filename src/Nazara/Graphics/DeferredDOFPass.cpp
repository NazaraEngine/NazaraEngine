// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/DeferredDOFPass.hpp>
#include <Nazara/Renderer/Renderer.hpp>
#include <Nazara/Renderer/RenderTexture.hpp>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	namespace
	{
		/*!
		* \brief Builds the shader for the depth of field
		* \return Reference to the shader newly created
		*/
		// http://digitalerr0r.wordpress.com/2009/05/16/xna-shader-programming-tutorial-20-depth-of-field/
		ShaderRef BuildDepthOfFieldShader()
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
			ShaderRef shader = Shader::New();
			if (!shader->Create())
			{
					NazaraError("Failed to load create shader");
					return nullptr;
			}

			if (!shader->AttachStageFromSource(ShaderStageType_Fragment, fragmentSource))
			{
					NazaraError("Failed to load fragment shader");
					return nullptr;
			}

			if (!shader->AttachStageFromSource(ShaderStageType_Vertex, vertexSource))
			{
					NazaraError("Failed to load vertex shader");
					return nullptr;
			}

			if (!shader->Link())
			{
					NazaraError("Failed to link shader");
					return nullptr;
			}

			return shader;
		}
	}

	/*!
	* \ingroup graphics
	* \class Nz::DeferredDOFPass
	* \brief Graphics class that represents the pass for depth of field in deferred rendering
	*/

	/*!
	* \brief Constructs a DeferredDOFPass object by default
	*/

	DeferredDOFPass::DeferredDOFPass()
	{
		m_dofShader = BuildDepthOfFieldShader();
		m_dofShader->SendInteger(m_dofShader->GetUniformLocation("ColorTexture"), 0);
		m_dofShader->SendInteger(m_dofShader->GetUniformLocation("BlurTexture"), 1);
		m_dofShader->SendInteger(m_dofShader->GetUniformLocation("GBuffer1"), 2);

		m_gaussianBlurShader = ShaderLibrary::Get("DeferredGaussianBlur");
		m_gaussianBlurShaderFilterLocation = m_gaussianBlurShader->GetUniformLocation("Filter");

		for (unsigned int i = 0; i < 2; ++i)
			m_dofTextures[i] = Texture::New();

		m_bilinearSampler.SetAnisotropyLevel(1);
		m_bilinearSampler.SetFilterMode(SamplerFilter_Bilinear);
		m_bilinearSampler.SetWrapMode(SamplerWrap_Clamp);

		m_pointSampler.SetAnisotropyLevel(1);
		m_pointSampler.SetFilterMode(SamplerFilter_Nearest);
		m_pointSampler.SetWrapMode(SamplerWrap_Clamp);

		m_states.depthBuffer = false;
	}

	DeferredDOFPass::~DeferredDOFPass() = default;

	/*!
	* \brief Processes the work on the data while working with textures
	* \return true
	*
	* \param sceneData Data for the scene
	* \param firstWorkTexture Index of the first texture to work with
	* \param firstWorkTexture Index of the second texture to work with
	*/

	bool DeferredDOFPass::Process(const SceneData& sceneData, unsigned int firstWorkTexture, unsigned int secondWorkTexture) const
	{
		NazaraUnused(sceneData);

		Renderer::SetTextureSampler(0, m_pointSampler);
		Renderer::SetTextureSampler(1, m_bilinearSampler);
		Renderer::SetTextureSampler(2, m_pointSampler);

		Renderer::SetTarget(&m_dofRTT);
		Renderer::SetViewport(Recti(0, 0, m_dimensions.x/4, m_dimensions.y/4));

		Renderer::SetShader(m_gaussianBlurShader);

		const unsigned int dofBlurPass = 2;
		for (unsigned int i = 0; i < dofBlurPass; ++i)
		{
			m_dofRTT.SetColorTarget(0); // dofTextureA

			m_gaussianBlurShader->SendVector(m_gaussianBlurShaderFilterLocation, Vector2f(1.f, 0.f));

			Renderer::SetTexture(0, (i == 0) ? m_workTextures[secondWorkTexture] : static_cast<const Texture*>(m_dofTextures[1]));
			Renderer::DrawFullscreenQuad();

			m_dofRTT.SetColorTarget(1); // dofTextureB

			m_gaussianBlurShader->SendVector(m_gaussianBlurShaderFilterLocation, Vector2f(0.f, 1.f));

			Renderer::SetTexture(0, m_dofTextures[0]);
			Renderer::DrawFullscreenQuad();
		}

		m_workRTT->SetColorTarget(firstWorkTexture);
		Renderer::SetTarget(m_workRTT);
		Renderer::SetViewport(Recti(0, 0, m_dimensions.x, m_dimensions.y));

		Renderer::SetShader(m_dofShader);
		Renderer::SetTexture(0, m_workTextures[secondWorkTexture]);
		Renderer::SetTexture(1, m_dofTextures[1]);
		Renderer::SetTexture(2, m_GBuffer[1]);
		Renderer::DrawFullscreenQuad();

		return true;
	}

	/*!
	* \brief Resizes the texture sizes
	* \return true If successful
	*
	* \param dimensions Dimensions for the compute texture
	*/

	bool DeferredDOFPass::Resize(const Vector2ui& dimensions)
	{
		DeferredRenderPass::Resize(dimensions);

		m_dofRTT.Create(true);
		for (unsigned int i = 0; i < 2; ++i)
		{
			m_dofTextures[i]->Create(ImageType_2D, PixelFormatType_RGBA8, dimensions.x/4, dimensions.y/4);
			m_dofRTT.AttachTexture(AttachmentPoint_Color, i, m_dofTextures[i]);
		}
		m_dofRTT.Unlock();

		if (!m_dofRTT.IsComplete())
		{
			NazaraError("Incomplete RTT");
			return false;
		}

		return true;
	}
}
