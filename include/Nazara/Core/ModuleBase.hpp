// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_CORE_MODULEBASE_HPP
#define NAZARA_CORE_MODULEBASE_HPP

#include <Nazara/Prerequisites.hpp>
#include <string>

namespace Nz
{
	template<typename T>
	class ModuleBase
	{
		friend class Core;

		public:
			ModuleBase(const ModuleBase&) = delete;
			ModuleBase(ModuleBase&&) = delete;

			ModuleBase& operator=(const ModuleBase&) = delete;
			ModuleBase& operator=(ModuleBase&&) = delete;

			static T* Instance();

		protected:
			ModuleBase(std::string moduleName, T* pointer);
			~ModuleBase();

		private:
			struct NoLog {};

			ModuleBase(std::string moduleName, T* pointer, NoLog);

			void LogInit();
			void LogUninit();

			std::string m_moduleName;
	};
}

#include <Nazara/Core/ModuleBase.inl>

#endif // NAZARA_CORE_MODULEBASE_HPP
