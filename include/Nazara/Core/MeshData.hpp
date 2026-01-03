// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_CORE_MESHDATA_HPP
#define NAZARA_CORE_MESHDATA_HPP

#include <Nazara/Core/Enums.hpp>

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

#endif // NAZARA_CORE_MESHDATA_HPP
