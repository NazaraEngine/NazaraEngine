// Copyright (C) 2021 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Shader module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_SPIRVSECTION_HPP
#define NAZARA_SPIRVSECTION_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Shader/SpirvSectionBase.hpp>

namespace Nz
{
	class NAZARA_SHADER_API SpirvSection : public SpirvSectionBase
	{
		public:
			SpirvSection() = default;
			SpirvSection(const SpirvSection&) = default;
			SpirvSection(SpirvSection&&) = default;
			~SpirvSection() = default;

			using SpirvSectionBase::Append;
			using SpirvSectionBase::AppendRaw;
			using SpirvSectionBase::AppendSection;
			using SpirvSectionBase::AppendVariadic;

			SpirvSection& operator=(const SpirvSection&) = delete;
			SpirvSection& operator=(SpirvSection&&) = default;
	};
}

#include <Nazara/Shader/SpirvSection.inl>

#endif
