// Copyright (C) 2013 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/DeferredGeometryPass.hpp>
#include <Nazara/Core/ErrorFlags.hpp>
#include <Nazara/Graphics/AbstractViewer.hpp>
#include <Nazara/Graphics/DeferredRenderTechnique.hpp>
#include <Nazara/Graphics/Scene.hpp>
#include <Nazara/Renderer/Material.hpp>
#include <Nazara/Renderer/Renderer.hpp>
#include <Nazara/Renderer/RenderTexture.hpp>
#include <Nazara/Utility/BufferMapper.hpp>
#include <Nazara/Utility/StaticMesh.hpp>
#include <Nazara/Utility/VertexStruct.hpp>
#include <memory>
#include <Nazara/Graphics/Debug.hpp>

namespace
{
	NzShaderProgram* BuildClearProgram()
	{
		const nzUInt8 fragmentSource[] = {
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

NzDeferredGeometryPass::NzDeferredGeometryPass()
{
	m_clearProgram = BuildClearProgram();
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
	NzRenderer::SetShaderProgram(m_clearProgram);
	NzRenderer::DrawFullscreenQuad();


	NzRenderer::SetMatrix(nzMatrixType_Projection, viewer->GetProjectionMatrix());
	NzRenderer::SetMatrix(nzMatrixType_View, viewer->GetViewMatrix());

	const NzShaderProgram* lastProgram = nullptr;

	for (auto& matIt : m_renderQueue->opaqueModels)
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
				bool useInstancing = instancingEnabled && renderQueueInstancing;

				// On commence par récupérer le programme du matériau
				nzUInt32 flags = nzShaderFlags_Deferred;
				if (useInstancing)
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
						if (useInstancing)
						{
							NzVertexBuffer* instanceBuffer = NzRenderer::GetInstanceBuffer();

							instanceBuffer->SetVertexDeclaration(NzVertexDeclaration::Get(nzVertexLayout_Matrix4));

							unsigned int stride = instanceBuffer->GetStride();

							const NzDeferredRenderQueue::StaticData* data = &staticData[0];
							unsigned int instanceCount = staticData.size();
							unsigned int maxInstanceCount = instanceBuffer->GetVertexCount(); // Le nombre de sommets maximum avec la déclaration donnée plus hautg

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

	return false; // On ne fait que remplir le G-Buffer, les work texture ne sont pas affectées
}

bool NzDeferredGeometryPass::Resize(const NzVector2ui& dimensions)
{
	NzDeferredRenderPass::Resize(dimensions);

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

		unsigned int width = dimensions.x;
		unsigned int height = dimensions.y;

		m_depthStencilBuffer->Create(nzPixelFormat_Depth24Stencil8, width, height);

		m_GBuffer[0]->Create(nzImageType_2D, nzPixelFormat_RGBA8, width, height);
		m_GBuffer[1]->Create(nzImageType_2D, nzPixelFormat_RGBA32F, width, height);
		m_GBuffer[2]->Create(nzImageType_2D, nzPixelFormat_RGBA8, width, height);

		m_GBufferRTT->Create(true);

		// Texture 0 : Diffuse Color + Flags
		m_GBufferRTT->AttachTexture(nzAttachmentPoint_Color, 0, m_GBuffer[0]);

		// Texture 1 : Normal map + Depth
		m_GBufferRTT->AttachTexture(nzAttachmentPoint_Color, 1, m_GBuffer[1]);

		// Texture 2 : Specular value + Shininess
		m_GBufferRTT->AttachTexture(nzAttachmentPoint_Color, 2, m_GBuffer[2]);

		// Texture 3 : Emission map ?

		m_GBufferRTT->AttachBuffer(nzAttachmentPoint_DepthStencil, 0, m_deferredTechnique->GetDepthStencilBuffer());

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
		NazaraError("Failed to create G-Buffer RTT");
		return false;
	}
}
