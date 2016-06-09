// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_MESH_HPP
#define NAZARA_MESH_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/ObjectLibrary.hpp>
#include <Nazara/Core/ObjectRef.hpp>
#include <Nazara/Core/Primitive.hpp>
#include <Nazara/Core/RefCounted.hpp>
#include <Nazara/Core/Resource.hpp>
#include <Nazara/Core/ResourceLoader.hpp>
#include <Nazara/Core/ResourceManager.hpp>
#include <Nazara/Core/ResourceParameters.hpp>
#include <Nazara/Core/Stream.hpp>
#include <Nazara/Core/String.hpp>
#include <Nazara/Math/Box.hpp>
#include <Nazara/Utility/Skeleton.hpp>
#include <Nazara/Utility/SubMesh.hpp>
#include <Nazara/Utility/VertexStruct.hpp>

namespace Nz
{
	struct NAZARA_UTILITY_API MeshParams : ResourceParameters
	{
		MeshParams(); // Vérifie que le storage par défaut est supporté (software autrement)

		// La transformation appliquée à tous les sommets du mesh
		Matrix4f matrix = Matrix4f::Identity();

		// Si ceci sera le stockage utilisé par les buffers
		UInt32 storage = DataStorage_Hardware;

		// Charger une version animée du mesh si possible ?
		bool animated = true;

		// Faut-il centrer le mesh autour de l'origine ?
		bool center = false;

		// Faut-il retourner les UV ?
		bool flipUVs = false;

		// Faut-il optimiser les index buffers ? (Rendu plus rapide, mais le chargement dure plus longtemps)
		bool optimizeIndexBuffers = true;

		bool IsValid() const;
	};

	class Mesh;
	class PrimitiveList;

	typedef VertexStruct_XYZ_Normal_UV_Tangent MeshVertex;
	typedef VertexStruct_XYZ_Normal_UV_Tangent_Skinning SkeletalMeshVertex;

	using MeshConstRef = ObjectRef<const Mesh>;
	using MeshLibrary = ObjectLibrary<Mesh>;
	using MeshLoader = ResourceLoader<Mesh, MeshParams>;
	using MeshManager = ResourceManager<Mesh, MeshParams>;
	using MeshRef = ObjectRef<Mesh>;

	struct MeshImpl;

	class NAZARA_UTILITY_API Mesh : public RefCounted, public Resource
	{
		friend MeshLibrary;
		friend MeshLoader;
		friend MeshManager;
		friend class Utility;

		public:
			Mesh() = default;
			~Mesh();

			void AddSubMesh(SubMesh* subMesh);
			void AddSubMesh(const String& identifier, SubMesh* subMesh);

			SubMesh* BuildSubMesh(const Primitive& primitive, const MeshParams& params = MeshParams());
			void BuildSubMeshes(const PrimitiveList& list, const MeshParams& params = MeshParams());

			bool CreateSkeletal(unsigned int jointCount);
			bool CreateStatic();
			void Destroy();

			void GenerateNormals();
			void GenerateNormalsAndTangents();
			void GenerateTangents();

			const Boxf& GetAABB() const;
			String GetAnimation() const;
			AnimationType GetAnimationType() const;
			unsigned int GetJointCount() const;
			ParameterList& GetMaterialData(unsigned int index);
			const ParameterList& GetMaterialData(unsigned int index) const;
			unsigned int GetMaterialCount() const;
			Skeleton* GetSkeleton();
			const Skeleton* GetSkeleton() const;
			SubMesh* GetSubMesh(const String& identifier);
			SubMesh* GetSubMesh(unsigned int index);
			const SubMesh* GetSubMesh(const String& identifier) const;
			const SubMesh* GetSubMesh(unsigned int index) const;
			unsigned int GetSubMeshCount() const;
			int GetSubMeshIndex(const String& identifier) const;
			unsigned int GetTriangleCount() const;
			unsigned int GetVertexCount() const;

			bool HasSubMesh(const String& identifier) const;
			bool HasSubMesh(unsigned int index = 0) const;

			void InvalidateAABB() const;

			bool IsAnimable() const;
			bool IsValid() const;

			bool LoadFromFile(const String& filePath, const MeshParams& params = MeshParams());
			bool LoadFromMemory(const void* data, std::size_t size, const MeshParams& params = MeshParams());
			bool LoadFromStream(Stream& stream, const MeshParams& params = MeshParams());

			void Recenter();

			void RemoveSubMesh(const String& identifier);
			void RemoveSubMesh(unsigned int index);

			void SetAnimation(const String& animationPath);
			void SetMaterialCount(unsigned int matCount);
			void SetMaterialData(unsigned int matIndex, ParameterList data);

			void Transform(const Matrix4f& matrix);

			template<typename... Args> static MeshRef New(Args&&... args);

			// Signals:
			NazaraSignal(OnMeshDestroy, const Mesh* /*mesh*/);
			NazaraSignal(OnMeshRelease, const Mesh* /*mesh*/);

		private:
			MeshImpl* m_impl = nullptr;

			static bool Initialize();
			static void Uninitialize();

			static MeshLibrary::LibraryMap s_library;
			static MeshLoader::LoaderList s_loaders;
			static MeshManager::ManagerMap s_managerMap;
			static MeshManager::ManagerParams s_managerParameters;
	};
}

#include <Nazara/Utility/Mesh.inl>

#endif // NAZARA_MESH_HPP
