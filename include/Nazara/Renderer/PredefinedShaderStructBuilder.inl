// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Export.hpp

namespace Nz
{
NAZARA_WARNING_PUSH()
NAZARA_WARNING_CLANG_GCC_DISABLE("-Wmissing-field-initializers")

	constexpr DebugDrawerData DebugDrawerData::Build()
	{
		DebugDrawerData debugDrawerData = { nzsl::FieldOffsets(nzsl::StructLayout::Std140) };
		debugDrawerData.viewProjOffset = debugDrawerData.fieldOffsets.AddMatrix(nzsl::StructFieldType::Float1, 4, 4, true);

		debugDrawerData.totalSize = debugDrawerData.fieldOffsets.GetAlignedSize();

		return debugDrawerData;
	}

NAZARA_WARNING_POP()
}
