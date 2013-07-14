// Copyright (C) 2013 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/ForwardRenderQueue.hpp>
#include <Nazara/Graphics/Camera.hpp>
#include <Nazara/Graphics/Light.hpp>
#include <Nazara/Graphics/Model.hpp>
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
				NzStaticMesh* staticMesh = static_cast<NzStaticMesh*>(subMesh);
				if (material->IsEnabled(nzRendererParameter_Blend))
				{
					unsigned int index = transparentStaticModels.size();
					transparentStaticModels.resize(index+1);

					TransparentStaticModel& data = transparentStaticModels.back();
					data.aabb = staticMesh->GetAABB();
					data.aabb.Transform(transformMatrix);
					data.material = material;
					data.mesh = staticMesh;
					data.transformMatrix = transformMatrix;

					transparentsModels.push_back(std::make_pair(index, true));
				}
				else
				{
					auto pair = opaqueModels.insert(std::make_pair(material, MeshContainer::mapped_type()));
					if (pair.second)
						material->AddResourceListener(this, ResourceType_Material);

					auto& meshMap = pair.first->second.second;

					auto pair2 = meshMap.insert(std::make_pair(staticMesh, StaticMeshContainer::mapped_type()));
					if (pair2.second)
						staticMesh->AddResourceListener(this, ResourceType_StaticMesh);

					std::vector<StaticData>& staticDataContainer = pair2.first->second;

					staticDataContainer.resize(staticDataContainer.size()+1);
					StaticData& data = staticDataContainer.back();

					data.aabb = staticMesh->GetAABB();
					data.aabb.Transform(transformMatrix);
					data.transformMatrix = transformMatrix;
				}

				break;
			}
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
		opaqueModels.clear();
}

void NzForwardRenderQueue::Sort(const NzCamera& camera)
{
	struct TransparentModelComparator
	{
		bool operator()(const std::pair<unsigned int, bool>& index1, const std::pair<unsigned int, bool>& index2)
		{
			const NzBoxf& aabb1 = (index1.second) ?
			                     queue->transparentStaticModels[index1.first].aabb :
			                     queue->transparentSkeletalModels[index1.first].aabb;

			const NzBoxf& aabb2 = (index1.second) ?
			                     queue->transparentStaticModels[index2.first].aabb :
			                     queue->transparentSkeletalModels[index2.first].aabb;

			NzVector3f position1 = aabb1.GetNegativeVertex(cameraNormal);
			NzVector3f position2 = aabb2.GetNegativeVertex(cameraNormal);

			return nearPlane.Distance(position1) < nearPlane.Distance(position2);
		}

		NzForwardRenderQueue* queue;
		NzPlanef nearPlane;
		NzVector3f cameraNormal;
	};

	TransparentModelComparator comparator {this, camera.GetFrustum().GetPlane(nzFrustumPlane_Near), camera.GetForward()};
	std::sort(transparentsModels.begin(), transparentsModels.end(), comparator);
}

void NzForwardRenderQueue::OnResourceDestroy(const NzResource* resource, int index)
{
	switch (index)
	{
		case ResourceType_Material:
			opaqueModels.erase(static_cast<const NzMaterial*>(resource));
			break;

		case ResourceType_SkeletalMesh:
		{
			for (auto& pair : opaqueModels)
				pair.second.first.erase(static_cast<const NzSkeletalMesh*>(resource));

			break;
		}

		case ResourceType_StaticMesh:
		{
			for (auto& pair : opaqueModels)
				pair.second.second.erase(static_cast<const NzStaticMesh*>(resource));

			break;
		}
	}

	resource->RemoveResourceListener(this);
}

bool NzForwardRenderQueue::SkeletalMeshComparator::operator()(const NzSkeletalMesh* subMesh1, const NzSkeletalMesh* subMesh2)
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

bool NzForwardRenderQueue::StaticMeshComparator::operator()(const NzStaticMesh* subMesh1, const NzStaticMesh* subMesh2)
{
	const NzIndexBuffer* iBuffer1 = subMesh1->GetIndexBuffer();
	const NzBuffer* buffer1 = (iBuffer1) ? iBuffer1->GetBuffer() : nullptr;

	const NzIndexBuffer* iBuffer2 = subMesh1->GetIndexBuffer();
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

bool NzForwardRenderQueue::ModelMaterialComparator::operator()(const NzMaterial* mat1, const NzMaterial* mat2)
{
	///TODO: Comparaison des shaders
	for (unsigned int i = 0; i <= nzShaderFlags_Max; ++i)
	{
		const NzShader* shader1 = mat1->GetShader(nzShaderTarget_Model, i);
		const NzShader* shader2 = mat2->GetShader(nzShaderTarget_Model, i);

		if (shader1 != shader2)
			return shader1 < shader2;
	}

	const NzTexture* diffuseMap1 = mat1->GetDiffuseMap();
	const NzTexture* diffuseMap2 = mat2->GetDiffuseMap();
	if (diffuseMap1 != diffuseMap2)
		return diffuseMap1 < diffuseMap2;

	return mat1 < mat2;
}
