// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/SkyboxBackground.hpp>
#include <Nazara/Core/Algorithm.hpp>
#include <Nazara/Core/ErrorFlags.hpp>
#include <Nazara/Graphics/AbstractViewer.hpp>
#include <Nazara/Renderer/Renderer.hpp>
#include <Nazara/Renderer/RenderPipelineLayout.hpp>
#include <Nazara/Renderer/RenderStates.hpp>
#include <Nazara/Renderer/RenderTarget.hpp>
#include <Nazara/Renderer/Shader.hpp>
#include <Nazara/Utility/BufferMapper.hpp>
#include <Nazara/Utility/FieldOffsets.hpp>
#include <Nazara/Utility/IndexBuffer.hpp>
#include <Nazara/Utility/VertexBuffer.hpp>
#include <Nazara/Utility/VertexDeclaration.hpp>
#include <Nazara/Utility/UniformBuffer.hpp>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	namespace
	{
		static IndexBufferRef s_indexBuffer;
		static RenderStates s_renderStates;
		static ShaderRef s_shader;
		static UniformBufferRef s_skyboxData;
		static VertexBufferRef s_vertexBuffer;
		static std::size_t s_matrixOffset;
		static std::size_t s_skyboxDataSize;
		static std::size_t s_vertexDepthOffset;
		static unsigned int s_textureIndex;
		static unsigned int s_skyboxDataIndex;

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
		const UInt8 r_fragmentShaderSource[] = {
#include <Nazara/Graphics/Resources/Shaders/Skybox/core.frag.h>
		};

		const UInt8 r_vertexShaderSource[] = {
#include <Nazara/Graphics/Resources/Shaders/Skybox/core.vert.h>
		};
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

		m_skyboxData = UniformBuffer::New(s_skyboxDataSize, DataStorage::DataStorage_Hardware, BufferUsage_Dynamic);
	}

	/*!
	* \brief Draws this relatively to the viewer
	*
	* \param viewer Viewer for the background
	*/

	void SkyboxBackground::Draw(const AbstractViewer* viewer) const
	{
		const Nz::RenderTarget* target = viewer->GetTarget();
		Nz::Vector2ui targetSize = target->GetSize();

		Matrix4f projectionMatrix = Matrix4f::Perspective(45.f, float(targetSize.x) / targetSize.y, viewer->GetZNear(), viewer->GetZFar());

		Matrix4f viewMatrix(viewer->GetViewMatrix());
		viewMatrix.SetTranslation(Vector3f::Zero());

		float zNear = viewer->GetZNear();

		constexpr float movementLimit = 0.05f;

		Vector3f offset = (viewer->GetEyePosition() - m_movementOffset) * -m_movementScale;
		offset.x = Clamp(offset.x, -movementLimit, movementLimit);
		offset.y = Clamp(offset.y, -movementLimit, movementLimit);
		offset.z = Clamp(offset.z, -movementLimit, movementLimit);
		offset *= zNear;

		Matrix4f worldMatrix = Matrix4f::Scale(Vector3f(zNear));
		worldMatrix.SetTranslation(offset);

		{
			BufferMapper<UniformBuffer> uniformMapper(m_skyboxData, BufferAccess_DiscardAndWrite);
			void* ptr = uniformMapper.GetPointer();

			*AccessByOffset<Matrix4f>(ptr, s_matrixOffset) = worldMatrix * viewMatrix * projectionMatrix;
			*AccessByOffset<float>(ptr, s_vertexDepthOffset) = 1.f;
		}

		Renderer::SetIndexBuffer(s_indexBuffer);
		Renderer::SetUniformBuffer(s_skyboxDataIndex, m_skyboxData);
		Renderer::SetRenderStates(s_renderStates);
		Renderer::SetShader(s_shader);
		Renderer::SetTexture(s_textureIndex, m_texture);
		Renderer::SetTextureSampler(s_textureIndex, m_sampler);
		Renderer::SetVertexBuffer(s_vertexBuffer);

		Renderer::DrawIndexedPrimitives(PrimitiveMode_TriangleList, 0, 36);
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
			shader->AttachStageFromSource(ShaderStageType_Fragment, reinterpret_cast<const char*>(r_fragmentShaderSource), sizeof(r_fragmentShaderSource));
			shader->AttachStageFromSource(ShaderStageType_Vertex, reinterpret_cast<const char*>(r_vertexShaderSource), sizeof(r_vertexShaderSource));
			shader->Link();

			auto bindings = shader->ApplyLayout(RenderPipelineLayout::New(RenderPipelineLayoutInfo{
				{
					{
						"SkyboxTexture",
						ShaderBindingType_Texture,
						ShaderStageType_Fragment,
						0
					},
					{
						"SkyboxData",
						ShaderBindingType_UniformBuffer,
						ShaderStageType_Fragment | ShaderStageType_Vertex,
						1
					},
				}
			}));

			s_textureIndex = bindings[0];
			s_skyboxDataIndex = bindings[1];

			// UBO
			FieldOffsets skyboxDataStruct(StructLayout_Std140);
			s_matrixOffset = skyboxDataStruct.AddMatrix(StructFieldType_Float1, 4, 4, true);
			s_vertexDepthOffset = skyboxDataStruct.AddField(StructFieldType_Float1);
			s_skyboxDataSize = skyboxDataStruct.GetSize();

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
		s_skyboxData.Reset();
		s_vertexBuffer.Reset();
	}
}
