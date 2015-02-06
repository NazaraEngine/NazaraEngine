// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/DeferredGeometryPass.hpp>
#include <Nazara/Core/ErrorFlags.hpp>
#include <Nazara/Graphics/AbstractViewer.hpp>
#include <Nazara/Graphics/DeferredRenderTechnique.hpp>
#include <Nazara/Graphics/Material.hpp>
#include <Nazara/Graphics/Scene.hpp>
#include <Nazara/Renderer/Renderer.hpp>
#include <Nazara/Renderer/RenderTexture.hpp>
#include <Nazara/Utility/BufferMapper.hpp>
#include <Nazara/Utility/StaticMesh.hpp>
#include <Nazara/Utility/VertexStruct.hpp>
#include <memory>
#include <Nazara/Graphics/Debug.hpp>

NzDeferredGeometryPass::NzDeferredGeometryPass()
{
	m_clearShader = NzShaderLibrary::Get("DeferredGBufferClear");
	m_clearStates.parameters[nzRendererParameter_DepthBuffer] = true;
	m_clearStates.parameters[nzRendererParameter_FaceCulling] = true;
	m_clearStates.parameters[nzRendererParameter_StencilTest] = true;
	m_clearStates.depthFunc = nzRendererComparison_Always;
	m_clearStates.frontFace.stencilCompare = nzRendererComparison_Always;
	m_clearStates.frontFace.stencilPass = nzStencilOperation_Zero;
}

NzDeferredGeometryPass::~NzDeferredGeometryPass() = default;

bool NzDeferredGeometryPass::Process(const NzScene* scene, unsigned int firstWorkTexture, unsigned secondWorkTexture) const
{
	NazaraUnused(firstWorkTexture);
	NazaraUnused(secondWorkTexture);

	NzAbstractViewer* viewer = scene->GetViewer();
	bool instancingEnabled = m_deferredTechnique->IsInstancingEnabled();

	m_GBufferRTT->SetColorTargets({0, 1, 2}); // G-Buffer
	NzRenderer::SetTarget(m_GBufferRTT);
	NzRenderer::SetViewport(NzRecti(0, 0, m_dimensions.x, m_dimensions.y));

	NzRenderer::SetRenderStates(m_clearStates);
	NzRenderer::SetShader(m_clearShader);
	NzRenderer::DrawFullscreenQuad();


	NzRenderer::SetMatrix(nzMatrixType_Projection, viewer->GetProjectionMatrix());
	NzRenderer::SetMatrix(nzMatrixType_View, viewer->GetViewMatrix());

	const NzShader* lastShader = nullptr;
	const ShaderUniforms* shaderUniforms = nullptr;

	for (auto& matIt : m_renderQueue->opaqueModels)
	{
		auto& matEntry = matIt.second;

		if (matEntry.enabled)
		{
			NzDeferredRenderQueue::MeshInstanceContainer& meshInstances = matEntry.meshMap;

			if (!meshInstances.empty())
			{
				const NzMaterial* material = matIt.first;

				bool useInstancing = instancingEnabled && matEntry.instancingEnabled;

				// On commence par récupérer le programme du matériau
				nzUInt32 flags = nzShaderFlags_Deferred;
				if (useInstancing)
					flags |= nzShaderFlags_Instancing;

				const NzShader* shader = material->Apply(flags);

				// Les uniformes sont conservées au sein d'un programme, inutile de les renvoyer tant qu'il ne change pas
				if (shader != lastShader)
				{
					// Index des uniformes dans le shader
					shaderUniforms = GetShaderUniforms(shader);

					// Couleur ambiante de la scène
					shader->SendColor(shaderUniforms->sceneAmbient, scene->GetAmbientColor());
					// Position de la caméra
					shader->SendVector(shaderUniforms->eyePosition, viewer->GetEyePosition());

					lastShader = shader;
				}

				// Meshes
				for (auto& meshIt : meshInstances)
				{
					const NzMeshData& meshData = meshIt.first;
					auto& meshEntry = meshIt.second;

					std::vector<NzMatrix4f>& instances = meshEntry.instances;
					if (!instances.empty())
					{
						const NzIndexBuffer* indexBuffer = meshData.indexBuffer;
						const NzVertexBuffer* vertexBuffer = meshData.vertexBuffer;

						// Gestion du draw call avant la boucle de rendu
						NzRenderer::DrawCall drawFunc;
						NzRenderer::DrawCallInstanced instancedDrawFunc;
						unsigned int indexCount;

						if (indexBuffer)
						{
							drawFunc = NzRenderer::DrawIndexedPrimitives;
							instancedDrawFunc = NzRenderer::DrawIndexedPrimitivesInstanced;
							indexCount = indexBuffer->GetIndexCount();
						}
						else
						{
							drawFunc = NzRenderer::DrawPrimitives;
							instancedDrawFunc = NzRenderer::DrawPrimitivesInstanced;
							indexCount = vertexBuffer->GetVertexCount();
						}

						NzRenderer::SetIndexBuffer(indexBuffer);
						NzRenderer::SetVertexBuffer(vertexBuffer);

						if (useInstancing)
						{
							// On récupère le buffer d'instancing du Renderer et on le configure pour fonctionner avec des matrices
							NzVertexBuffer* instanceBuffer = NzRenderer::GetInstanceBuffer();
							instanceBuffer->SetVertexDeclaration(NzVertexDeclaration::Get(nzVertexLayout_Matrix4));

							const NzMatrix4f* instanceMatrices = &instances[0];
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
							for (const NzMatrix4f& matrix : instances)
							{
								NzRenderer::SetMatrix(nzMatrixType_World, matrix);
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

	return false; // On ne fait que remplir le G-Buffer, les work texture ne sont pas affectées
}

bool NzDeferredGeometryPass::Resize(const NzVector2ui& dimensions)
{
	NzDeferredRenderPass::Resize(dimensions);

	/*
	G-Buffer:
	Texture0: Diffuse Color + Flags
	Texture1: Encoded normal
	Texture2: Specular value + Shininess
	Texture3: N/A
	*/

	try
	{
		NzErrorFlags errFlags(nzErrorFlag_ThrowException);

		unsigned int width = dimensions.x;
		unsigned int height = dimensions.y;

		m_depthStencilBuffer->Create(nzPixelFormat_Depth24Stencil8, width, height);

		m_GBuffer[0]->Create(nzImageType_2D, nzPixelFormat_RGBA8, width, height); // Texture 0 : Diffuse Color + Specular
		m_GBuffer[1]->Create(nzImageType_2D, nzPixelFormat_RG16F, width, height); // Texture 1 : Encoded normal
		m_GBuffer[2]->Create(nzImageType_2D, nzPixelFormat_RGBA8, width, height); // Texture 2 : Depth (24bits) + Shininess

		m_GBufferRTT->Create(true);

		m_GBufferRTT->AttachTexture(nzAttachmentPoint_Color, 0, m_GBuffer[0]);
		m_GBufferRTT->AttachTexture(nzAttachmentPoint_Color, 1, m_GBuffer[1]);
		m_GBufferRTT->AttachTexture(nzAttachmentPoint_Color, 2, m_GBuffer[2]);

		// Texture 3 : Emission map ?

		m_GBufferRTT->AttachBuffer(nzAttachmentPoint_DepthStencil, 0, m_depthStencilBuffer);

		m_GBufferRTT->Unlock();

		m_workRTT->Create(true);

		for (unsigned int i = 0; i < 2; ++i)
		{
			m_workTextures[i]->Create(nzImageType_2D, nzPixelFormat_RGBA8, width, height);
			m_workRTT->AttachTexture(nzAttachmentPoint_Color, i, m_workTextures[i]);
		}

		m_workRTT->AttachBuffer(nzAttachmentPoint_DepthStencil, 0, m_depthStencilBuffer);

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
		NazaraError("Failed to create G-Buffer RTT: " + NzString(e.what()));
		return false;
	}
}

const NzDeferredGeometryPass::ShaderUniforms* NzDeferredGeometryPass::GetShaderUniforms(const NzShader* shader) const
{
	auto it = m_shaderUniforms.find(shader);
	if (it == m_shaderUniforms.end())
	{
		ShaderUniforms uniforms;
		uniforms.eyePosition = shader->GetUniformLocation("EyePosition");
		uniforms.sceneAmbient = shader->GetUniformLocation("SceneAmbient");
		uniforms.textureOverlay = shader->GetUniformLocation("TextureOverlay");

		it = m_shaderUniforms.emplace(shader, uniforms).first;
	}

	return &it->second;
}
