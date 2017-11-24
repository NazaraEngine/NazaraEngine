// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#ifndef NAZARA_RENDERER_OPENGL
#define NAZARA_RENDERER_OPENGL // Nécessaire pour inclure les headers OpenGL
#endif

#include <Nazara/Graphics/DeferredRenderTechnique.hpp>
#include <Nazara/Core/ErrorFlags.hpp>
#include <Nazara/Graphics/AbstractViewer.hpp>
#include <Nazara/Graphics/DeferredBloomPass.hpp>
#include <Nazara/Graphics/DeferredDOFPass.hpp>
#include <Nazara/Graphics/DeferredFinalPass.hpp>
#include <Nazara/Graphics/DeferredFogPass.hpp>
#include <Nazara/Graphics/DeferredForwardPass.hpp>
#include <Nazara/Graphics/DeferredFXAAPass.hpp>
#include <Nazara/Graphics/DeferredGeometryPass.hpp>
#include <Nazara/Graphics/DeferredPhongLightingPass.hpp>
#include <Nazara/Graphics/SceneData.hpp>
#include <Nazara/Renderer/Config.hpp>
#include <Nazara/Renderer/OpenGL.hpp>
#include <Nazara/Renderer/Renderer.hpp>
#include <Nazara/Renderer/Shader.hpp>
#include <Nazara/Renderer/ShaderStage.hpp>
#include <memory>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	namespace
	{
		const UInt8 r_fragmentSource_BloomBright[] = {
			#include <Nazara/Graphics/Resources/DeferredShading/Shaders/BloomBright.frag.h>
		};

		const UInt8 r_fragmentSource_BloomFinal[] = {
			#include <Nazara/Graphics/Resources/DeferredShading/Shaders/BloomFinal.frag.h>
		};

		const UInt8 r_fragmentSource_DirectionalLight[] = {
			#include <Nazara/Graphics/Resources/DeferredShading/Shaders/DirectionalLight.frag.h>
		};

		const UInt8 r_fragmentSource_FXAA[] = {
			#include <Nazara/Graphics/Resources/DeferredShading/Shaders/FXAA.frag.h>
		};

		const UInt8 r_fragmentSource_GBufferClear[] = {
			#include <Nazara/Graphics/Resources/DeferredShading/Shaders/GBufferClear.frag.h>
		};

		const UInt8 r_fragmentSource_GaussianBlur[] = {
			#include <Nazara/Graphics/Resources/DeferredShading/Shaders/GaussianBlur.frag.h>
		};

		const UInt8 r_fragmentSource_PointSpotLight[] = {
			#include <Nazara/Graphics/Resources/DeferredShading/Shaders/PointSpotLight.frag.h>
		};

		unsigned int RenderPassPriority[] =
		{
			6,    // RenderPassType_AA
			4,    // RenderPassType_Bloom
			7,    // RenderPassType_DOF
			0xFF, // RenderPassType_Final
			5,    // RenderPassType_Fog
			2,    // RenderPassType_Forward
			1,    // RenderPassType_Lighting
			0,    // RenderPassType_Geometry
			3,    // RenderPassType_SSAO
		};

		static_assert(sizeof(RenderPassPriority) / sizeof(unsigned int) == RenderPassType_Max + 1, "Render pass priority array is incomplete");

		/*!
		* \brief Registers the deferred shader
		* \return Reference to the newly created shader
		*
		* \param name Name of the shader
		* \param fragmentSource Raw data to fragment shader
		* \param fragmentSourceLength Size of the fragment source
		* \param vertexStage Stage of the shader
		* \param err Pointer to string to contain error message
		*/

		inline ShaderRef RegisterDeferredShader(const String& name, const UInt8* fragmentSource, unsigned int fragmentSourceLength, const ShaderStage& vertexStage, String* err)
		{
			ErrorFlags errFlags(ErrorFlag_Silent | ErrorFlag_ThrowExceptionDisabled);

			ShaderRef shader = Shader::New();
			if (!shader->Create())
			{
				err->Set("Failed to create shader: " + Error::GetLastError());
				return nullptr;
			}

			if (!shader->AttachStageFromSource(ShaderStageType_Fragment, reinterpret_cast<const char*>(fragmentSource), fragmentSourceLength))
			{
				err->Set("Failed to attach fragment stage: " + Error::GetLastError());
				return nullptr;
			}

			shader->AttachStage(ShaderStageType_Vertex, vertexStage);

			if (!shader->Link())
			{
				err->Set("Failed to link shader: " + Error::GetLastError());
				return nullptr;
			}

			ShaderLibrary::Register(name, shader);
			return shader;
		}
	}

	/*!
	* \ingroup graphics
	* \class Nz::DeferredRenderTechnique
	* \brief Graphics class that represents the technique used in deferred rendering
	*/

	/*!
	* \brief Constructs a DeferredRenderTechnique object by default
	*
	* \remark Produces a NazaraError if one pass could not be created
	*/

	DeferredRenderTechnique::DeferredRenderTechnique() :
	m_renderQueue(static_cast<ForwardRenderQueue*>(m_forwardTechnique.GetRenderQueue())),
	m_GBufferSize(0U)
	{
		m_depthStencilTexture = Texture::New();

		for (unsigned int i = 0; i < 2; ++i)
			m_workTextures[i] = Texture::New();

		for (unsigned int i = 0; i < 3; ++i)
			m_GBuffer[i] = Texture::New();
			
		try
		{
			ErrorFlags errFlags(ErrorFlag_ThrowException);

			ResetPass(RenderPassType_Final, 0);
			ResetPass(RenderPassType_Geometry, 0);
			ResetPass(RenderPassType_Lighting, 0);
		}
		catch (const std::exception& e)
		{
			ErrorFlags errFlags(ErrorFlag_ThrowExceptionDisabled);

			NazaraError("Failed to add geometry and/or phong lighting pass: " + String(e.what()));
			throw;
		}

		try
		{
			ErrorFlags errFlags(ErrorFlag_ThrowException);
			ResetPass(RenderPassType_AA, 0);
		}
		catch (const std::exception& e)
		{
			NazaraWarning("Failed to add FXAA pass: " + String(e.what()));
		}

		try
		{
			ErrorFlags errFlags(ErrorFlag_ThrowException);
			ResetPass(RenderPassType_Bloom, 0);
		}
		catch (const std::exception& e)
		{
			NazaraWarning("Failed to add bloom pass: " + String(e.what()));
		}

		try
		{
			ErrorFlags errFlags(ErrorFlag_ThrowException);

			DeferredRenderPass* dofPass = ResetPass(RenderPassType_DOF, 0);
			dofPass->Enable(false);
		}
		catch (const std::exception& e)
		{
			NazaraWarning("Failed to add DOF pass: " + String(e.what()));
		}

		try
		{
			ErrorFlags errFlags(ErrorFlag_ThrowException);

			DeferredRenderPass* fogPass = ResetPass(RenderPassType_Fog, 0);
			fogPass->Enable(false);
		}
		catch (const std::exception& e)
		{
			NazaraWarning("Failed to add fog pass: " + String(e.what()));
		}

		try
		{
			ErrorFlags errFlags(ErrorFlag_ThrowException);
			ResetPass(RenderPassType_Forward, 0);
		}
		catch (const std::exception& e)
		{
			NazaraWarning("Failed to add forward pass: " + String(e.what()));
		}

		try
		{
			ErrorFlags errFlags(ErrorFlag_ThrowException);
			ResetPass(RenderPassType_SSAO, 0);
		}
		catch (const std::exception& e)
		{
			NazaraWarning("Failed to add SSAO pass: " + String(e.what()));
		}
	}

	DeferredRenderTechnique::~DeferredRenderTechnique() = default;

	/*!
	* \brief Clears the data
	*
	* \param sceneData Data of the scene
	*/

	void DeferredRenderTechnique::Clear(const SceneData& sceneData) const
	{
		NazaraUnused(sceneData);
	}

	/*!
	* \brief Draws the data of the scene
	* \return true If successful
	*
	* \param sceneData Data of the scene
	*
	* \remark Produces a NazaraAssert if viewer of the scene is invalid
	* \remark Produces a NazaraError if updating viewport dimensions failed
	*/

	bool DeferredRenderTechnique::Draw(const SceneData& sceneData) const
	{
		NazaraAssert(sceneData.viewer, "Invalid viewer");
		Recti viewerViewport = sceneData.viewer->GetViewport();

		Vector2ui viewportDimensions(viewerViewport.width, viewerViewport.height);
		if (viewportDimensions != m_GBufferSize)
		{
			if (!Resize(viewportDimensions))
			{
				NazaraError("Failed to update RTT");
				return false;
			}
		}

		unsigned int sceneTexture = 0;
		unsigned int workTexture = 1;
		for (auto& passIt : m_passes)
		{
			for (auto& passIt2 : passIt.second)
			{
				const DeferredRenderPass* pass = passIt2.second.get();
				if (pass->IsEnabled())
				{
					if (pass->Process(sceneData, workTexture, sceneTexture))
						std::swap(workTexture, sceneTexture);
				}
			}
		}

		return true;
	}

	/*!
	* \brief Enables a pass
	*
	* \param renderPass Enumeration for the pass
	* \param position Position of the pass
	* \param enable Should the pass be enabled
	*/

	void DeferredRenderTechnique::EnablePass(RenderPassType renderPass, int position, bool enable)
	{
		auto it = m_passes.find(renderPass);
		if (it != m_passes.end())
		{
			auto it2 = it->second.find(position);
			if (it2 != it->second.end())
				it2->second->Enable(enable);
		}
	}

	/*!
	* \brief Gets the stencil buffer
	* \return Pointer to the rendering buffer
	*/

	Texture* DeferredRenderTechnique::GetDepthStencilTexture() const
	{
		return m_depthStencilTexture;
	}

	/*!
	* \brief Gets the G-buffer
	* \return Pointer to the ith texture
	*
	* \param i Index of the G-buffer
	*
	* \remark Produces a NazaraError with NAZARA_GRAPHICS_SAFE defined if index is invalid
	*/

	Texture* DeferredRenderTechnique::GetGBuffer(unsigned int i) const
	{
		#if NAZARA_GRAPHICS_SAFE
		if (i >= 3)
		{
			NazaraError("GBuffer texture index out of range (" + String::Number(i) + " >= 3)");
			return nullptr;
		}
		#endif

		return m_GBuffer[i];
	}

	/*!
	* \brief Gets the rendering texture of the G-buffer
	* \return Pointer to the rendering buffer
	*/

	RenderTexture* DeferredRenderTechnique::GetGBufferRTT() const
	{
		return &m_GBufferRTT;
	}

	/*!
	* \brief Gets the forward technique
	* \return Constant pointer to the forward technique
	*/

	const ForwardRenderTechnique* DeferredRenderTechnique::GetForwardTechnique() const
	{
		return &m_forwardTechnique;
	}

	/*!
	* \brief Gets the pass
	* \return Pointer to the deferred render pass
	*
	* \param renderPass Enumeration for the pass
	* \param position Position of the pass
	*/

	DeferredRenderPass* DeferredRenderTechnique::GetPass(RenderPassType renderPass, int position)
	{
		auto it = m_passes.find(renderPass);
		if (it != m_passes.end())
		{
			auto it2 = it->second.find(position);
			if (it2 != it->second.end())
				return it2->second.get();
		}

		return nullptr;
	}

	/*!
	* \brief Gets the render queue
	* \return Pointer to the render queue
	*/

	AbstractRenderQueue* DeferredRenderTechnique::GetRenderQueue()
	{
		return &m_renderQueue;
	}

	/*!
	* \brief Gets the type of the current technique
	* \return Type of the render technique
	*/

	RenderTechniqueType DeferredRenderTechnique::GetType() const
	{
		return RenderTechniqueType_DeferredShading;
	}

	/*!
	* \brief Gets the render texture used to work
	* \return Pointer to the rendering texture
	*/

	RenderTexture* DeferredRenderTechnique::GetWorkRTT() const
	{
		return &m_workRTT;
	}

	/*!
	* \brief Gets the ith texture to work
	* \return Pointer to the texture
	*
	* \param i Index of the texture used to work
	*
	* \remark Produces a NazaraError with NAZARA_GRAPHICS_SAFE defined if index is invalid
	*/

	Texture* DeferredRenderTechnique::GetWorkTexture(unsigned int i) const
	{
		#if NAZARA_GRAPHICS_SAFE
		if (i >= 2)
		{
			NazaraError("Work texture index out of range (" + String::Number(i) + " >= 2)");
			return nullptr;
		}
		#endif

		return m_workTextures[i];
	}

	/*!
	* \brief Checks whether the pass is enable
	* \return true If it is the case
	*
	* \param renderPass Enumeration for the pass
	* \param position Position of the pass
	*/

	bool DeferredRenderTechnique::IsPassEnabled(RenderPassType renderPass, int position)
	{
		auto it = m_passes.find(renderPass);
		if (it != m_passes.end())
		{
			auto it2 = it->second.find(position);
			if (it2 != it->second.end())
				return it2->second->IsEnabled();
		}

		return false;
	}

	/*!
	* \brief Resets the pass
	* \return Pointer to the new deferred render pass
	*
	* \param renderPass Enumeration for the pass
	* \param position Position of the pass
	*/

	DeferredRenderPass* DeferredRenderTechnique::ResetPass(RenderPassType renderPass, int position)
	{
		std::unique_ptr<DeferredRenderPass> smartPtr; // We avoid to leak in case of exception

		switch (renderPass)
		{
			case RenderPassType_AA:
				smartPtr.reset(new DeferredFXAAPass);
				break;

			case RenderPassType_Bloom:
				smartPtr.reset(new DeferredBloomPass);
				break;

			case RenderPassType_DOF:
				smartPtr.reset(new DeferredDOFPass);
				break;

			case RenderPassType_Final:
				smartPtr.reset(new DeferredFinalPass);
				break;

			case RenderPassType_Fog:
				smartPtr.reset(new DeferredFogPass);
				break;

			case RenderPassType_Forward:
				smartPtr.reset(new DeferredForwardPass);
				break;

			case RenderPassType_Geometry:
				smartPtr.reset(new DeferredGeometryPass);
				break;

			case RenderPassType_Lighting:
				smartPtr.reset(new DeferredPhongLightingPass);
				break;

			case RenderPassType_SSAO:
				//smartPtr.reset(new DeferredSSAOPass);
				break;
		}

		DeferredRenderPass* oldPass = GetPass(renderPass, position);
		if (oldPass && !oldPass->IsEnabled())
			smartPtr->Enable(false);

		SetPass(renderPass, position, smartPtr.get());
		return smartPtr.release();
	}

	/*!
	* \brief Sets the pass
	*
	* \param relativeTo Enumeration for the pass
	* \param position Position of the pass
	* \param pass Render pass to set
	*/

	void DeferredRenderTechnique::SetPass(RenderPassType relativeTo, int position, DeferredRenderPass* pass)
	{
		if (pass)
		{
			pass->Initialize(this);
			if (m_GBufferSize != Vector2ui(0U))
				pass->Resize(m_GBufferSize);

			m_passes[relativeTo][position].reset(pass);
		}
		else
			m_passes[relativeTo].erase(position);
	}

	/*!
	* \brief Checks whether the technique is supported
	* \return true if it is the case
	*/

	bool DeferredRenderTechnique::IsSupported()
	{
		// Since OpenGL 3.3 is the minimal version, the Renderer supports what it needs, but we are never sure...
		return Renderer::GetMaxColorAttachments() >= 4 && Renderer::GetMaxRenderTargets() >= 4;
	}

	/*!
	* \brief Resizes the texture sizes used for the render technique
	* \return true If successful
	*
	* \param dimensions Dimensions for the render technique
	*
	* \param Produces a NazaraError if one pass could not be resized
	*/

	bool DeferredRenderTechnique::Resize(const Vector2ui& dimensions) const
	{
		try
		{
			ErrorFlags errFlags(ErrorFlag_ThrowException);

			for (auto& passIt : m_passes)
				for (auto& passIt2 : passIt.second)
					passIt2.second->Resize(dimensions);

			m_GBufferSize = dimensions;

			return true;
		}
		catch (const std::exception& e)
		{
			NazaraError("Failed to create work RTT/G-Buffer: " + String(e.what()));
			return false;
		}
	}

	/*!
	* \brief Initializes the deferred render technique
	* \return true If successful
	*
	* \remark Produces a NazaraError if one shader creation failed
	*/

	bool DeferredRenderTechnique::Initialize()
	{
		const char vertexSource_Basic[] =
		"#version 140\n"

		"in vec3 VertexPosition;\n"
		"uniform mat4 WorldViewProjMatrix;\n"

		"void main()\n"
		"{\n"
			"gl_Position = WorldViewProjMatrix * vec4(VertexPosition, 1.0);\n"
		"}\n";

		const char vertexSource_PostProcess[] =
		"#version 140\n"

		"in vec3 VertexPosition;\n"

		"void main()\n"
		"{\n"
			"gl_Position = vec4(VertexPosition, 1.0);"
		"}\n";

		ShaderStage basicVertexStage(ShaderStageType_Vertex);
		if (!basicVertexStage.IsValid())
		{
			NazaraError("Failed to create basic vertex shader");
			return false;
		}

		basicVertexStage.SetSource(vertexSource_Basic, sizeof(vertexSource_Basic));

		if (!basicVertexStage.Compile())
		{
			NazaraError("Failed to compile basic vertex shader");
			return false;
		}


		ShaderStage ppVertexStage(ShaderStageType_Vertex);
		if (!ppVertexStage.IsValid())
		{
			NazaraError("Failed to create vertex shader");
			return false;
		}

		ppVertexStage.SetSource(vertexSource_PostProcess, sizeof(vertexSource_PostProcess));

		if (!ppVertexStage.Compile())
		{
			NazaraError("Failed to compile vertex shader");
			return false;
		}


		String error;
		Shader* shader;

		// Shaders critiques (Nécessaires pour le Deferred Shading minimal)
		shader = RegisterDeferredShader("DeferredGBufferClear", r_fragmentSource_GBufferClear, sizeof(r_fragmentSource_GBufferClear), ppVertexStage, &error);
		if (!shader)
		{
			NazaraError("Failed to register critical shader: " + error);
			return false;
		}


		shader = RegisterDeferredShader("DeferredDirectionnalLight", r_fragmentSource_DirectionalLight, sizeof(r_fragmentSource_DirectionalLight), ppVertexStage, &error);
		if (!shader)
		{
			NazaraError("Failed to register critical shader: " + error);
			return false;
		}

		shader->SendInteger(shader->GetUniformLocation("GBuffer0"), 0);
		shader->SendInteger(shader->GetUniformLocation("GBuffer1"), 1);
		shader->SendInteger(shader->GetUniformLocation("GBuffer2"), 2);
		shader->SendInteger(shader->GetUniformLocation("DepthBuffer"), 3);


		shader = RegisterDeferredShader("DeferredPointSpotLight", r_fragmentSource_PointSpotLight, sizeof(r_fragmentSource_PointSpotLight), basicVertexStage, &error);
		if (!shader)
		{
			NazaraError("Failed to register critical shader: " + error);
			return false;
		}

		shader->SendInteger(shader->GetUniformLocation("GBuffer0"), 0);
		shader->SendInteger(shader->GetUniformLocation("GBuffer1"), 1);
		shader->SendInteger(shader->GetUniformLocation("GBuffer2"), 2);
		shader->SendInteger(shader->GetUniformLocation("DepthBuffer"), 3);


		// Shaders optionnels (S'ils ne sont pas présents, le rendu minimal sera quand même assuré)
		shader = RegisterDeferredShader("DeferredBloomBright", r_fragmentSource_BloomBright, sizeof(r_fragmentSource_BloomBright), ppVertexStage, &error);
		if (shader)
			shader->SendInteger(shader->GetUniformLocation("ColorTexture"), 0);
		else
		{
			NazaraWarning("Failed to register bloom (bright pass) shader, certain features will not work: " + error);
		}


		shader = RegisterDeferredShader("DeferredBloomFinal", r_fragmentSource_BloomFinal, sizeof(r_fragmentSource_BloomFinal), ppVertexStage, &error);
		if (shader)
		{
			shader->SendInteger(shader->GetUniformLocation("ColorTexture"), 0);
			shader->SendInteger(shader->GetUniformLocation("BloomTexture"), 1);
		}
		else
		{
			NazaraWarning("Failed to register bloom (final pass) shader, certain features will not work: " + error);
		}


		shader = RegisterDeferredShader("DeferredFXAA", r_fragmentSource_FXAA, sizeof(r_fragmentSource_FXAA), ppVertexStage, &error);
		if (shader)
			shader->SendInteger(shader->GetUniformLocation("ColorTexture"), 0);
		else
		{
			NazaraWarning("Failed to register FXAA shader, certain features will not work: " + error);
		}


		shader = RegisterDeferredShader("DeferredGaussianBlur", r_fragmentSource_GaussianBlur, sizeof(r_fragmentSource_GaussianBlur), ppVertexStage, &error);
		if (shader)
			shader->SendInteger(shader->GetUniformLocation("ColorTexture"), 0);
		else
		{
			NazaraWarning("Failed to register gaussian blur shader, certain features will not work: " + error);
		}

		return true;
	}

	/*!
	* \brief Uninitializes the deferred render technique
	*/

	void DeferredRenderTechnique::Uninitialize()
	{
		ShaderLibrary::Unregister("DeferredGBufferClear");
		ShaderLibrary::Unregister("DeferredDirectionnalLight");
		ShaderLibrary::Unregister("DeferredPointSpotLight");
		ShaderLibrary::Unregister("DeferredBloomBright");
		ShaderLibrary::Unregister("DeferredBloomFinal");
		ShaderLibrary::Unregister("DeferredFXAA");
		ShaderLibrary::Unregister("DeferredGaussianBlur");
	}

	/*!
	* \brief Functor to compare two render pass
	* \return true If first render pass is "smaller" than the second one
	*
	* \param pass1 First render pass to compare
	* \param pass2 Second render pass to compare
	*/

	bool DeferredRenderTechnique::RenderPassComparator::operator()(RenderPassType pass1, RenderPassType pass2) const
	{
		return RenderPassPriority[pass1] < RenderPassPriority[pass2];
	}
}
