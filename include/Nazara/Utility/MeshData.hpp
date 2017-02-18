// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_MESHDATA_HPP
#define NAZARA_MESHDATA_HPP

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

#endif // NAZARA_MESHDATA_HPP
