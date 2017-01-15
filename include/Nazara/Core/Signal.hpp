// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_SIGNAL_HPP
#define NAZARA_SIGNAL_HPP

#include <functional>
#include <memory>
#include <vector>

#define NazaraDetailSignal(Keyword, SignalName, ...) using SignalName ## Type = Nz::Signal<__VA_ARGS__>; \
                                                     Keyword SignalName ## Type SignalName

#define NazaraSignal(SignalName, ...) NazaraDetailSignal(mutable, SignalName, __VA_ARGS__)
#define NazaraStaticSignal(SignalName, ...) NazaraDetailSignal(static, SignalName, __VA_ARGS__)
#define NazaraStaticSignalImpl(Class, SignalName) Class :: SignalName ## Type Class :: SignalName

#define NazaraSlotType(Class, SignalName) Class::SignalName ## Type::ConnectionGuard
#define NazaraSlot(Class, SignalName, SlotName) NazaraSlotType(Class, SignalName) SlotName

namespace Nz
{
	template<typename... Args>
	class Signal
	{
		public:
			using Callback = std::function<void(Args...)>;
			class Connection;
			class ConnectionGuard;

			Signal();
			Signal(const Signal&) = delete;
			Signal(Signal&& signal) noexcept;
			~Signal() = default;

			void Clear();

			Connection Connect(const Callback& func);
			Connection Connect(Callback&& func);
			template<typename O> Connection Connect(O& object, void (O::*method)(Args...));
			template<typename O> Connection Connect(O* object, void (O::*method)(Args...));
			template<typename O> Connection Connect(const O& object, void (O::*method)(Args...) const);
			template<typename O> Connection Connect(const O* object, void (O::*method)(Args...) const);

			void operator()(Args... args) const;

			Signal& operator=(const Signal&) = delete;
			Signal& operator=(Signal&& signal) noexcept;

		private:
			struct Slot;

			using SlotPtr = std::shared_ptr<Slot>;
			using SlotList = std::vector<SlotPtr>;
			using SlotListIndex = typename SlotList::size_type;

			struct Slot
			{
				Slot(Signal* me) :
				signal(me)
				{
				}

				Callback callback;
				Signal* signal;
				SlotListIndex index;
			};

			void Disconnect(const SlotPtr& slot);

			SlotList m_slots;
			mutable SlotListIndex m_slotIterator;
	};

	template<typename... Args>
	class Signal<Args...>::Connection
	{
		using BaseClass = Signal<Args...>;
		friend BaseClass;

		public:
			Connection() = default;
			Connection(const Connection& connection) = default;
			Connection(Connection&& connection) = default;
			~Connection() = default;

			template<typename... ConnectArgs>
			void Connect(BaseClass& signal, ConnectArgs&&... args);
			void Disconnect();

			bool IsConnected() const;

			Connection& operator=(const Connection& connection) = default;
			Connection& operator=(Connection&& connection) = default;

		private:
			Connection(const SlotPtr& slot);

			std::weak_ptr<Slot> m_ptr;
	};

	template<typename... Args>
	class Signal<Args...>::ConnectionGuard
	{
		using BaseClass = Signal<Args...>;
		using Connection = typename BaseClass::Connection;

		public:
			ConnectionGuard() = default;
			ConnectionGuard(const Connection& connection);
			ConnectionGuard(const ConnectionGuard& connection) = delete;
			ConnectionGuard(Connection&& connection);
			ConnectionGuard(ConnectionGuard&& connection) = default;
			~ConnectionGuard();

			template<typename... ConnectArgs>
			void Connect(BaseClass& signal, ConnectArgs&&... args);
			void Disconnect();

			Connection& GetConnection();

			bool IsConnected() const;

			ConnectionGuard& operator=(const Connection& connection);
			ConnectionGuard& operator=(const ConnectionGuard& connection) = delete;
			ConnectionGuard& operator=(Connection&& connection);
			ConnectionGuard& operator=(ConnectionGuard&& connection);

		private:
			Connection m_connection;
	};
}

#include <Nazara/Core/Signal.inl>

#endif // NAZARA_SIGNAL_HPP
