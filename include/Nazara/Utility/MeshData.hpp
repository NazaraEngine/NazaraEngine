// Copyright (C) 2014 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_MESHDATA_HPP
#define NAZARA_MESHDATA_HPP

#include <Nazara/Utility/Enums.hpp>

class NzIndexBuffer;
class NzVertexBuffer;

struct NzMeshData
{
	nzPrimitiveMode primitiveMode;
	const NzIndexBuffer* indexBuffer;
	const NzVertexBuffer* vertexBuffer;
};

#endif // NAZARA_MESHDATA_HPP
