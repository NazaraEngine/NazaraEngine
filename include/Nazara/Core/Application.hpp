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
			template<typename... ModuleConfig> Application(ModuleConfig&&... configs);
			template<typename... ModuleConfig> Application(int argc, char** argv, ModuleConfig&&... configs);
			template<typename... ModuleConfig> Application(int argc, const Pointer<const char>* argv, ModuleConfig&&... configs);
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
