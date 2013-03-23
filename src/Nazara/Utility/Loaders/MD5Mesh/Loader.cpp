// Copyright (C) 2013 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Utility/Loaders/MD5Mesh.hpp>
#include <Nazara/Utility/Loaders/MD5Mesh/Parser.hpp>
#include <Nazara/Utility/Debug.hpp>

namespace
{
	bool IsSupported(const NzString& extension)
	{
		return (extension == "md5mesh");
	}

	bool Check(NzInputStream& stream, const NzMeshParams& parameters)
	{
		NzMD5MeshParser parser(stream, parameters);
		return parser.Check();
	}

	bool Load(NzMesh* mesh, NzInputStream& stream, const NzMeshParams& parameters)
	{
		NzMD5MeshParser parser(stream, parameters);
		return parser.Parse(mesh);
	}
}

void NzLoaders_MD5Mesh_Register()
{
	NzMeshLoader::RegisterLoader(IsSupported, Check, Load);
}

void NzLoaders_MD5Mesh_Unregister()
{
	NzMeshLoader::UnregisterLoader(IsSupported, Check, Load);
}
