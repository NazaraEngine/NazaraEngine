// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_OBJECTLIBRARY_HPP
#define NAZARA_OBJECTLIBRARY_HPP

#include <Nazara/Core/ObjectRef.hpp>
#include <Nazara/Core/RefCounted.hpp>
#include <string>
#include <unordered_map>

namespace Nz
{
	template<typename Type>
	class ObjectLibrary
	{
		friend Type;

		public:
			ObjectLibrary() = delete;
			~ObjectLibrary() = delete;

			static void Clear();

			static ObjectRef<Type> Get(const std::string& name);
			static bool Has(const std::string& name);

			static void Register(const std::string& name, ObjectRef<Type> object);
			static ObjectRef<Type> Query(const std::string& name);
			static void Unregister(const std::string& name);

		private:
			static bool Initialize();
			static void Uninitialize();

			using LibraryMap = std::unordered_map<std::string, ObjectRef<Type>>;
	};
}

#include <Nazara/Core/ObjectLibrary.inl>

#endif // NAZARA_OBJECTLIBRARY_HPP
