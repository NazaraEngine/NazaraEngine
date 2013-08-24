// Copyright (C) 2013 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/ForwardRenderQueue.hpp>
#include <Nazara/Graphics/AbstractViewer.hpp>
#include <Nazara/Graphics/Light.hpp>
#include <Nazara/Graphics/Model.hpp>
#include <Nazara/Graphics/Sprite.hpp>
#include <Nazara/Renderer/Material.hpp>
#include <Nazara/Utility/SkeletalMesh.hpp>
#include <Nazara/Utility/StaticMesh.hpp>
#include <Nazara/Graphics/Debug.hpp>

namespace
{
	enum ResourceType
	{
		ResourceType_Material,
		ResourceType_SkeletalMesh,
		ResourceType_StaticMesh
	};
}

NzForwardRenderQueue::~NzForwardRenderQueue()
{
	Clear(true);
}

void NzForwardRenderQueue::AddDrawable(const NzDrawable* drawable)
{
	#if NAZARA_GRAPHICS_SAFE
	if (!drawable)
	{
		NazaraError("Invalid drawable");
		return;
	}
	#endif

	otherDrawables.push_back(drawable);
}

void NzForwardRenderQueue::AddLight(const NzLight* light)
{
	#if NAZARA_GRAPHICS_SAFE
	if (!light)
	{
		NazaraError("Invalid light");
		return;
	}
	#endif

	switch (light->GetLightType())
	{
		case nzLightType_Directional:
			directionnalLights.push_back(light);
			break;

		case nzLightType_Point:
		case nzLightType_Spot:
			lights.push_back(light);
			break;

		#ifdef NAZARA_DEBUG
		default:
			NazaraError("Light type not handled (0x" + NzString::Number(light->GetLightType(), 16) + ')');
		#endif
	}
}

void NzForwardRenderQueue::AddModel(const NzModel* model)
{
	#if NAZARA_GRAPHICS_SAFE
	if (!model)
	{
		NazaraError("Invalid model");
		return;
	}

	if (!model->IsDrawable())
	{
		NazaraError("Model is not drawable");
		return;
	}
	#endif

	const NzMatrix4f& transformMatrix = model->GetTransformMatrix();

	NzMesh* mesh = model->GetMesh();
	unsigned int submeshCount = mesh->GetSubMeshCount();

	for (unsigned int i = 0; i < submeshCount; ++i)
	{
		NzSubMesh* subMesh = mesh->GetSubMesh(i);
		NzMaterial* material = model->GetMaterial(subMesh->GetMaterialIndex());

		AddSubMesh(material, subMesh, transformMatrix);
	}
}

void NzForwardRenderQueue::AddSprite(const NzSprite* sprite)
{
	#if NAZARA_GRAPHICS_SAFE
	if (!sprite)
	{
		NazaraError("Invalid sprite");
		return;
	}

	if (!sprite->IsDrawable())
	{
		NazaraError("Sprite is not drawable");
		return;
	}
	#endif

	sprites[sprite->GetMaterial()].push_back(sprite);
}

void NzForwardRenderQueue::AddSubMesh(const NzMaterial* material, const NzSubMesh* subMesh, const NzMatrix4f& transformMatrix)
{
	switch (subMesh->GetAnimationType())
	{
		case nzAnimationType_Skeletal:
		{
			///TODO
			/*
			** Il y a ici deux choses importantes à gérer:
			** -Pour commencer, la mise en cache de std::vector suffisamment grands pour contenir le résultat du skinning
			**  l'objectif ici est d'éviter une allocation à chaque frame, donc de réutiliser un tableau existant
			**  Note: Il faudrait évaluer aussi la possibilité de conserver le buffer d'une frame à l'autre.
			**        Ceci permettant de ne pas skinner inutilement ce qui ne bouge pas, ou de skinner partiellement un mesh.
			**        Il faut cependant voir où stocker ce set de buffers, qui doit être communs à toutes les RQ d'une même scène.
			**
			** -Ensuite, la possibilité de regrouper les modèles skinnés identiques, une centaine de soldats marchant au pas
			**  ne devrait requérir qu'un skinning.
			*/
			NazaraError("Skeletal mesh not supported yet, sorry");
			break;
		}

		case nzAnimationType_Static:
		{
			const NzStaticMesh* staticMesh = static_cast<const NzStaticMesh*>(subMesh);

			if (material->IsEnabled(nzRendererParameter_Blend))
			{
				unsigned int index = transparentStaticModels.size();
				transparentStaticModels.resize(index+1);

				TransparentStaticModel& data = transparentStaticModels.back();
				data.boundingSphere = NzSpheref(transformMatrix.GetTranslation(), staticMesh->GetAABB().GetSquaredRadius());
				data.material = material;
				data.mesh = staticMesh;
				data.transformMatrix = transformMatrix;

				transparentsModels.push_back(std::make_pair(index, true));
			}
			else
			{
				auto pair = opaqueModels.insert(std::make_pair(material, BatchedModelContainer::mapped_type()));
				if (pair.second)
					material->AddResourceListener(this, ResourceType_Material);

				bool& used = std::get<0>(pair.first->second);
				bool& enableInstancing = std::get<1>(pair.first->second);

				used = true;

				auto& meshMap = std::get<3>(pair.first->second);

				auto pair2 = meshMap.insert(std::make_pair(staticMesh, BatchedStaticMeshContainer::mapped_type()));
				if (pair2.second)
				{
					staticMesh->AddResourceListener(this, ResourceType_StaticMesh);

					NzSpheref& squaredBoundingSphere = pair2.first->second.first;
					squaredBoundingSphere.Set(staticMesh->GetAABB().GetSquaredBoundingSphere());
					///TODO: Écouter le StaticMesh pour repérer tout changement de géométrie
				}

				std::vector<StaticData>& staticDataContainer = pair2.first->second.second;

				unsigned int instanceCount = staticDataContainer.size() + 1;

				// Avons-nous suffisamment d'instances pour que le coût d'utilisation de l'instancing soit payé ?
				if (instanceCount >= NAZARA_GRAPHICS_INSTANCING_MIN_INSTANCES_COUNT)
					enableInstancing = true; // Apparemment oui, activons l'instancing avec ce matériau

				staticDataContainer.resize(instanceCount);
				StaticData& data = staticDataContainer.back();
				data.transformMatrix = transformMatrix;
			}

			break;
		}
	}
}

void NzForwardRenderQueue::Clear(bool fully)
{
	directionnalLights.clear();
	lights.clear();
	otherDrawables.clear();
	transparentsModels.clear();
	transparentSkeletalModels.clear();
	transparentStaticModels.clear();

	if (fully)
	{
		opaqueModels.clear();
		sprites.clear();
	}
}

void NzForwardRenderQueue::Sort(const NzAbstractViewer* viewer)
{
	struct TransparentModelComparator
	{
		bool operator()(const std::pair<unsigned int, bool>& index1, const std::pair<unsigned int, bool>& index2)
		{
			const NzSpheref& sphere1 = (index1.second) ?
			                     queue->transparentStaticModels[index1.first].boundingSphere :
			                     queue->transparentSkeletalModels[index1.first].boundingSphere;

			const NzSpheref& sphere2 = (index2.second) ?
			                     queue->transparentStaticModels[index2.first].boundingSphere :
			                     queue->transparentSkeletalModels[index2.first].boundingSphere;

			NzVector3f position1 = sphere1.GetNegativeVertex(viewerNormal);
			NzVector3f position2 = sphere2.GetNegativeVertex(viewerNormal);

			return nearPlane.Distance(position1) > nearPlane.Distance(position2);
		}

		NzForwardRenderQueue* queue;
		NzPlanef nearPlane;
		NzVector3f viewerNormal;
	};

	TransparentModelComparator comparator {this, viewer->GetFrustum().GetPlane(nzFrustumPlane_Near), viewer->GetForward()};
	std::sort(transparentsModels.begin(), transparentsModels.end(), comparator);
}

bool NzForwardRenderQueue::OnResourceDestroy(const NzResource* resource, int index)
{
	switch (index)
	{
		case ResourceType_Material:
			opaqueModels.erase(static_cast<const NzMaterial*>(resource));
			break;

		case ResourceType_SkeletalMesh:
		{
			for (auto& pair : opaqueModels)
				std::get<2>(pair.second).erase(static_cast<const NzSkeletalMesh*>(resource));

			break;
		}

		case ResourceType_StaticMesh:
		{
			for (auto& pair : opaqueModels)
				std::get<3>(pair.second).erase(static_cast<const NzStaticMesh*>(resource));

			break;
		}
	}

	return false; // Nous ne voulons plus recevoir d'évènement de cette ressource
}

bool NzForwardRenderQueue::BatchedModelMaterialComparator::operator()(const NzMaterial* mat1, const NzMaterial* mat2)
{
	for (unsigned int i = 0; i <= nzShaderFlags_Max; ++i)
	{
		const NzShaderProgram* program1 = mat1->GetShaderProgram(nzShaderTarget_Model, i);
		const NzShaderProgram* program2 = mat2->GetShaderProgram(nzShaderTarget_Model, i);

		if (program1 != program2)
			return program1 < program2;
	}

	const NzTexture* diffuseMap1 = mat1->GetDiffuseMap();
	const NzTexture* diffuseMap2 = mat2->GetDiffuseMap();
	if (diffuseMap1 != diffuseMap2)
		return diffuseMap1 < diffuseMap2;

	return mat1 < mat2;
}

bool NzForwardRenderQueue::BatchedSpriteMaterialComparator::operator()(const NzMaterial* mat1, const NzMaterial* mat2)
{
	for (unsigned int i = 0; i <= nzShaderFlags_Max; ++i)
	{
		const NzShaderProgram* program1 = mat1->GetShaderProgram(nzShaderTarget_Sprite, i);
		const NzShaderProgram* program2 = mat2->GetShaderProgram(nzShaderTarget_Sprite, i);

		if (program1 != program2)
			return program1 < program2;
	}

	const NzTexture* diffuseMap1 = mat1->GetDiffuseMap();
	const NzTexture* diffuseMap2 = mat2->GetDiffuseMap();
	if (diffuseMap1 != diffuseMap2)
		return diffuseMap1 < diffuseMap2;

	return mat1 < mat2;
}

bool NzForwardRenderQueue::BatchedSkeletalMeshComparator::operator()(const NzSkeletalMesh* subMesh1, const NzSkeletalMesh* subMesh2)
{
	const NzIndexBuffer* iBuffer1 = subMesh1->GetIndexBuffer();
	const NzBuffer* buffer1 = (iBuffer1) ? iBuffer1->GetBuffer() : nullptr;

	const NzIndexBuffer* iBuffer2 = subMesh1->GetIndexBuffer();
	const NzBuffer* buffer2 = (iBuffer2) ? iBuffer2->GetBuffer() : nullptr;

	if (buffer1 == buffer2)
		return subMesh1 < subMesh2;
	else
		return buffer2 < buffer2;
}

bool NzForwardRenderQueue::BatchedStaticMeshComparator::operator()(const NzStaticMesh* subMesh1, const NzStaticMesh* subMesh2)
{
	const NzIndexBuffer* iBuffer1 = subMesh1->GetIndexBuffer();
	const NzBuffer* buffer1 = (iBuffer1) ? iBuffer1->GetBuffer() : nullptr;

	const NzIndexBuffer* iBuffer2 = subMesh2->GetIndexBuffer();
	const NzBuffer* buffer2 = (iBuffer2) ? iBuffer2->GetBuffer() : nullptr;

	if (buffer1 == buffer2)
	{
		buffer1 = subMesh1->GetVertexBuffer()->GetBuffer();
		buffer2 = subMesh2->GetVertexBuffer()->GetBuffer();

		if (buffer1 == buffer2)
			return subMesh1 < subMesh2;
		else
			return buffer1 < buffer2;
	}
	else
		return buffer1 < buffer2;
}
