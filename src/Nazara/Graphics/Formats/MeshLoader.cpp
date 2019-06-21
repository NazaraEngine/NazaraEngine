// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/Formats/MeshLoader.hpp>
#include <Nazara/Graphics/Material.hpp>
#include <Nazara/Graphics/Model.hpp>
#include <Nazara/Graphics/SkeletalModel.hpp>
#include <Nazara/Utility/MaterialData.hpp>
#include <Nazara/Utility/Mesh.hpp>
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

					if (MaterialRef material = Material::LoadFromFile(filePath, parameters.material))
						model->SetMaterial(i, std::move(material));
					else
						NazaraWarning("Failed to load material from file " + String::Number(i));
				}
				else
				{
					/*MaterialRef material = Material::New();
					material->BuildFromParameters(matData, parameters.material);

					model->SetMaterial(i, std::move(material));*/
				}
			}
		}

		Ternary Check(Stream& stream, const ModelParameters& parameters)
		{
			NazaraUnused(stream);

			bool skip;
			if (parameters.custom.GetBooleanParameter("SkipNativeMeshLoader", &skip) && skip)
				return Ternary_False;

			return Ternary_Unknown;
		}

		ModelRef Load(Stream& stream, const ModelParameters& parameters)
		{
			NazaraUnused(parameters);

			MeshRef mesh = Mesh::LoadFromStream(stream, parameters.mesh);
			if (!mesh)
			{
				NazaraError("Failed to load model mesh");
				return nullptr;
			}

			ModelRef model;
			if (mesh->IsAnimable())
				model = SkeletalModel::New();
			else
				model = Model::New();

			model->SetMesh(mesh);

			if (parameters.loadMaterials)
				LoadMaterials(model, parameters);

			return model;
		}
	}

	namespace Loaders
	{
		void RegisterMesh()
		{
			ModelLoader::RegisterLoader(MeshLoader::IsExtensionSupported, Check, Load);
		}

		void UnregisterMesh()
		{
			ModelLoader::UnregisterLoader(MeshLoader::IsExtensionSupported, Check, Load);
		}
	}
}

