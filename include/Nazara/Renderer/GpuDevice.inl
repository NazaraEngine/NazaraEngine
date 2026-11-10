// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Export.hpp

namespace Nz
{
	inline void GpuDevice::SubmitAsyncCommands(std::unique_ptr<GpuAsyncCommands>&& transfer, bool waitForCompletion)
	{
		return SubmitAsyncCommands(std::span(&transfer, 1), waitForCompletion);
	}
}
