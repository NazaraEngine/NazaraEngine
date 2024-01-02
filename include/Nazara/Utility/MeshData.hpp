// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_UTILITY_MESHDATA_HPP
#define NAZARA_UTILITY_MESHDATA_HPP

#include <Nazara/Utility/Enums.hpp>

namespace Nz
{
	class IndexBuffer;
	class VertexBuffer;

	struct MeshData
	{
		PrimitiveMode primitiveMode;
		const IndexBuffer* indexBuffer;
		const VertexBuffer* vertexBuffer;
	};
}

#endif // NAZARA_UTILITY_MESHDATA_HPP
