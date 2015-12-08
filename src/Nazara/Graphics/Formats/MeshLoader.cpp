// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/Formats/MeshLoader.hpp>
#include <Nazara/Graphics/Material.hpp>
#include <Nazara/Graphics/Model.hpp>
#include <Nazara/Graphics/SkeletalModel.hpp>
#include <Nazara/Utility/Mesh.hpp>
#include <memory>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	namespace
	{
		Ternary CheckStatic(Stream& stream, const ModelParameters& parameters)
		{
			NazaraUnused(stream);
			NazaraUnused(parameters);

			return Ternary_Unknown;
		}

		bool LoadStatic(Model* model, Stream& stream, const ModelParameters& parameters)
		{
			NazaraUnused(parameters);

			MeshRef mesh = Mesh::New();
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
					String mat = mesh->GetMaterial(i);
					if (!mat.IsEmpty())
					{
						MaterialRef material = Material::New();
						if (material->LoadFromFile(mat, parameters.material))
							model->SetMaterial(i, material);
						else
							NazaraWarning("Failed to load material #" + String::Number(i));
					}
				}
			}

			return true;
		}

		Ternary CheckAnimated(Stream& stream, const SkeletalModelParameters& parameters)
		{
			NazaraUnused(stream);
			NazaraUnused(parameters);

			return Ternary_Unknown;
		}

		bool LoadAnimated(SkeletalModel* model, Stream& stream, const SkeletalModelParameters& parameters)
		{
			NazaraUnused(parameters);

			MeshRef mesh = Mesh::New();
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
					String mat = mesh->GetMaterial(i);
					if (!mat.IsEmpty())
					{
						MaterialRef material = Material::New();
						if (material->LoadFromFile(mat, parameters.material))
							model->SetMaterial(i, material);
						else
							NazaraWarning("Failed to load material #" + String::Number(i));
					}
				}
			}

			return true;
		}
	}

	namespace Loaders
	{
		void RegisterMesh()
		{
			ModelLoader::RegisterLoader(MeshLoader::IsExtensionSupported, CheckStatic, LoadStatic);
			SkeletalModelLoader::RegisterLoader(MeshLoader::IsExtensionSupported, CheckAnimated, LoadAnimated);
		}

		void UnregisterMesh()
		{
			ModelLoader::UnregisterLoader(MeshLoader::IsExtensionSupported, CheckStatic, LoadStatic);
			SkeletalModelLoader::UnregisterLoader(MeshLoader::IsExtensionSupported, CheckAnimated, LoadAnimated);
		}
	}
}

