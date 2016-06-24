// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_MODEL_HPP
#define NAZARA_MODEL_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/Bitset.hpp>
#include <Nazara/Core/Resource.hpp>
#include <Nazara/Core/ResourceLoader.hpp>
#include <Nazara/Core/ResourceParameters.hpp>
#include <Nazara/Graphics/InstancedRenderable.hpp>
#include <Nazara/Graphics/Material.hpp>
#include <Nazara/Utility/Mesh.hpp>

namespace Nz
{
	struct NAZARA_GRAPHICS_API ModelParameters : ResourceParameters
	{
		ModelParameters();

		bool loadMaterials = true;
		MaterialParams material;
		MeshParams mesh;

		bool IsValid() const;
	};

	class Model;

	using ModelConstRef = ObjectRef<const Model>;
	using ModelLoader = ResourceLoader<Model, ModelParameters>;
	using ModelRef = ObjectRef<Model>;

	class NAZARA_GRAPHICS_API Model : public InstancedRenderable, public Resource
	{
		friend ModelLoader;

		public:
			Model();
			Model(const Model& model) = default;
			Model(Model&& model) = default;
			virtual ~Model();

			void AddToRenderQueue(AbstractRenderQueue* renderQueue, const InstanceData& instanceData) const override;

			Material* GetMaterial(const String& subMeshName) const;
			Material* GetMaterial(unsigned int matIndex) const;
			Material* GetMaterial(unsigned int skinIndex, const String& subMeshName) const;
			Material* GetMaterial(unsigned int skinIndex, unsigned int matIndex) const;
			unsigned int GetMaterialCount() const;
			unsigned int GetSkin() const;
			unsigned int GetSkinCount() const;
			Mesh* GetMesh() const;

			virtual bool IsAnimated() const;

			bool LoadFromFile(const String& filePath, const ModelParameters& params = ModelParameters());
			bool LoadFromMemory(const void* data, std::size_t size, const ModelParameters& params = ModelParameters());
			bool LoadFromStream(Stream& stream, const ModelParameters& params = ModelParameters());

			void Reset();

			bool SetMaterial(const String& subMeshName, Material* material);
			void SetMaterial(unsigned int matIndex, Material* material);
			bool SetMaterial(unsigned int skinIndex, const String& subMeshName, Material* material);
			void SetMaterial(unsigned int skinIndex, unsigned int matIndex, Material* material);
			virtual void SetMesh(Mesh* mesh);
			void SetSkin(unsigned int skin);
			void SetSkinCount(unsigned int skinCount);

			inline void ShowSubmeshes(Bitset<> enabledSubmeshes);

			Model& operator=(const Model& node) = default;
			Model& operator=(Model&& node) = default;

			template<typename... Args> static ModelRef New(Args&&... args);

		protected:
			void MakeBoundingVolume() const override;

			std::vector<MaterialRef> m_materials;
			Bitset<> m_enabledSubmeshes;
			MeshRef m_mesh;
			unsigned int m_matCount;
			unsigned int m_skin;
			unsigned int m_skinCount;

			static ModelLoader::LoaderList s_loaders;
	};
}

#include <Nazara/Graphics/Model.inl>

#endif // NAZARA_MODEL_HPP
