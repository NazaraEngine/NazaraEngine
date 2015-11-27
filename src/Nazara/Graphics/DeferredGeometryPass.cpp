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
	DeferredGeometryPass::DeferredGeometryPass()
	{
		m_clearShader = ShaderLibrary::Get("DeferredGBufferClear");
		m_clearStates.parameters[RendererParameter_DepthBuffer] = true;
		m_clearStates.parameters[RendererParameter_FaceCulling] = true;
		m_clearStates.parameters[RendererParameter_StencilTest] = true;
		m_clearStates.depthFunc = RendererComparison_Always;
		m_clearStates.frontFace.stencilCompare = RendererComparison_Always;
		m_clearStates.frontFace.stencilPass = StencilOperation_Zero;
	}

	DeferredGeometryPass::~DeferredGeometryPass() = default;

	bool DeferredGeometryPass::Process(const SceneData& sceneData, unsigned int firstWorkTexture, unsigned secondWorkTexture) const
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

		for (auto& pair : m_renderQueue->layers)
		{
			DeferredRenderQueue::Layer& layer = pair.second;

			for (auto& matIt : layer.opaqueModels)
			{
				auto& matEntry = matIt.second;

				if (matEntry.enabled)
				{
					DeferredRenderQueue::MeshInstanceContainer& meshInstances = matEntry.meshMap;

					if (!meshInstances.empty())
					{
						const Material* material = matIt.first;

						bool useInstancing = instancingEnabled && matEntry.instancingEnabled;

						// On commence par récupérer le programme du matériau
						UInt32 flags = ShaderFlags_Deferred;
						if (useInstancing)
							flags |= ShaderFlags_Instancing;

						const Shader* shader = material->Apply(flags);

						// Les uniformes sont conservées au sein d'un programme, inutile de les renvoyer tant qu'il ne change pas
						if (shader != lastShader)
						{
							// Index des uniformes dans le shader
							shaderUniforms = GetShaderUniforms(shader);

							// Couleur ambiante de la scène
							shader->SendColor(shaderUniforms->sceneAmbient, sceneData.ambientColor);
							// Position de la caméra
							shader->SendVector(shaderUniforms->eyePosition, sceneData.viewer->GetEyePosition());

							lastShader = shader;
						}

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

								// Gestion du draw call avant la boucle de rendu
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

								if (useInstancing)
								{
									// On récupère le buffer d'instancing du Renderer et on le configure pour fonctionner avec des matrices
									VertexBuffer* instanceBuffer = Renderer::GetInstanceBuffer();
									instanceBuffer->SetVertexDeclaration(VertexDeclaration::Get(VertexLayout_Matrix4));

									const Matrix4f* instanceMatrices = &instances[0];
									unsigned int instanceCount = instances.size();
									unsigned int maxInstanceCount = instanceBuffer->GetVertexCount(); // Le nombre de matrices que peut contenir le buffer

									while (instanceCount > 0)
									{
										// On calcule le nombre d'instances que l'on pourra afficher cette fois-ci (Selon la taille du buffer d'instancing)
										unsigned int renderedInstanceCount = std::min(instanceCount, maxInstanceCount);
										instanceCount -= renderedInstanceCount;

										// On remplit l'instancing buffer avec nos matrices world
										instanceBuffer->Fill(instanceMatrices, 0, renderedInstanceCount, true);
										instanceMatrices += renderedInstanceCount;

										// Et on affiche
										instancedDrawFunc(renderedInstanceCount, meshData.primitiveMode, 0, indexCount);
									}
								}
								else
								{
									// Sans instancing, on doit effectuer un draw call pour chaque instance
									// Cela reste néanmoins plus rapide que l'instancing en dessous d'un certain nombre d'instances
									// À cause du temps de modification du buffer d'instancing
									for (const Matrix4f& matrix : instances)
									{
										Renderer::SetMatrix(MatrixType_World, matrix);
										drawFunc(meshData.primitiveMode, 0, indexCount);
									}
								}

								instances.clear();
							}
						}
					}

					// Et on remet à zéro les données
					matEntry.enabled = false;
					matEntry.instancingEnabled = false;
				}
			}
		}

		return false; // On ne fait que remplir le G-Buffer, les work texture ne sont pas affectées
	}

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

			m_depthStencilBuffer->Create(PixelFormatType_Depth24Stencil8, width, height);

			m_GBuffer[0]->Create(ImageType_2D, PixelFormatType_RGBA8, width, height); // Texture 0 : Diffuse Color + Specular
			m_GBuffer[1]->Create(ImageType_2D, PixelFormatType_RG16F, width, height); // Texture 1 : Encoded normal
			m_GBuffer[2]->Create(ImageType_2D, PixelFormatType_RGBA8, width, height); // Texture 2 : Depth (24bits) + Shininess

			m_GBufferRTT->Create(true);

			m_GBufferRTT->AttachTexture(AttachmentPoint_Color, 0, m_GBuffer[0]);
			m_GBufferRTT->AttachTexture(AttachmentPoint_Color, 1, m_GBuffer[1]);
			m_GBufferRTT->AttachTexture(AttachmentPoint_Color, 2, m_GBuffer[2]);

			// Texture 3 : Emission map ?

			m_GBufferRTT->AttachBuffer(AttachmentPoint_DepthStencil, 0, m_depthStencilBuffer);

			m_GBufferRTT->Unlock();

			m_workRTT->Create(true);

			for (unsigned int i = 0; i < 2; ++i)
			{
				m_workTextures[i]->Create(ImageType_2D, PixelFormatType_RGBA8, width, height);
				m_workRTT->AttachTexture(AttachmentPoint_Color, i, m_workTextures[i]);
			}

			m_workRTT->AttachBuffer(AttachmentPoint_DepthStencil, 0, m_depthStencilBuffer);

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

	void DeferredGeometryPass::OnShaderInvalidated(const Shader* shader) const
	{
		m_shaderUniforms.erase(shader);
	}
}
