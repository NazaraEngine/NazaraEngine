// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_UBERSHADERINSTANCE_HPP
#define NAZARA_UBERSHADERINSTANCE_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Renderer/Shader.hpp>

namespace Nz
{
	class NAZARA_RENDERER_API UberShaderInstance
	{
		public:
			UberShaderInstance(const Shader* shader);
			virtual ~UberShaderInstance();

			virtual bool Activate() const = 0;

			const Shader* GetShader() const;

		protected:
			ShaderConstRef m_shader;
	};
}

#endif // NAZARA_UBERSHADERINSTANCE_HPP
