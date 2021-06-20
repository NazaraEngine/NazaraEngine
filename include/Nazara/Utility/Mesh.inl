// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Utility/Mesh.hpp>
#include <memory>
#include <Nazara/Utility/StaticMesh.hpp>
#include <Nazara/Utility/Debug.hpp>

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
}

#include <Nazara/Utility/DebugOff.hpp>
