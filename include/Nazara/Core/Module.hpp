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
	class Module
	{
		friend class Core;

		public:
			static T* Instance();

		protected:
			Module(std::string moduleName, T* pointer);
			~Module();

		private:
			struct NoLog {};

			Module(std::string moduleName, T* pointer, NoLog);

			void LogInit();

			std::string m_moduleName;
	};
}

#include <Nazara/Core/Module.inl>

#endif // NAZARA_MODULE_HPP
