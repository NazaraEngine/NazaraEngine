// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_MODEL_HPP
#define NAZARA_MODEL_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/Resource.hpp>
#include <Nazara/Core/ResourceLoader.hpp>
#include <Nazara/Graphics/Material.hpp>
#include <Nazara/Graphics/SceneNode.hpp>
#include <Nazara/Utility/Mesh.hpp>

struct NAZARA_API NzModelParameters
{
	NzModelParameters();

	bool loadMaterials = true;
	NzMaterialParams material;
	NzMeshParams mesh;

	bool IsValid() const;
};

class NzModel;

using NzModelLoader = NzResourceLoader<NzModel, NzModelParameters>;

class NAZARA_API NzModel : public NzResource, public NzSceneNode
{
	friend NzModelLoader;
	friend class NzScene;

	public:
		NzModel();
		NzModel(const NzModel& model);
		virtual ~NzModel();

		void AddToRenderQueue(NzAbstractRenderQueue* renderQueue) const override;

		NzModel* Clone() const;
		NzModel* Create() const;

		NzMaterial* GetMaterial(const NzString& subMeshName) const;
		NzMaterial* GetMaterial(unsigned int matIndex) const;
		NzMaterial* GetMaterial(unsigned int skinIndex, const NzString& subMeshName) const;
		NzMaterial* GetMaterial(unsigned int skinIndex, unsigned int matIndex) const;
		unsigned int GetMaterialCount() const;
		unsigned int GetSkin() const;
		unsigned int GetSkinCount() const;
		NzMesh* GetMesh() const;
		nzSceneNodeType GetSceneNodeType() const override;

		virtual bool IsAnimated() const;
		bool IsDrawable() const;

		void InvalidateBoundingVolume();

		bool LoadFromFile(const NzString& filePath, const NzModelParameters& params = NzModelParameters());
		bool LoadFromMemory(const void* data, std::size_t size, const NzModelParameters& params = NzModelParameters());
		bool LoadFromStream(NzInputStream& stream, const NzModelParameters& params = NzModelParameters());

		void Reset();

		bool SetMaterial(const NzString& subMeshName, NzMaterial* material);
		void SetMaterial(unsigned int matIndex, NzMaterial* material);
		bool SetMaterial(unsigned int skinIndex, const NzString& subMeshName, NzMaterial* material);
		void SetMaterial(unsigned int skinIndex, unsigned int matIndex, NzMaterial* material);
		virtual void SetMesh(NzMesh* mesh);
		bool SetSequence(const NzString& sequenceName);
		void SetSequence(unsigned int sequenceIndex);
		void SetSkin(unsigned int skin);
		void SetSkinCount(unsigned int skinCount);

		NzModel& operator=(const NzModel& node);

	protected:
		void MakeBoundingVolume() const override;

		std::vector<NzMaterialRef> m_materials;
		NzMeshRef m_mesh;
		unsigned int m_matCount;
		unsigned int m_skin;
		unsigned int m_skinCount;

		static NzModelLoader::LoaderList s_loaders;
};

#endif // NAZARA_MODEL_HPP
