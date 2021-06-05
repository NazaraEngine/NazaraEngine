// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_MESH_HPP
#define NAZARA_MESH_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Core/ObjectLibrary.hpp>
#include <Nazara/Core/Resource.hpp>
#include <Nazara/Core/ResourceLoader.hpp>
#include <Nazara/Core/ResourceManager.hpp>
#include <Nazara/Core/ResourceParameters.hpp>
#include <Nazara/Core/ResourceSaver.hpp>
#include <Nazara/Math/Box.hpp>
#include <Nazara/Utility/Config.hpp>
#include <Nazara/Utility/Enums.hpp>
#include <Nazara/Utility/Skeleton.hpp>
#include <Nazara/Utility/SubMesh.hpp>
#include <Nazara/Utility/VertexDeclaration.hpp>
#include <Nazara/Utility/VertexStruct.hpp>
#include <unordered_map>
#include <vector>

namespace Nz
{
	struct NAZARA_UTILITY_API MeshParams : ResourceParameters
	{
		MeshParams();

		BufferUsageFlags indexBufferFlags = 0;      ///< Buffer usage flags used to build the index buffers
		BufferUsageFlags vertexBufferFlags = 0;     ///< Buffer usage flags used to build the vertex buffers
		Matrix4f matrix = Matrix4f::Identity();     ///< A matrix which will transform every vertex position
		DataStorage storage = DataStorage::Hardware; ///< The place where the buffers will be allocated
		Vector2f texCoordOffset = {0.f, 0.f};       ///< Offset to apply on the texture coordinates (not scaled)
		Vector2f texCoordScale  = {1.f, 1.f};       ///< Scale to apply on the texture coordinates
		bool animated = true;                       ///< If true, will load an animated version of the model if possible
		bool center = false;                        ///< If true, will center the mesh vertices around the origin
		#ifndef NAZARA_DEBUG
		bool optimizeIndexBuffers = true;           ///< Optimize the index buffers after loading, improve cache locality (and thus rendering speed) but increase loading time.
		#else
		bool optimizeIndexBuffers = false;          ///< Since this optimization take a lot of time, especially in debug mode, don't enable it by default in debug.
		#endif

		/* The declaration must have a Vector3f position component enabled
		 * If the declaration has a Vector2f UV component enabled, UV are generated
		 * If the declaration has a Vector3f Normals component enabled, Normals are generated.
		 * If the declaration has a Vector3f Tangents component enabled, Tangents are generated.
		 */
		std::shared_ptr<VertexDeclaration> vertexDeclaration = VertexDeclaration::Get(VertexLayout::XYZ_Normal_UV_Tangent);

		bool IsValid() const;
	};

	class Mesh;
	struct Primitive;
	class PrimitiveList;
	class SubMesh;

	using MeshVertex = VertexStruct_XYZ_Normal_UV_Tangent;
	using SkeletalMeshVertex = VertexStruct_XYZ_Normal_UV_Tangent_Skinning;

	using MeshLibrary = ObjectLibrary<Mesh>;
	using MeshLoader = ResourceLoader<Mesh, MeshParams>;
	using MeshManager = ResourceManager<Mesh, MeshParams>;
	using MeshSaver = ResourceSaver<Mesh, MeshParams>;

	struct MeshImpl;

	class NAZARA_UTILITY_API Mesh : public Resource
	{
		public:
			inline Mesh();
			Mesh(const Mesh&) = delete;
			Mesh(Mesh&&) = delete;
			~Mesh() = default;

			void AddSubMesh(std::shared_ptr<SubMesh> subMesh);
			void AddSubMesh(const std::string& identifier, std::shared_ptr<SubMesh> subMesh);

			std::shared_ptr<SubMesh> BuildSubMesh(const Primitive& primitive, const MeshParams& params = MeshParams());
			void BuildSubMeshes(const PrimitiveList& list, const MeshParams& params = MeshParams());

			bool CreateSkeletal(std::size_t jointCount);
			bool CreateStatic();
			void Destroy();

			void GenerateNormals();
			void GenerateNormalsAndTangents();
			void GenerateTangents();

			const Boxf& GetAABB() const;
			std::filesystem::path GetAnimation() const;
			AnimationType GetAnimationType() const;
			std::size_t GetJointCount() const;
			ParameterList& GetMaterialData(std::size_t index);
			const ParameterList& GetMaterialData(std::size_t index) const;
			std::size_t GetMaterialCount() const;
			Skeleton* GetSkeleton();
			const Skeleton* GetSkeleton() const;
			const std::shared_ptr<SubMesh>& GetSubMesh(const std::string& identifier) const;
			const std::shared_ptr<SubMesh>& GetSubMesh(std::size_t index) const;
			std::size_t GetSubMeshCount() const;
			std::size_t GetSubMeshIndex(const std::string& identifier) const;
			std::size_t GetTriangleCount() const;
			std::size_t GetVertexCount() const;

			bool HasSubMesh(const std::string& identifier) const;
			bool HasSubMesh(std::size_t index = 0) const;

			void InvalidateAABB() const;

			bool IsAnimable() const;
			bool IsValid() const;

			void Recenter();

			void RemoveSubMesh(const std::string& identifier);
			void RemoveSubMesh(std::size_t index);

			bool SaveToFile(const std::filesystem::path& filePath, const MeshParams& params = MeshParams());
			bool SaveToStream(Stream& stream, const std::string& format, const MeshParams& params = MeshParams());

			void SetAnimation(const std::filesystem::path& animationPath);
			void SetMaterialCount(std::size_t matCount);
			void SetMaterialData(std::size_t matIndex, ParameterList data);

			void Transform(const Matrix4f& matrix);

			Mesh& operator=(const Mesh&) = delete;
			Mesh& operator=(Mesh&&) = delete;

			static std::shared_ptr<Mesh> LoadFromFile(const std::filesystem::path& filePath, const MeshParams& params = MeshParams());
			static std::shared_ptr<Mesh> LoadFromMemory(const void* data, std::size_t size, const MeshParams& params = MeshParams());
			static std::shared_ptr<Mesh> LoadFromStream(Stream& stream, const MeshParams& params = MeshParams());

			// Signals:
			NazaraSignal(OnMeshInvalidateAABB, const Mesh* /*mesh*/);

		private:
			struct SubMeshData
			{
				std::shared_ptr<SubMesh> subMesh;

				NazaraSlot(SubMesh, OnSubMeshInvalidateAABB, onSubMeshInvalidated);
			};

			std::size_t m_jointCount; // Only used by skeletal meshes
			std::unordered_map<std::string, std::size_t> m_subMeshMap;
			std::vector<ParameterList> m_materialData;
			std::vector<SubMeshData> m_subMeshes;
			AnimationType m_animationType;
			mutable Boxf m_aabb;
			Skeleton m_skeleton; // Only used by skeletal meshes
			std::filesystem::path m_animationPath;
			mutable bool m_aabbUpdated;
			bool m_isValid;
	};
}

#include <Nazara/Utility/Mesh.inl>

#endif // NAZARA_MESH_HPP
