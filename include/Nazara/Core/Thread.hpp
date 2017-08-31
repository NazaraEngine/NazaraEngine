// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_THREAD_HPP
#define NAZARA_THREAD_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/Functor.hpp>
#include <iosfwd>

namespace Nz
{
	class ThreadImpl;

	class NAZARA_CORE_API Thread
	{
		public:
			class Id;

			Thread();
			template<typename F> Thread(F function);
			template<typename F, typename... Args> Thread(F function, Args&&... args);
			template<typename C> Thread(void (C::*function)(), C* object);
			Thread(const Thread&) = delete;
			Thread(Thread&& other) noexcept;
			~Thread();

			void Detach();
			Id GetId() const;
			bool IsJoinable() const;
			void Join();
			void SetName(const String& name);

			Thread& operator=(const Thread&) = delete;
			Thread& operator=(Thread&& thread);

			static unsigned int HardwareConcurrency();
			static void SetCurrentThreadName(const String& name);
			static void Sleep(UInt32 milliseconds);

		private:
			void CreateImpl(Functor* functor);

			ThreadImpl* m_impl;
	};

	class NAZARA_CORE_API Thread::Id
	{
		friend Thread;

		public:
			NAZARA_CORE_API friend bool operator==(const Id& lhs, const Id& rhs);
			NAZARA_CORE_API friend bool operator!=(const Id& lhs, const Id& rhs);
			NAZARA_CORE_API friend bool operator<(const Id& lhs, const Id& rhs);
			NAZARA_CORE_API friend bool operator<=(const Id& lhs, const Id& rhs);
			NAZARA_CORE_API friend bool operator>(const Id& lhs, const Id& rhs);
			NAZARA_CORE_API friend bool operator>=(const Id& lhs, const Id& rhs);

			NAZARA_CORE_API friend std::ostream& operator<<(std::ostream& o, const Id& id);

		private:
			Id(ThreadImpl* thread);

			ThreadImpl* m_id = nullptr;
	};
}

#include <Nazara/Core/Thread.inl>

#endif // NAZARA_THREAD_HPP
