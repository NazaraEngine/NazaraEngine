// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/DeferredGeometryPass.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/ErrorFlags.hpp>
#include <Nazara/Graphics/AbstractViewer.hpp>
#include <Nazara/Graphics/DeferredRenderTechnique.hpp>
#include <Nazara/Graphics/Material.hpp>
#include <Nazara/Renderer/Renderer.hpp>
#include <Nazara/Renderer/RenderTexture.hpp>
#include <Nazara/Utility/BufferMapper.hpp>
#include <Nazara/Utility/StaticMesh.hpp>
#include <Nazara/Utility/VertexStruct.hpp>
#include <memory>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	/*!
	* \ingroup graphics
	* \class Nz::DeferredGeometryPass
	* \brief Graphics class that represents the pass for geometries in deferred rendering
	*/

	/*!
	* \brief Constructs a DeferredGeometryPass object by default
	*/

	DeferredGeometryPass::DeferredGeometryPass()
	{
		m_clearShader = ShaderLibrary::Get("DeferredGBufferClear");
		m_clearStates.depthBuffer = true;
		m_clearStates.faceCulling = true;
		m_clearStates.stencilTest = true;
		m_clearStates.depthFunc = RendererComparison_Always;
		m_clearStates.stencilCompare.front = RendererComparison_Always;
		m_clearStates.stencilPass.front = StencilOperation_Zero;
	}

	DeferredGeometryPass::~DeferredGeometryPass() = default;

	/*!
	* \brief Processes the work on the data while working with textures
	* \return false
	*
	* \param sceneData Data for the scene
	* \param firstWorkTexture Index of the first texture to work with
	* \param firstWorkTexture Index of the second texture to work with
	*/

	bool DeferredGeometryPass::Process(const SceneData& sceneData, unsigned int firstWorkTexture, unsigned int secondWorkTexture) const
	{
		NazaraAssert(sceneData.viewer, "Invalid viewer");
		NazaraUnused(firstWorkTexture);
		NazaraUnused(secondWorkTexture);

		bool instancingEnabled = m_deferredTechnique->IsInstancingEnabled();

		m_GBufferRTT->SetColorTargets({0, 1, 2}); // G-Buffer
		Renderer::SetTarget(m_GBufferRTT);
		Renderer::SetViewport(Recti(0, 0, m_dimensions.x, m_dimensions.y));

		Renderer::SetRenderStates(m_clearStates);
		Renderer::SetShader(m_clearShader);
		Renderer::DrawFullscreenQuad();


		Renderer::SetMatrix(MatrixType_Projection, sceneData.viewer->GetProjectionMatrix());
		Renderer::SetMatrix(MatrixType_View, sceneData.viewer->GetViewMatrix());

		const Shader* lastShader = nullptr;
		const ShaderUniforms* shaderUniforms = nullptr;

		for (auto& layerPair : m_renderQueue->layers)
		{
			for (auto& pipelinePair : layerPair.second.opaqueModels)
			{
				const MaterialPipeline* pipeline = pipelinePair.first;
				auto& pipelineEntry = pipelinePair.second;

				if (pipelineEntry.maxInstanceCount > 0)
				{
					bool instancing = instancingEnabled && (pipelineEntry.maxInstanceCount > NAZARA_GRAPHICS_INSTANCING_MIN_INSTANCES_COUNT);

					UInt32 flags = ShaderFlags_Deferred;
					if (instancing)
						flags |= ShaderFlags_Instancing;

					const MaterialPipeline::Instance& pipelineInstance = pipeline->Apply(flags);

					const Shader* shader = pipelineInstance.uberInstance->GetShader();

					// Uniforms are conserved in our program, there's no point to send them back until they change
					if (shader != lastShader)
					{
						// Index of uniforms in the shader
						shaderUniforms = GetShaderUniforms(shader);

						// Ambiant color of the scene
						shader->SendColor(shaderUniforms->sceneAmbient, sceneData.ambientColor);
						// Position of the camera
						shader->SendVector(shaderUniforms->eyePosition, sceneData.viewer->GetEyePosition());

						lastShader = shader;
					}

					for (auto& materialPair : pipelineEntry.materialMap)
					{
						const Material* material = materialPair.first;
						auto& matEntry = materialPair.second;

						if (matEntry.enabled)
						{
							DeferredRenderQueue::MeshInstanceContainer& meshInstances = matEntry.meshMap;

							if (!meshInstances.empty())
							{
								material->Apply(pipelineInstance);

								// Meshes
								for (auto& meshIt : meshInstances)
								{
									const MeshData& meshData = meshIt.first;
									auto& meshEntry = meshIt.second;

									std::vector<Matrix4f>& instances = meshEntry.instances;
									if (!instances.empty())
									{
										const IndexBuffer* indexBuffer = meshData.indexBuffer;
										const VertexBuffer* vertexBuffer = meshData.vertexBuffer;

										// Handle draw call before rendering loop
										Renderer::DrawCall drawFunc;
										Renderer::DrawCallInstanced instancedDrawFunc;
										unsigned int indexCount;

										if (indexBuffer)
										{
											drawFunc = Renderer::DrawIndexedPrimitives;
											instancedDrawFunc = Renderer::DrawIndexedPrimitivesInstanced;
											indexCount = indexBuffer->GetIndexCount();
										}
										else
										{
											drawFunc = Renderer::DrawPrimitives;
											instancedDrawFunc = Renderer::DrawPrimitivesInstanced;
											indexCount = vertexBuffer->GetVertexCount();
										}

										Renderer::SetIndexBuffer(indexBuffer);
										Renderer::SetVertexBuffer(vertexBuffer);

										if (instancing)
										{
											// We get the buffer for instance of Renderer and we configure it to work with matrices
											VertexBuffer* instanceBuffer = Renderer::GetInstanceBuffer();
											instanceBuffer->SetVertexDeclaration(VertexDeclaration::Get(VertexLayout_Matrix4));

											const Matrix4f* instanceMatrices = &instances[0];
											unsigned int instanceCount = instances.size();
											unsigned int maxInstanceCount = instanceBuffer->GetVertexCount(); // The number of matrices that can be hold in the buffer

											while (instanceCount > 0)
											{
												// We compute the number of instances that we will be able to show this time (Depending on the instance buffer size)
												unsigned int renderedInstanceCount = std::min(instanceCount, maxInstanceCount);
												instanceCount -= renderedInstanceCount;

												// We fill the instancing buffer with our world matrices
												instanceBuffer->Fill(instanceMatrices, 0, renderedInstanceCount, true);
												instanceMatrices += renderedInstanceCount;

												// And we show
												instancedDrawFunc(renderedInstanceCount, meshData.primitiveMode, 0, indexCount);
											}
										}
										else
										{
											// Without instancing, we must do one draw call for each instance
											// This may be faster than instancing under a threshold
											// Due to the time to modify the instancing buffer
											for (const Matrix4f& matrix : instances)
											{
												Renderer::SetMatrix(MatrixType_World, matrix);
												drawFunc(meshData.primitiveMode, 0, indexCount);
											}
										}
									}
								}
							}
						}
					}
				}
			}
		}

		return false; // We only fill the G-Buffer, the work texture are unchanged
	}

	/*!
	* \brief Resizes the texture sizes
	* \return true If successful
	*
	* \param dimensions Dimensions for the compute texture
	*/

	bool DeferredGeometryPass::Resize(const Vector2ui& dimensions)
	{
		DeferredRenderPass::Resize(dimensions);

		/*
		G-Buffer:
		Texture0: Diffuse Color + Flags
		Texture1: Encoded normal
		Texture2: Specular value + Shininess
		Texture3: N/A
		*/

		try
		{
			ErrorFlags errFlags(ErrorFlag_ThrowException);

			unsigned int width = dimensions.x;
			unsigned int height = dimensions.y;

			m_depthStencilTexture->Create(ImageType_2D, PixelFormatType_Depth24Stencil8, width, height);

			m_GBuffer[0]->Create(ImageType_2D, PixelFormatType_RGBA8, width, height); // Texture 0 : Diffuse Color + Specular
			m_GBuffer[1]->Create(ImageType_2D, PixelFormatType_RG16F, width, height); // Texture 1 : Encoded normal
			m_GBuffer[2]->Create(ImageType_2D, PixelFormatType_RGBA8, width, height); // Texture 2 : Depth (24bits) + Shininess

			m_GBufferRTT->Create(true);

			m_GBufferRTT->AttachTexture(AttachmentPoint_Color, 0, m_GBuffer[0]);
			m_GBufferRTT->AttachTexture(AttachmentPoint_Color, 1, m_GBuffer[1]);
			m_GBufferRTT->AttachTexture(AttachmentPoint_Color, 2, m_GBuffer[2]);

			// Texture 3 : Emission map ?

			m_GBufferRTT->AttachTexture(AttachmentPoint_DepthStencil, 0, m_depthStencilTexture);

			m_GBufferRTT->Unlock();

			m_workRTT->Create(true);

			for (unsigned int i = 0; i < 2; ++i)
			{
				m_workTextures[i]->Create(ImageType_2D, PixelFormatType_RGBA8, width, height);
				m_workRTT->AttachTexture(AttachmentPoint_Color, i, m_workTextures[i]);
			}

			m_workRTT->AttachTexture(AttachmentPoint_DepthStencil, 0, m_depthStencilTexture);

			m_workRTT->Unlock();

			if (!m_workRTT->IsComplete() || !m_GBufferRTT->IsComplete())
			{
				NazaraError("Incomplete RTT");
				return false;
			}

			return true;
		}
		catch (const std::exception& e)
		{
			NazaraError("Failed to create G-Buffer RTT: " + String(e.what()));
			return false;
		}
	}

	/*!
	* \brief Gets the uniforms of a shader
	* \return Uniforms of the shader
	*
	* \param shader Shader to get uniforms from
	*/

	const DeferredGeometryPass::ShaderUniforms* DeferredGeometryPass::GetShaderUniforms(const Shader* shader) const
	{
		auto it = m_shaderUniforms.find(shader);
		if (it == m_shaderUniforms.end())
		{
			ShaderUniforms uniforms;
			uniforms.shaderReleaseSlot.Connect(shader->OnShaderRelease, this, &DeferredGeometryPass::OnShaderInvalidated);
			uniforms.shaderUniformInvalidatedSlot.Connect(shader->OnShaderUniformInvalidated, this, &DeferredGeometryPass::OnShaderInvalidated);

			uniforms.eyePosition = shader->GetUniformLocation("EyePosition");
			uniforms.sceneAmbient = shader->GetUniformLocation("SceneAmbient");
			uniforms.textureOverlay = shader->GetUniformLocation("TextureOverlay");

			it = m_shaderUniforms.emplace(shader, std::move(uniforms)).first;
		}

		return &it->second;
	}

	/*!
	* \brief Handle the invalidation of a shader
	*
	* \param shader Shader being invalidated
	*/

	void DeferredGeometryPass::OnShaderInvalidated(const Shader* shader) const
	{
		m_shaderUniforms.erase(shader);
	}
}
