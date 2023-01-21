// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_CORE_APPLICATION_HPP
#define NAZARA_CORE_APPLICATION_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Core/ApplicationBase.hpp>
#include <Nazara/Core/Modules.hpp>

namespace Nz
{
	template<typename... ModuleList>
	class Application : public ApplicationBase
	{
		public:
			using ApplicationBase::ApplicationBase;
			Application(const Application&) = delete;
			Application(Application&&) = delete;
			~Application();

			Application& operator=(const Application&) = delete;
			Application& operator=(Application&&) = delete;

		private:
			Modules<ModuleList...> m_modules;
	};
}

#include <Nazara/Core/Application.inl>

#endif // NAZARA_CORE_APPLICATION_HPP
