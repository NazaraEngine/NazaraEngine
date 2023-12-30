// Copyright (C) 2023 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_CORE_OBJECTLIBRARY_HPP
#define NAZARA_CORE_OBJECTLIBRARY_HPP

#include <NazaraUtils/StringHash.hpp>
#include <functional>
#include <memory>
#include <string>
#include <unordered_map>

namespace Nz
{
	template<typename Type>
	class ObjectLibrary
	{
		friend Type;

		public:
			ObjectLibrary() = default;
			~ObjectLibrary() = default;

			void Clear();

			std::shared_ptr<Type> Get(std::string_view name);
			bool Has(std::string_view name);

			void Register(std::string name, std::shared_ptr<Type> object);
			std::shared_ptr<Type> Query(std::string_view name);
			void Unregister(std::string_view name);

		private:
			std::unordered_map<std::string, std::shared_ptr<Type>, StringHash<>, std::equal_to<>> m_library;
	};
}

#include <Nazara/Core/ObjectLibrary.inl>

#endif // NAZARA_CORE_OBJECTLIBRARY_HPP
