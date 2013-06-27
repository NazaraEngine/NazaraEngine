// Copyright (C) 2013 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/ForwardRenderTechnique.hpp>
#include <Nazara/Graphics/AbstractBackground.hpp>
#include <Nazara/Graphics/Camera.hpp>
#include <Nazara/Graphics/Drawable.hpp>
#include <Nazara/Graphics/Light.hpp>
#include <Nazara/Renderer/Config.hpp>
#include <Nazara/Renderer/Material.hpp>
#include <Nazara/Renderer/Renderer.hpp>
#include <Nazara/Renderer/ShaderBuilder.hpp>
#include <Nazara/Utility/StaticMesh.hpp>
#include <Nazara/Graphics/Debug.hpp>

namespace
{
	struct LightComparator
	{
		bool operator()(const NzLight* light1, const NzLight* light2)
		{
			return light1->GetPosition().SquaredDistance(pos) < light2->GetPosition().SquaredDistance(pos);
		}

		NzVector3f pos;
	};
}

NzForwardRenderTechnique::NzForwardRenderTechnique() :
m_maxLightsPerObject(3) // Valeur totalement arbitraire
{
}

void NzForwardRenderTechnique::Clear(const NzScene* scene)
{
	NzRenderer::Enable(nzRendererParameter_DepthBuffer, true);
	NzRenderer::Enable(nzRendererParameter_DepthWrite, true);
	NzRenderer::Clear(nzRendererClear_Depth);

	NzAbstractBackground* background = scene->GetBackground();
	if (background)
		background->Draw(scene);
}

void NzForwardRenderTechnique::Draw(const NzScene* scene)
{
	///TODO: Regrouper les activations par méthode
	LightComparator lightComparator;

	const NzCamera* camera = scene->GetActiveCamera();
	const NzShader* lastShader = nullptr;

	// Externes à la boucle pour conserver leur valeurs si le shader ne change pas
	unsigned int lightCount = 0;
	int lightCountLocation = -1;

	// Rendu des modèles opaques
	for (auto matIt : m_renderQueue.visibleModels)
	{
		NzMaterial* material = matIt.first;

		// On commence par récupérer le shader du matériau
		const NzShader* shader;
		if (material->HasCustomShader())
			shader = material->GetCustomShader();
		else
			shader = NzShaderBuilder::Get(material->GetShaderFlags());

		// Les uniformes sont conservées au sein du shader, inutile de les renvoyer tant que le shader reste le même
		if (shader != lastShader)
		{
			// On récupère l'information sur l'éclairage en même temps que la position de l'uniforme "LightCount"
			lightCountLocation = shader->GetUniformLocation(nzShaderUniform_LightCount);

			NzRenderer::SetShader(shader);

			// Couleur ambiante de la scène
			shader->SendColor(shader->GetUniformLocation(nzShaderUniform_SceneAmbient), scene->GetAmbientColor());
			// Position de la caméra
			shader->SendVector(shader->GetUniformLocation(nzShaderUniform_CameraPosition), camera->GetPosition());

			lightCount = 0;

			// On envoie les lumières directionnelles s'il y a (Les mêmes pour tous)
			if (lightCountLocation != -1)
			{
				for (const NzLight* light : m_renderQueue.directionnalLights)
				{
					light->Apply(shader, lightCount++);
					if (lightCount > NAZARA_RENDERER_SHADER_MAX_LIGHTCOUNT)
						break; // Prévenons les bêtises des utilisateurs
				}
			}
			lastShader = shader;
		}

		material->Apply(shader);

		// Meshs squelettiques
		/*NzForwardRenderQueue::SkeletalMeshContainer& container = matIt.second.first;
		if (!container.empty())
		{
			NzRenderer::SetVertexBuffer(m_skinningBuffer); // Vertex buffer commun
			for (auto subMeshIt : container)
			{
				///TODO
			}
		}*/

		// Meshs statiques
		for (auto subMeshIt : matIt.second.second)
		{
			NzStaticMesh* mesh = subMeshIt.first;

			const NzIndexBuffer* indexBuffer = mesh->GetIndexBuffer();
			const NzVertexBuffer* vertexBuffer = mesh->GetVertexBuffer();

			// Gestion du draw call avant la boucle de rendu
			std::function<void(nzPrimitiveMode, unsigned int, unsigned int)> drawFunc;
			unsigned int indexCount;

			if (indexBuffer)
			{
				drawFunc = NzRenderer::DrawIndexedPrimitives;
				indexCount = indexBuffer->GetIndexCount();
			}
			else
			{
				drawFunc = NzRenderer::DrawPrimitives;
				indexCount = vertexBuffer->GetVertexCount();
			}

			NzRenderer::SetIndexBuffer(indexBuffer);
			NzRenderer::SetVertexBuffer(vertexBuffer);

			for (const NzMatrix4f& matrix : subMeshIt.second)
			{
				// Calcul des lumières les plus proches
				///TODO: LightManager ?
				if (lightCountLocation != -1)
				{
					std::vector<const NzLight*>& visibleLights = m_renderQueue.visibleLights;

					lightComparator.pos = matrix.GetTranslation();
					std::sort(visibleLights.begin(), visibleLights.end(), lightComparator);

					unsigned int max = std::min(std::min(NAZARA_RENDERER_SHADER_MAX_LIGHTCOUNT - lightCount, m_maxLightsPerObject), static_cast<unsigned int>(visibleLights.size()));
					for (unsigned int i = 0; i < max; ++i)
						visibleLights[i]->Apply(shader, lightCount++);

					shader->SendInteger(lightCountLocation, lightCount);
				}

				NzRenderer::SetMatrix(nzMatrixType_World, matrix);
				drawFunc(mesh->GetPrimitiveMode(), 0, indexCount);
			}
		}
	}

	for (const std::pair<unsigned int, bool>& pair : m_renderQueue.visibleTransparentsModels)
	{
		// Matériau
		NzMaterial* material = (pair.second) ?
		                       m_renderQueue.transparentStaticModels[pair.first].material :
		                       m_renderQueue.transparentSkeletalModels[pair.first].material;

		// On commence par récupérer le shader du matériau
		const NzShader* shader;
		if (material->HasCustomShader())
			shader = material->GetCustomShader();
		else
			shader = NzShaderBuilder::Get(material->GetShaderFlags());

		// Les uniformes sont conservées au sein du shader, inutile de les renvoyer tant que le shader reste le même
		if (shader != lastShader)
		{
			// On récupère l'information sur l'éclairage en même temps que la position de l'uniforme "LightCount"
			lightCountLocation = shader->GetUniformLocation(nzShaderUniform_LightCount);

			NzRenderer::SetShader(shader);

			// Couleur ambiante de la scène
			shader->SendColor(shader->GetUniformLocation(nzShaderUniform_SceneAmbient), scene->GetAmbientColor());
			// Position de la caméra
			shader->SendVector(shader->GetUniformLocation(nzShaderUniform_CameraPosition), camera->GetPosition());

			lightCount = 0;

			// On envoie les lumières directionnelles s'il y a (Les mêmes pour tous)
			if (lightCountLocation != -1)
			{
				for (const NzLight* light : m_renderQueue.directionnalLights)
				{
					light->Apply(shader, lightCount++);
					if (lightCount > NAZARA_RENDERER_SHADER_MAX_LIGHTCOUNT)
						break; // Prévenons les bêtises des utilisateurs
				}
			}
			lastShader = shader;
		}

		material->Apply(shader);

		// Mesh
		if (pair.second)
		{
			NzForwardRenderQueue::TransparentStaticModel& staticModel = m_renderQueue.transparentStaticModels[pair.first];

			const NzMatrix4f& matrix = staticModel.transformMatrix;
			NzStaticMesh* mesh = staticModel.mesh;

			const NzIndexBuffer* indexBuffer = mesh->GetIndexBuffer();
			const NzVertexBuffer* vertexBuffer = mesh->GetVertexBuffer();

			// Gestion du draw call avant la boucle de rendu
			std::function<void(nzPrimitiveMode, unsigned int, unsigned int)> drawFunc;
			unsigned int indexCount;

			if (indexBuffer)
			{
				drawFunc = NzRenderer::DrawIndexedPrimitives;
				indexCount = indexBuffer->GetIndexCount();
			}
			else
			{
				drawFunc = NzRenderer::DrawPrimitives;
				indexCount = vertexBuffer->GetVertexCount();
			}

			NzRenderer::SetIndexBuffer(indexBuffer);
			NzRenderer::SetVertexBuffer(vertexBuffer);

			// Calcul des lumières les plus proches
			///TODO: LightManager ?
			if (lightCountLocation != -1)
			{
				std::vector<const NzLight*>& visibleLights = m_renderQueue.visibleLights;

				lightComparator.pos = matrix.GetTranslation();
				std::sort(visibleLights.begin(), visibleLights.end(), lightComparator);

				unsigned int max = std::min(std::min(NAZARA_RENDERER_SHADER_MAX_LIGHTCOUNT - lightCount, m_maxLightsPerObject), static_cast<unsigned int>(visibleLights.size()));
				for (unsigned int i = 0; i < max; ++i)
					visibleLights[i]->Apply(shader, lightCount++);

				shader->SendInteger(lightCountLocation, lightCount);
			}

			NzRenderer::SetMatrix(nzMatrixType_World, matrix);
			drawFunc(mesh->GetPrimitiveMode(), 0, indexCount);
		}
		else
		{
			///TODO
		}
	}

	// Les autres drawables (Exemple: Terrain)
	for (const NzDrawable* drawable : m_renderQueue.otherDrawables)
		drawable->Draw();
}

unsigned int NzForwardRenderTechnique::GetMaxLightsPerObject() const
{
	return m_maxLightsPerObject;
}

NzAbstractRenderQueue* NzForwardRenderTechnique::GetRenderQueue()
{
	return &m_renderQueue;
}

void NzForwardRenderTechnique::SetMaxLightsPerObject(unsigned int lightCount)
{
	m_maxLightsPerObject = lightCount;
}
