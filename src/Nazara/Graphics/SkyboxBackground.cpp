// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/SkyboxBackground.hpp>
#include <Nazara/Core/ErrorFlags.hpp>
#include <Nazara/Graphics/AbstractViewer.hpp>
#include <Nazara/Renderer/Renderer.hpp>
#include <Nazara/Utility/IndexBuffer.hpp>
#include <Nazara/Utility/VertexBuffer.hpp>
#include <Nazara/Utility/VertexDeclaration.hpp>
#include <memory>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	namespace
	{
		static IndexBufferRef s_indexBuffer;
		static RenderStates s_renderStates;
		static ShaderRef s_shader;
		static VertexBufferRef s_vertexBuffer;
	}

	/*!
	* \ingroup graphics
	* \class Nz::SkyboxBackground
	* \brief Graphics class that represents a background with a cubemap texture
	*/

	/*!
	* \brief Constructs a SkyboxBackground object with a cubemap texture
	*
	* \param cubemapTexture Cubemap texture
	*/

	SkyboxBackground::SkyboxBackground(TextureRef cubemapTexture) :
	m_movementOffset(Vector3f::Zero()),
	m_movementScale(0.f)
	{
		m_sampler.SetWrapMode(SamplerWrap_Clamp); // We don't want to see any beam

		SetTexture(std::move(cubemapTexture));
	}

	/*!
	* \brief Draws this relatively to the viewer
	*
	* \param viewer Viewer for the background
	*/

	void SkyboxBackground::Draw(const AbstractViewer* viewer) const
	{
		Matrix4f skyboxMatrix(viewer->GetViewMatrix());
		skyboxMatrix.SetTranslation(Vector3f::Zero());

		float zNear = viewer->GetZNear();

		constexpr float movementLimit = 0.05f;

		Vector3f offset = (viewer->GetEyePosition() - m_movementOffset) * -m_movementScale;
		offset.x = Clamp(offset.x, -movementLimit, movementLimit);
		offset.y = Clamp(offset.y, -movementLimit, movementLimit);
		offset.z = Clamp(offset.z, -movementLimit, movementLimit);
		offset *= zNear;

		Matrix4f world;
		world.MakeIdentity();
		world.SetScale(Vector3f(zNear));
		world.SetTranslation(offset);

		Renderer::SetIndexBuffer(s_indexBuffer);
		Renderer::SetMatrix(MatrixType_View, skyboxMatrix);
		Renderer::SetMatrix(MatrixType_World, world);
		Renderer::SetRenderStates(s_renderStates);
		Renderer::SetShader(s_shader);
		Renderer::SetTexture(0, m_texture);
		Renderer::SetTextureSampler(0, m_sampler);
		Renderer::SetVertexBuffer(s_vertexBuffer);

		Renderer::DrawIndexedPrimitives(PrimitiveMode_TriangleList, 0, 36);

		Renderer::SetMatrix(MatrixType_View, viewer->GetViewMatrix());
	}

	/*!
	* \brief Gets the background type
	* \return Type of background
	*/

	BackgroundType SkyboxBackground::GetBackgroundType() const
	{
		return BackgroundType_Skybox;
	}

	/*!
	* \brief Initializes the skybox
	* \return true If successful
	*
	* \remark Produces a NazaraError if initialization failed
	*/

	bool SkyboxBackground::Initialize()
	{
		const UInt16 indices[6 * 6] =
		{
			0, 1, 2, 0, 2, 3,
			3, 2, 6, 3, 6, 7,
			7, 6, 5, 7, 5, 4,
			4, 5, 1, 4, 1, 0,
			0, 3, 7, 0, 7, 4,
			1, 6, 2, 1, 5, 6
		};

		const float vertices[8 * 3 * sizeof(float)] =
		{
			-1.0,  1.0,  1.0,
			-1.0, -1.0,  1.0,
			 1.0, -1.0,  1.0,
			 1.0,  1.0,  1.0,
			-1.0,  1.0, -1.0,
			-1.0, -1.0, -1.0,
			 1.0, -1.0, -1.0,
			 1.0,  1.0, -1.0,
		};

		///TODO: Replace by ShaderNode (probably after Vulkan)
		const char* fragmentShaderSource =
		"#version 140\n"

		"in vec3 vTexCoord;\n"

		"out vec4 RenderTarget0;\n"

		"uniform samplerCube Skybox;\n"
		"uniform float VertexDepth;\n"

		"void main()\n"
		"{\n"
		"	RenderTarget0 = texture(Skybox, vTexCoord);\n"
		"	gl_FragDepth = VertexDepth;\n"
		"}\n";

		const char* vertexShaderSource =
		"#version 140\n"

		"in vec3 VertexPosition;\n"

		"out vec3 vTexCoord;\n"

		"uniform mat4 WorldViewProjMatrix;\n"

		"void main()\n"
		"{\n"
		"    vec4 WVPVertex = WorldViewProjMatrix * vec4(VertexPosition, 1.0);\n"
		"    gl_Position = WVPVertex.xyww;\n"
		"    vTexCoord = vec3(VertexPosition.x, VertexPosition.y, -VertexPosition.z);\n"
		"}\n";

		try
		{
			ErrorFlags flags(ErrorFlag_ThrowException, true);

			// Index buffer
			IndexBufferRef indexBuffer = IndexBuffer::New(false, 36, DataStorage_Hardware, 0);
			indexBuffer->Fill(indices, 0, 36);

			// Vertex buffer
			VertexBufferRef vertexBuffer = VertexBuffer::New(VertexDeclaration::Get(VertexLayout_XYZ), 8, DataStorage_Hardware, 0);
			vertexBuffer->Fill(vertices, 0, 8);

			// Shader
			ShaderRef shader = Shader::New();
			shader->Create();
			shader->AttachStageFromSource(ShaderStageType_Fragment, fragmentShaderSource);
			shader->AttachStageFromSource(ShaderStageType_Vertex, vertexShaderSource);
			shader->Link();

			shader->SendInteger(shader->GetUniformLocation("Skybox"), 0);
			shader->SendFloat(shader->GetUniformLocation("VertexDepth"), 1.f);

			// Renderstates
			s_renderStates.depthFunc = RendererComparison_Equal;
			s_renderStates.cullingSide = FaceSide_Front;
			s_renderStates.depthBuffer = true;
			s_renderStates.depthWrite = false;
			s_renderStates.faceCulling = true;

			// Exception-free zone
			s_indexBuffer = std::move(indexBuffer);
			s_shader = std::move(shader);
			s_vertexBuffer = std::move(vertexBuffer);
		}
		catch (const std::exception& e)
		{
			NazaraError("Failed to initialise: " + String(e.what()));
			return false;
		}

		return true;
	}

	/*!
	* \brief Uninitializes the skybox
	*/

	void SkyboxBackground::Uninitialize()
	{
		s_indexBuffer.Reset();
		s_shader.Reset();
		s_vertexBuffer.Reset();
	}
}
