// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_UBERSHADERINSTANCE_HPP
#define NAZARA_UBERSHADERINSTANCE_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Renderer/Shader.hpp>

class NAZARA_RENDERER_API NzUberShaderInstance
{
	public:
		NzUberShaderInstance(const NzShader* shader);
		NzUberShaderInstance(const NzUberShaderInstance&) = delete;
		NzUberShaderInstance(NzUberShaderInstance&&) = delete;
		virtual ~NzUberShaderInstance();

		virtual bool Activate() const = 0;

		const NzShader* GetShader() const;

		NzUberShaderInstance& operator=(const NzUberShaderInstance&) = delete;
		NzUberShaderInstance& operator=(NzUberShaderInstance&&) = delete;

	protected:
		NzShaderConstRef m_shader;
};

#endif // NAZARA_UBERSHADERINSTANCE_HPP
