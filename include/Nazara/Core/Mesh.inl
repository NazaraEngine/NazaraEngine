// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Core/StaticMesh.hpp>
#include <memory>

namespace Nz
{
	inline Mesh::Mesh() :
	m_materialData(1),
	m_aabbUpdated(false),
	m_isValid(false)
	{
	}

	inline std::shared_ptr<Mesh> Mesh::Build(std::shared_ptr<StaticMesh> staticMesh)
	{
		std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>();
		mesh->CreateStatic();
		mesh->AddSubMesh(std::move(staticMesh));

		return mesh;
	}

	inline std::shared_ptr<Mesh> Mesh::Build(const Primitive& primitive, const MeshParams& params)
	{
		std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>();
		mesh->CreateStatic();
		mesh->BuildSubMesh(primitive, params);

		return mesh;
	}

	inline std::shared_ptr<Mesh> Mesh::Build(const PrimitiveList& primitiveList, const MeshParams& params)
	{
		std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>();
		mesh->CreateStatic();
		mesh->BuildSubMeshes(primitiveList, params);

		return mesh;
	}
}
