// Copyright (C) 2014 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_UBERSHADERINSTANCEPREPROCESSOR_HPP
#define NAZARA_UBERSHADERINSTANCEPREPROCESSOR_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Renderer/UberShaderInstance.hpp>

class NAZARA_API NzUberShaderInstancePreprocessor : public NzUberShaderInstance
{
	public:
		NzUberShaderInstancePreprocessor(const NzShader* shader);
		virtual ~NzUberShaderInstancePreprocessor();

		bool Activate() const;
};

#endif // NAZARA_UBERSHADERINSTANCEPREPROCESSOR_HPP
