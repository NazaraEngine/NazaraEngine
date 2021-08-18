// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_MODULE_HPP
#define NAZARA_MODULE_HPP

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

#endif // NAZARA_MODULE_HPP
