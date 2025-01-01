// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_CORE_HANDLEDOBJECT_HPP
#define NAZARA_CORE_HANDLEDOBJECT_HPP

#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/Export.hpp>
#include <NazaraUtils/Bitset.hpp>
#include <NazaraUtils/Signal.hpp>
#include <memory>
#include <vector>

namespace Nz
{
	namespace Detail
	{
		struct NAZARA_CORE_API HandleData
		{
			void* object;

			static std::shared_ptr<HandleData> GetEmptyObject();
		};
	}

	template<typename T> class ObjectHandle;

	template<typename T>
	class HandledObject
	{
		friend ObjectHandle<T>;

		public:
			HandledObject() = default;
			HandledObject(const HandledObject& object);
			HandledObject(HandledObject&& object) noexcept;
			~HandledObject();

			template<typename U = T>
			ObjectHandle<U> CreateHandle();

			std::shared_ptr<const Detail::HandleData> GetHandleData();

			HandledObject& operator=(const HandledObject& object);
			HandledObject& operator=(HandledObject&& object) noexcept;

			NazaraSignal(OnHandledObjectDestruction, HandledObject* /*emitter*/);

		protected:
			void UnregisterAllHandles() noexcept;

		private:
			void InitHandleData();

			std::shared_ptr<Detail::HandleData> m_handleData;
	};
}

#include <Nazara/Core/HandledObject.inl>

#endif // NAZARA_CORE_HANDLEDOBJECT_HPP
