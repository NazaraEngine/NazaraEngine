// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_SUBMESH_HPP
#define NAZARA_SUBMESH_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Core/ObjectRef.hpp>
#include <Nazara/Core/RefCounted.hpp>
#include <Nazara/Math/Box.hpp>
#include <Nazara/Utility/Enums.hpp>
#include <Nazara/Utility/IndexBuffer.hpp>
#include <Nazara/Utility/VertexBuffer.hpp>

namespace Nz
{
	class Mesh;
	class SubMesh;

	using SubMeshConstRef = ObjectRef<const SubMesh>;
	using SubMeshRef = ObjectRef<SubMesh>;

	class NAZARA_UTILITY_API SubMesh : public RefCounted
	{
		friend Mesh;

		public:
			SubMesh();

			NAZARA_DEPRECATED("Submesh constructor taking a mesh is deprecated, submeshes no longer require to be part of a single mesh")
			SubMesh(const Mesh* parent);

			SubMesh(const SubMesh&) = delete;
			SubMesh(SubMesh&&) = delete;
			virtual ~SubMesh();

			void GenerateNormals();
			void GenerateNormalsAndTangents();
			void GenerateTangents();

			virtual const Boxf& GetAABB() const = 0;
			virtual AnimationType GetAnimationType() const = 0;
			virtual const IndexBuffer* GetIndexBuffer() const = 0;
			UInt32 GetMaterialIndex() const;
			PrimitiveMode GetPrimitiveMode() const;
			UInt32 GetTriangleCount() const;
			virtual UInt32 GetVertexCount() const = 0;

			virtual bool IsAnimated() const = 0;

			void SetMaterialIndex(UInt32 matIndex);
			void SetPrimitiveMode(PrimitiveMode mode);

			SubMesh& operator=(const SubMesh&) = delete;
			SubMesh& operator=(SubMesh&&) = delete;

			// Signals:
			NazaraSignal(OnSubMeshInvalidateAABB, const SubMesh* /*subMesh*/);
			NazaraSignal(OnSubMeshRelease, const SubMesh* /*subMesh*/);

		protected:
			PrimitiveMode m_primitiveMode;
			UInt32 m_matIndex;
	};
}

#endif // NAZARA_SUBMESH_HPP
