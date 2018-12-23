// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequisites.hpp

#pragma once

#ifndef NAZARA_OBJECTHANDLER_HPP
#define NAZARA_OBJECTHANDLER_HPP

#include <Nazara/Core/Bitset.hpp>
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

			ObjectHandle<T> CreateHandle();

			HandledObject& operator=(const HandledObject& object);
			HandledObject& operator=(HandledObject&& object) noexcept;

		protected:
			void UnregisterAllHandles() noexcept;

		private:
			std::shared_ptr<const Detail::HandleData> GetHandleData();
			void InitHandleData();

			std::shared_ptr<Detail::HandleData> m_handleData;
	};
}

#include <Nazara/Core/HandledObject.inl>

#endif // NAZARA_OBJECTHANDLER_HPP
