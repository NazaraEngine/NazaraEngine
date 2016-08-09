// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/DepthRenderTechnique.hpp>
#include <Nazara/Core/ErrorFlags.hpp>
#include <Nazara/Core/OffsetOf.hpp>
#include <Nazara/Graphics/AbstractBackground.hpp>
#include <Nazara/Graphics/AbstractViewer.hpp>
#include <Nazara/Graphics/Drawable.hpp>
#include <Nazara/Graphics/Light.hpp>
#include <Nazara/Graphics/Material.hpp>
#include <Nazara/Graphics/Sprite.hpp>
#include <Nazara/Renderer/Config.hpp>
#include <Nazara/Renderer/Renderer.hpp>
#include <Nazara/Utility/BufferMapper.hpp>
#include <Nazara/Utility/StaticMesh.hpp>
#include <Nazara/Utility/VertexStruct.hpp>
#include <limits>
#include <memory>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	namespace
	{
		struct BillboardPoint
		{
			Color color;
			Vector3f position;
			Vector2f size;
			Vector2f sinCos; // must follow `size` (both will be sent as a Vector4f)
			Vector2f uv;
		};

		unsigned int s_maxQuads = std::numeric_limits<UInt16>::max() / 6;
		unsigned int s_vertexBufferSize = 4 * 1024 * 1024; // 4 MiB
	}

	/*!
	* \ingroup graphics
	* \class Nz::DepthRenderTechnique
	* \brief Graphics class that represents the technique used in depth rendering
	*/

	/*!
	* \brief Constructs a DepthRenderTechnique object by default
	*/

	DepthRenderTechnique::DepthRenderTechnique() :
		m_vertexBuffer(BufferType_Vertex)
	{
		ErrorFlags flags(ErrorFlag_ThrowException, true);

		std::array<UInt8, 4> whitePixel = {255, 255, 255, 255};
		m_whiteTexture.Create(ImageType_2D, PixelFormatType_RGBA8, 1, 1);
		m_whiteTexture.Update(whitePixel.data());

		m_vertexBuffer.Create(s_vertexBufferSize, DataStorage_Hardware, BufferUsage_Dynamic);

		m_billboardPointBuffer.Reset(&s_billboardVertexDeclaration, &m_vertexBuffer);
		m_spriteBuffer.Reset(VertexDeclaration::Get(VertexLayout_XYZ_Color_UV), &m_vertexBuffer);
	}

	/*!
	* \brief Clears the data
	*
	* \param sceneData Data of the scene
	*/

	void DepthRenderTechnique::Clear(const SceneData& sceneData) const
	{
		Renderer::Enable(RendererParameter_DepthBuffer, true);
		Renderer::Enable(RendererParameter_DepthWrite, true);
		Renderer::Clear(RendererBuffer_Depth);

		// Just in case the background does render depth
		//if (sceneData.background)
		//	sceneData.background->Draw(sceneData.viewer);
	}

	/*!
	* \brief Draws the data of the scene
	* \return true If successful
	*
	* \param sceneData Data of the scene
	*/

	bool DepthRenderTechnique::Draw(const SceneData& sceneData) const
	{
		for (auto& pair : m_renderQueue.layers)
		{
			ForwardRenderQueue::Layer& layer = pair.second;

			if (!layer.opaqueModels.empty())
				DrawOpaqueModels(sceneData, layer);

			if (!layer.basicSprites.empty())
				DrawBasicSprites(sceneData, layer);

			if (!layer.billboards.empty())
				DrawBillboards(sceneData, layer);

			for (const Drawable* drawable : layer.otherDrawables)
				drawable->Draw();
		}

		return true;
	}

	/*!
	* \brief Gets the render queue
	* \return Pointer to the render queue
	*/

	AbstractRenderQueue* DepthRenderTechnique::GetRenderQueue()
	{
		return &m_renderQueue;
	}

	/*!
	* \brief Gets the type of the current technique
	* \return Type of the render technique
	*/

	RenderTechniqueType DepthRenderTechnique::GetType() const
	{
		return RenderTechniqueType_Depth;
	}

	/*!
	* \brief Initializes the depth render technique
	* \return true If successful
	*
	* \remark Produces a NazaraError if one shader creation failed
	*/

	bool DepthRenderTechnique::Initialize()
	{
		try
		{
			ErrorFlags flags(ErrorFlag_ThrowException, true);

			s_quadIndexBuffer.Reset(false, s_maxQuads * 6, DataStorage_Hardware, BufferUsage_Static);

			BufferMapper<IndexBuffer> mapper(s_quadIndexBuffer, BufferAccess_WriteOnly);
			UInt16* indices = static_cast<UInt16*>(mapper.GetPointer());

			for (unsigned int i = 0; i < s_maxQuads; ++i)
			{
				*indices++ = i * 4 + 0;
				*indices++ = i * 4 + 2;
				*indices++ = i * 4 + 1;

				*indices++ = i * 4 + 2;
				*indices++ = i * 4 + 3;
				*indices++ = i * 4 + 1;
			}

			mapper.Unmap(); // Inutile de garder le buffer ouvert plus longtemps

			// Quad buffer (utilisé pour l'instancing de billboard et de sprites)
			//Note: Les UV sont calculés dans le shader
			s_quadVertexBuffer.Reset(VertexDeclaration::Get(VertexLayout_XY), 4, DataStorage_Hardware, BufferUsage_Static);

			float vertices[2 * 4] = {
			   -0.5f, -0.5f,
			    0.5f, -0.5f,
			   -0.5f, 0.5f,
			    0.5f, 0.5f,
			};

			s_quadVertexBuffer.FillRaw(vertices, 0, sizeof(vertices));

			// Déclaration lors du rendu des billboards par sommet
			s_billboardVertexDeclaration.EnableComponent(VertexComponent_Color, ComponentType_Color, NazaraOffsetOf(BillboardPoint, color));
			s_billboardVertexDeclaration.EnableComponent(VertexComponent_Position, ComponentType_Float3, NazaraOffsetOf(BillboardPoint, position));
			s_billboardVertexDeclaration.EnableComponent(VertexComponent_TexCoord, ComponentType_Float2, NazaraOffsetOf(BillboardPoint, uv));
			s_billboardVertexDeclaration.EnableComponent(VertexComponent_Userdata0, ComponentType_Float4, NazaraOffsetOf(BillboardPoint, size)); // Englobe sincos

			// Declaration utilisée lors du rendu des billboards par instancing
			// L'avantage ici est la copie directe (std::memcpy) des données de la RenderQueue vers le buffer GPU
			s_billboardInstanceDeclaration.EnableComponent(VertexComponent_InstanceData0, ComponentType_Float3, NazaraOffsetOf(ForwardRenderQueue::BillboardData, center));
			s_billboardInstanceDeclaration.EnableComponent(VertexComponent_InstanceData1, ComponentType_Float4, NazaraOffsetOf(ForwardRenderQueue::BillboardData, size)); // Englobe sincos
			s_billboardInstanceDeclaration.EnableComponent(VertexComponent_InstanceData2, ComponentType_Color, NazaraOffsetOf(ForwardRenderQueue::BillboardData, color));
		}
		catch (const std::exception& e)
		{
			NazaraError("Failed to initialise: " + String(e.what()));
			return false;
		}

		return true;
	}

	/*!
	* \brief Uninitializes the depth render technique
	*/

	void DepthRenderTechnique::Uninitialize()
	{
		s_quadIndexBuffer.Reset();
		s_quadVertexBuffer.Reset();
	}
	
	/*!
	* \brief Draws basic sprites
	*
	* \param sceneData Data of the scene
	* \param layer Layer of the rendering
	*/

	void DepthRenderTechnique::DrawBasicSprites(const SceneData& sceneData, ForwardRenderQueue::Layer& layer) const
	{
		NazaraAssert(sceneData.viewer, "Invalid viewer");

		const Shader* lastShader = nullptr;
		const ShaderUniforms* shaderUniforms = nullptr;

		Renderer::SetIndexBuffer(&s_quadIndexBuffer);
		Renderer::SetMatrix(MatrixType_World, Matrix4f::Identity());
		Renderer::SetVertexBuffer(&m_spriteBuffer);

		for (auto& pipelinePair : layer.basicSprites)
		{
			const MaterialPipeline* pipeline = pipelinePair.first;
			auto& pipelineEntry = pipelinePair.second;

			if (pipelineEntry.enabled)
			{
				const MaterialPipeline::Instance& pipelineInstance = pipeline->Apply(ShaderFlags_TextureOverlay | ShaderFlags_VertexColor);

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
						UInt8 overlayUnit;
						material->Apply(pipelineInstance, 0, &overlayUnit);
						overlayUnit++;

						shader->SendInteger(shaderUniforms->textureOverlay, overlayUnit);

						Renderer::SetTextureSampler(overlayUnit, material->GetDiffuseSampler());

						auto& overlayMap = matEntry.overlayMap;
						for (auto& overlayIt : overlayMap)
						{
							const Texture* overlay = overlayIt.first;
							auto& spriteChainVector = overlayIt.second.spriteChains;

							unsigned int spriteChainCount = spriteChainVector.size();
							if (spriteChainCount > 0)
							{
								Renderer::SetTexture(overlayUnit, (overlay) ? overlay : &m_whiteTexture);

								unsigned int spriteChain = 0; // Which chain of sprites are we treating
								unsigned int spriteChainOffset = 0; // Where was the last offset where we stopped in the last chain

								do
								{
									// We open the buffer in writing mode
									BufferMapper<VertexBuffer> vertexMapper(m_spriteBuffer, BufferAccess_DiscardAndWrite);
									VertexStruct_XYZ_Color_UV* vertices = static_cast<VertexStruct_XYZ_Color_UV*>(vertexMapper.GetPointer());

									unsigned int spriteCount = 0;
									unsigned int maxSpriteCount = std::min(s_maxQuads, m_spriteBuffer.GetVertexCount() / 4);

									do
									{
										ForwardRenderQueue::SpriteChain_XYZ_Color_UV& currentChain = spriteChainVector[spriteChain];
										unsigned int count = std::min(maxSpriteCount - spriteCount, currentChain.spriteCount - spriteChainOffset);

										std::memcpy(vertices, currentChain.vertices + spriteChainOffset * 4, 4 * count * sizeof(VertexStruct_XYZ_Color_UV));
										vertices += count * 4;

										spriteCount += count;
										spriteChainOffset += count;

										// Have we treated the entire chain ?
										if (spriteChainOffset == currentChain.spriteCount)
										{
											spriteChain++;
											spriteChainOffset = 0;
										}
									} while (spriteCount < maxSpriteCount && spriteChain < spriteChainCount);

									vertexMapper.Unmap();

									Renderer::DrawIndexedPrimitives(PrimitiveMode_TriangleList, 0, spriteCount * 6);
								} while (spriteChain < spriteChainCount);

								spriteChainVector.clear();
							}
						}

						// We set it back to zero
						matEntry.enabled = false;
					}
				}
				pipelineEntry.enabled = false;
			}
		}
	}

	/*!
	* \brief Draws billboards
	*
	* \param sceneData Data of the scene
	* \param layer Layer of the rendering
	*/
	
	void DepthRenderTechnique::DrawBillboards(const SceneData& sceneData, ForwardRenderQueue::Layer& layer) const
	{
		NazaraAssert(sceneData.viewer, "Invalid viewer");

		const Shader* lastShader = nullptr;
		const ShaderUniforms* shaderUniforms = nullptr;

		if (Renderer::HasCapability(RendererCap_Instancing))
		{
			VertexBuffer* instanceBuffer = Renderer::GetInstanceBuffer();
			instanceBuffer->SetVertexDeclaration(&s_billboardInstanceDeclaration);

			Renderer::SetVertexBuffer(&s_quadVertexBuffer);

			for (auto& pipelinePair : layer.billboards)
			{
				const MaterialPipeline* pipeline = pipelinePair.first;
				auto& pipelineEntry = pipelinePair.second;

				if (pipelineEntry.enabled)
				{
					const MaterialPipeline::Instance& pipelineInstance = pipeline->Apply(ShaderFlags_Billboard | ShaderFlags_Instancing | ShaderFlags_VertexColor);

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

					for (auto& matIt : pipelinePair.second.materialMap)
					{
						const Material* material = matIt.first;
						auto& entry = matIt.second;
						auto& billboardVector = entry.billboards;

						unsigned int billboardCount = billboardVector.size();
						if (billboardCount > 0)
						{
							// We begin to apply the material (and get the shader activated doing so)
							material->Apply(pipelineInstance);

							const ForwardRenderQueue::BillboardData* data = &billboardVector[0];
							unsigned int maxBillboardPerDraw = instanceBuffer->GetVertexCount();
							do
							{
								unsigned int renderedBillboardCount = std::min(billboardCount, maxBillboardPerDraw);
								billboardCount -= renderedBillboardCount;

								instanceBuffer->Fill(data, 0, renderedBillboardCount, true);
								data += renderedBillboardCount;

								Renderer::DrawPrimitivesInstanced(renderedBillboardCount, PrimitiveMode_TriangleStrip, 0, 4);
							}
							while (billboardCount > 0);

							billboardVector.clear();
						}
					}
				}
			}
		}
		else
		{
			Renderer::SetIndexBuffer(&s_quadIndexBuffer);
			Renderer::SetVertexBuffer(&m_billboardPointBuffer);

			for (auto& pipelinePair : layer.billboards)
			{
				const MaterialPipeline* pipeline = pipelinePair.first;
				auto& pipelineEntry = pipelinePair.second;

				if (pipelineEntry.enabled)
				{
					const MaterialPipeline::Instance& pipelineInstance = pipeline->Apply(ShaderFlags_Billboard | ShaderFlags_VertexColor);

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

					for (auto& matIt : pipelinePair.second.materialMap)
					{
						const Material* material = matIt.first;
						auto& entry = matIt.second;
						auto& billboardVector = entry.billboards;

						const ForwardRenderQueue::BillboardData* data = &billboardVector[0];
						unsigned int maxBillboardPerDraw = std::min(s_maxQuads, m_billboardPointBuffer.GetVertexCount() / 4);

						unsigned int billboardCount = billboardVector.size();
						do
						{
							unsigned int renderedBillboardCount = std::min(billboardCount, maxBillboardPerDraw);
							billboardCount -= renderedBillboardCount;

							BufferMapper<VertexBuffer> vertexMapper(m_billboardPointBuffer, BufferAccess_DiscardAndWrite, 0, renderedBillboardCount * 4);
							BillboardPoint* vertices = static_cast<BillboardPoint*>(vertexMapper.GetPointer());

							for (unsigned int i = 0; i < renderedBillboardCount; ++i)
							{
								const ForwardRenderQueue::BillboardData& billboard = *data++;

								vertices->color = billboard.color;
								vertices->position = billboard.center;
								vertices->sinCos = billboard.sinCos;
								vertices->size = billboard.size;
								vertices->uv.Set(0.f, 1.f);
								vertices++;

								vertices->color = billboard.color;
								vertices->position = billboard.center;
								vertices->sinCos = billboard.sinCos;
								vertices->size = billboard.size;
								vertices->uv.Set(1.f, 1.f);
								vertices++;

								vertices->color = billboard.color;
								vertices->position = billboard.center;
								vertices->sinCos = billboard.sinCos;
								vertices->size = billboard.size;
								vertices->uv.Set(0.f, 0.f);
								vertices++;

								vertices->color = billboard.color;
								vertices->position = billboard.center;
								vertices->sinCos = billboard.sinCos;
								vertices->size = billboard.size;
								vertices->uv.Set(1.f, 0.f);
								vertices++;
							}

							vertexMapper.Unmap();

							Renderer::DrawIndexedPrimitives(PrimitiveMode_TriangleList, 0, renderedBillboardCount * 6);
						}
						while (billboardCount > 0);

						billboardVector.clear();
					}
				}
			}
		}
	}

	/*!
	* \brief Draws opaques models
	*
	* \param sceneData Data of the scene
	* \param layer Layer of the rendering
	*/
	
	void DepthRenderTechnique::DrawOpaqueModels(const SceneData& sceneData, ForwardRenderQueue::Layer& layer) const
	{
		NazaraAssert(sceneData.viewer, "Invalid viewer");

		const Shader* lastShader = nullptr;
		const ShaderUniforms* shaderUniforms = nullptr;

		for (auto& pipelinePair : layer.opaqueModels)
		{
			const MaterialPipeline* pipeline = pipelinePair.first;
			auto& pipelineEntry = pipelinePair.second;

			if (pipelineEntry.maxInstanceCount > 0)
			{
				bool instancing = (pipelineEntry.maxInstanceCount > NAZARA_GRAPHICS_INSTANCING_MIN_INSTANCES_COUNT);
				const MaterialPipeline::Instance& pipelineInstance = pipeline->Apply((instancing) ? ShaderFlags_Instancing : 0);

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
						UInt8 freeTextureUnit;
						material->Apply(pipelineInstance, 0, &freeTextureUnit);

						ForwardRenderQueue::MeshInstanceContainer& meshInstances = matEntry.meshMap;

						// Meshes
						for (auto& meshIt : meshInstances)
						{
							const MeshData& meshData = meshIt.first;
							auto& meshEntry = meshIt.second;

							const Spheref& squaredBoundingSphere = meshEntry.squaredBoundingSphere;
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
									// We compute the number of instances that we will be able to draw this time (depending on the instancing buffer size)
									VertexBuffer* instanceBuffer = Renderer::GetInstanceBuffer();
									instanceBuffer->SetVertexDeclaration(VertexDeclaration::Get(VertexLayout_Matrix4));

									const Matrix4f* instanceMatrices = &instances[0];
									unsigned int instanceCount = instances.size();
									unsigned int maxInstanceCount = instanceBuffer->GetVertexCount(); // Maximum number of instance in one batch

									while (instanceCount > 0)
									{
										// We compute the number of instances that we will be able to draw this time (depending on the instancing buffer size)
										unsigned int renderedInstanceCount = std::min(instanceCount, maxInstanceCount);
										instanceCount -= renderedInstanceCount;

										// We fill the instancing buffer with our world matrices
										instanceBuffer->Fill(instanceMatrices, 0, renderedInstanceCount, true);
										instanceMatrices += renderedInstanceCount;

										// And we draw
										instancedDrawFunc(renderedInstanceCount, meshData.primitiveMode, 0, indexCount);
									}
								}
								else
								{
									// Without instancing, we must do a draw call for each instance
									// This may be faster than instancing under a certain number
									// Due to the time to modify the instancing buffer
									for (const Matrix4f& matrix : instances)
									{
										Renderer::SetMatrix(MatrixType_World, matrix);
										drawFunc(meshData.primitiveMode, 0, indexCount);
									}
								}
								instances.clear();
							}
						}

						matEntry.enabled = false;
					}
				}

				pipelineEntry.maxInstanceCount = 0;
			}
		}
	}

	/*!
	* \brief Gets the shader uniforms
	* \return Uniforms of the shader
	*
	* \param shader Shader to get uniforms from
	*/

	const DepthRenderTechnique::ShaderUniforms* DepthRenderTechnique::GetShaderUniforms(const Shader* shader) const
	{
		auto it = m_shaderUniforms.find(shader);
		if (it == m_shaderUniforms.end())
		{
			ShaderUniforms uniforms;
			uniforms.shaderReleaseSlot.Connect(shader->OnShaderRelease, this, &DepthRenderTechnique::OnShaderInvalidated);
			uniforms.shaderUniformInvalidatedSlot.Connect(shader->OnShaderUniformInvalidated, this, &DepthRenderTechnique::OnShaderInvalidated);

			uniforms.eyePosition = shader->GetUniformLocation("EyePosition");
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

	void DepthRenderTechnique::OnShaderInvalidated(const Shader* shader) const
	{
		m_shaderUniforms.erase(shader);
	}

	IndexBuffer DepthRenderTechnique::s_quadIndexBuffer;
	VertexBuffer DepthRenderTechnique::s_quadVertexBuffer;
	VertexDeclaration DepthRenderTechnique::s_billboardInstanceDeclaration;
	VertexDeclaration DepthRenderTechnique::s_billboardVertexDeclaration;
}