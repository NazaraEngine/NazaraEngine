// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_OBJECTLIBRARY_HPP
#define NAZARA_OBJECTLIBRARY_HPP

#include <Nazara/Core/ObjectRef.hpp>
#include <Nazara/Core/RefCounted.hpp>
#include <Nazara/Core/String.hpp>
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

			static ObjectRef<Type> Get(const String& name);
			static bool Has(const String& name);

			static void Register(const String& name, ObjectRef<Type> object);
			static ObjectRef<Type> Query(const String& name);
			static void Unregister(const String& name);

		private:
			static bool Initialize();
			static void Uninitialize();

			using LibraryMap = std::unordered_map<String, ObjectRef<Type>>;
	};
}

#include <Nazara/Core/ObjectLibrary.inl>

#endif // NAZARA_OBJECTLIBRARY_HPP
