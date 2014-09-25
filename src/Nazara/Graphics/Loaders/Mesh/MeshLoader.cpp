// Copyright (C) 2014 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/Loaders/Mesh.hpp>
#include <Nazara/Graphics/Material.hpp>
#include <Nazara/Graphics/Model.hpp>
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

		std::unique_ptr<NzMesh> mesh(new NzMesh);
		mesh->SetPersistent(false);
		if (!mesh->LoadFromStream(stream, parameters.mesh))
		{
			NazaraError("Failed to load model mesh");
			return false;
		}

		if (mesh->IsAnimable())
		{
			NazaraError("Can't load static mesh into animated model");
			return false;
		}

		// Nous ne pouvons plus avoir recours au smart pointeur à partir d'ici si nous voulons être exception-safe
		NzMesh* meshPtr = mesh.get();

		model->Reset();
		model->SetMesh(meshPtr);
		mesh.release();

		if (parameters.loadMaterials)
		{
			unsigned int matCount = model->GetMaterialCount();

			for (unsigned int i = 0; i < matCount; ++i)
			{
				NzString mat = meshPtr->GetMaterial(i);
				if (!mat.IsEmpty())
				{
					std::unique_ptr<NzMaterial> material(new NzMaterial);
					material->SetPersistent(false);

					if (material->LoadFromFile(mat, parameters.material))
					{
						model->SetMaterial(i, material.get());
						material.release();
					}
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
}

void NzLoaders_Mesh_Unregister()
{
	NzModelLoader::UnregisterLoader(NzMeshLoader::IsExtensionSupported, CheckStatic, LoadStatic);
}
