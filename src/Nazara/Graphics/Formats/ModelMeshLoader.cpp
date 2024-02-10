// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/Formats/ModelMeshLoader.hpp>
#include <Nazara/Core/Core.hpp>
#include <Nazara/Core/Mesh.hpp>
#include <Nazara/Graphics/GraphicalMesh.hpp>
#include <Nazara/Graphics/MaterialInstance.hpp>
#include <Nazara/Graphics/Model.hpp>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz::Loaders
{
	ModelLoader::Entry GetModelLoader_Mesh()
	{
		ModelLoader::Entry loaderEntry;
		loaderEntry.extensionSupport = [](std::string_view extension)
		{
			return Core::Instance()->GetMeshLoader().IsExtensionSupported(extension);
		};

		loaderEntry.streamLoader = [](Stream& stream, const ModelParams& parameters) -> Result<std::shared_ptr<Model>, ResourceLoadingError>
		{
			std::shared_ptr<Mesh> mesh = Mesh::LoadFromStream(stream, parameters.mesh);
			if (!mesh)
				return Err(ResourceLoadingError::Unrecognized);

			if (parameters.meshCallback)
			{
				Result res = parameters.meshCallback(mesh);
				if (!res)
					return Err(res.GetError());
			}

			std::shared_ptr<GraphicalMesh> gfxMesh = GraphicalMesh::BuildFromMesh(*mesh);
			if (!gfxMesh)
				return Err(ResourceLoadingError::Internal);

			std::shared_ptr<Model> model = std::make_shared<Model>(std::move(gfxMesh));
			if (parameters.loadMaterials)
			{
				for (std::size_t matIndex = 0; matIndex < model->GetMaterialCount(); ++matIndex)
				{
					if (std::shared_ptr<MaterialInstance> matInstance = MaterialInstance::Build(mesh->GetMaterialData(matIndex)))
						model->SetMaterial(matIndex, std::move(matInstance));
				}
			}

			return model;
		};

		loaderEntry.parameterFilter = [](const ModelParams& parameters)
		{
			if (auto result = parameters.custom.GetBooleanParameter("SkipNativeMeshLoader"); result.GetValueOr(false))
				return false;

			return true;
		};

		return loaderEntry;
	}
}
