// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/Formats/MeshLoader.hpp>
#include <Nazara/Core/ErrorFlags.hpp>
#include <Nazara/Graphics/Material.hpp>
#include <Nazara/Graphics/Model.hpp>
#include <Nazara/Graphics/SkeletalModel.hpp>
#include <Nazara/Utility/MaterialData.hpp>
#include <Nazara/Utility/Mesh.hpp>
#include <memory>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	namespace
	{
		void LoadMaterials(Model* model, const ModelParameters& parameters)
		{
			unsigned int matCount = model->GetMaterialCount();

			for (unsigned int i = 0; i < matCount; ++i)
			{
				const ParameterList& matData = model->GetMesh()->GetMaterialData(i);

				String filePath;
				if (matData.GetStringParameter(MaterialData::FilePath, &filePath))
				{
					if (!File::Exists(filePath))
					{
						NazaraWarning("Shader name does not refer to an existing file, \".tga\" is used by default");
						filePath += ".tga";
					}

					MaterialRef material = Material::New();
					if (material->LoadFromFile(filePath, parameters.material))
						model->SetMaterial(i, std::move(material));
					else
						NazaraWarning("Failed to load material from file " + String::Number(i));
				}
				else
				{
					MaterialRef material = Material::New();
					material->BuildFromParameters(matData, parameters.material);

					model->SetMaterial(i, std::move(material));
				}
			}
		}

		Ternary CheckStatic(Stream& stream, const ModelParameters& parameters)
		{
			NazaraUnused(stream);

			bool skip;
			if (parameters.custom.GetBooleanParameter("SkipNativeMeshLoader", &skip) && skip)
				return Ternary_False;

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

			model->SetMesh(mesh);

			if (parameters.loadMaterials)
				LoadMaterials(model, parameters);

			return true;
		}

		Ternary CheckAnimated(Stream& stream, const SkeletalModelParameters& parameters)
		{
			NazaraUnused(stream);

			bool skip;
			if (parameters.custom.GetBooleanParameter("SkipNativeAnimatedMeshLoader", &skip) && skip)
				return Ternary_False;

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

			model->SetMesh(mesh);

			if (parameters.loadMaterials)
				LoadMaterials(model, parameters);

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

