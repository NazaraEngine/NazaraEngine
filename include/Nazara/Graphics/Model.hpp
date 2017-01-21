// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_MODEL_HPP
#define NAZARA_MODEL_HPP

#include <Nazara/Prerequesites.hpp>
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
			inline Model();
			Model(const Model& model) = default;
			Model(Model&& model) = default;
			virtual ~Model();

			void AddToRenderQueue(AbstractRenderQueue* renderQueue, const InstanceData& instanceData) const override;
			inline void AddToRenderQueue(AbstractRenderQueue* renderQueue, const Matrix4f& transformMatrix, unsigned int renderOrder = 0);

			using InstancedRenderable::GetMaterial;
			const MaterialRef& GetMaterial(const String& subMeshName) const;
			const MaterialRef& GetMaterial(std::size_t skinIndex, const String& subMeshName) const;
			Mesh* GetMesh() const;

			virtual bool IsAnimated() const;

			bool LoadFromFile(const String& filePath, const ModelParameters& params = ModelParameters());
			bool LoadFromMemory(const void* data, std::size_t size, const ModelParameters& params = ModelParameters());
			bool LoadFromStream(Stream& stream, const ModelParameters& params = ModelParameters());

			using InstancedRenderable::SetMaterial;
			bool SetMaterial(const String& subMeshName, MaterialRef material);
			bool SetMaterial(std::size_t skinIndex, const String& subMeshName, MaterialRef material);

			virtual void SetMesh(Mesh* mesh);

			Model& operator=(const Model& node) = default;
			Model& operator=(Model&& node) = default;

			template<typename... Args> static ModelRef New(Args&&... args);

		protected:
			void MakeBoundingVolume() const override;

			MeshRef m_mesh;

			static ModelLoader::LoaderList s_loaders;
	};
}

#include <Nazara/Graphics/Model.inl>

#endif // NAZARA_MODEL_HPP
