// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_RENDERER_DRAWINDIRECT_HPP
#define NAZARA_RENDERER_DRAWINDIRECT_HPP

#include <NazaraUtils/Prerequisites.hpp>

namespace Nz
{
	struct DrawIndirectCommand
	{
		UInt32 vertexCount;
		UInt32 instanceCount;
		UInt32 firstVertex;
		UInt32 firstInstance;
	};

	static_assert(sizeof(DrawIndirectCommand) == 16);
	static_assert(alignof(DrawIndirectCommand) == 4);

	struct DrawIndexedIndirectCommand
	{
		UInt32 indexCount;
		UInt32 instanceCount;
		UInt32 firstIndex;
		Int32 vertexOffset;
		UInt32 firstInstance;
	};

	static_assert(sizeof(DrawIndexedIndirectCommand) == 20);
	static_assert(alignof(DrawIndexedIndirectCommand) == 4);
}

#endif // NAZARA_RENDERER_DRAWINDIRECT_HPP
