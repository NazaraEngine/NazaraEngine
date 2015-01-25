// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/Loaders/Mesh.hpp>
#include <Nazara/Graphics/Material.hpp>
#include <Nazara/Graphics/Model.hpp>
#include <Nazara/Graphics/SkeletalModel.hpp>
#include <Nazara/Utility/Mesh.hpp>
#include <memory>
#include <Nazara/Graphics/Debug.hpp>

namespace
{
	nzTernary CheckStatic(NzInputStream& stream, const NzModelParameters& parameters)
	{
		NazaraUnused(stream);
		NazaraUnused(parameters);

		return nzTernary_Unknown;
	}

	bool LoadStatic(NzModel* model, NzInputStream& stream, const NzModelParameters& parameters)
	{
		NazaraUnused(parameters);

		NzMeshRef mesh = NzMesh::New();
		if (!mesh->LoadFromStream(stream, parameters.mesh))
		{
			NazaraError("Failed to load model mesh");
			return false;
		}

		if (mesh->IsAnimable())
		{
			NazaraError("Can't load animated mesh into static model");
			return false;
		}

		model->Reset();
		model->SetMesh(mesh);

		if (parameters.loadMaterials)
		{
			unsigned int matCount = model->GetMaterialCount();

			for (unsigned int i = 0; i < matCount; ++i)
			{
				NzString mat = mesh->GetMaterial(i);
				if (!mat.IsEmpty())
				{
					NzMaterialRef material = NzMaterial::New();
					if (material->LoadFromFile(mat, parameters.material))
						model->SetMaterial(i, material);
					else
						NazaraWarning("Failed to load material #" + NzString::Number(i));
				}
			}
		}

		return true;
	}

	nzTernary CheckAnimated(NzInputStream& stream, const NzSkeletalModelParameters& parameters)
	{
		NazaraUnused(stream);
		NazaraUnused(parameters);

		return nzTernary_Unknown;
	}

	bool LoadAnimated(NzSkeletalModel* model, NzInputStream& stream, const NzSkeletalModelParameters& parameters)
	{
		NazaraUnused(parameters);

		NzMeshRef mesh = NzMesh::New();
		if (!mesh->LoadFromStream(stream, parameters.mesh))
		{
			NazaraError("Failed to load model mesh");
			return false;
		}

		if (!mesh->IsAnimable())
		{
			NazaraError("Can't load static mesh into animated model");
			return false;
		}

		model->Reset();
		model->SetMesh(mesh);

		if (parameters.loadMaterials)
		{
			unsigned int matCount = model->GetMaterialCount();

			for (unsigned int i = 0; i < matCount; ++i)
			{
				NzString mat = mesh->GetMaterial(i);
				if (!mat.IsEmpty())
				{
					NzMaterialRef material = NzMaterial::New();
					if (material->LoadFromFile(mat, parameters.material))
						model->SetMaterial(i, material);
					else
						NazaraWarning("Failed to load material #" + NzString::Number(i));
				}
			}
		}

		return true;
	}
}

void NzLoaders_Mesh_Register()
{
	NzModelLoader::RegisterLoader(NzMeshLoader::IsExtensionSupported, CheckStatic, LoadStatic);
	NzSkeletalModelLoader::RegisterLoader(NzMeshLoader::IsExtensionSupported, CheckAnimated, LoadAnimated);
}

void NzLoaders_Mesh_Unregister()
{
	NzModelLoader::UnregisterLoader(NzMeshLoader::IsExtensionSupported, CheckStatic, LoadStatic);
	NzSkeletalModelLoader::UnregisterLoader(NzMeshLoader::IsExtensionSupported, CheckAnimated, LoadAnimated);
}
