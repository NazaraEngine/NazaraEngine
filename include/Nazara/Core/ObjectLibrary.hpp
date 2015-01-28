// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_OBJECTLIBRARY_HPP
#define NAZARA_OBJECTLIBRARY_HPP

#include <Nazara/Core/ObjectRef.hpp>
#include <Nazara/Core/RefCounted.hpp>
#include <Nazara/Core/String.hpp>
#include <unordered_map>

template<typename Type>
class NzObjectLibrary
{
	friend Type;

	public:
		NzObjectLibrary() = delete;
		~NzObjectLibrary() = delete;

		static NzObjectRef<Type> Get(const NzString& name);
		static bool Has(const NzString& name);

		static void Register(const NzString& name, NzObjectRef<Type> object);
		static NzObjectRef<Type> Query(const NzString& name);
		static void Unregister(const NzString& name);

	private:
		static bool Initialize();
		static void Uninitialize();

		using LibraryMap = std::unordered_map<NzString, NzObjectRef<Type>>;
};

#include <Nazara/Core/ObjectLibrary.inl>

#endif // NAZARA_OBJECTLIBRARY_HPP
