// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_SIGNAL_HPP
#define NAZARA_SIGNAL_HPP

#include <functional>
#include <memory>
#include <vector>

#define NazaraSignal(SignalName, ...) using SignalName ## Type = NzSignal<__VA_ARGS__>; \
                                       mutable SignalName ## Type SignalName

#define NazaraSlotType(Class, SignalName) Class::SignalName ## Type::ConnectionGuard
#define NazaraSlot(Class, SignalName, SlotName) NazaraSlotType(Class, SignalName) SlotName

#define NazaraConnect(Instance, SignalName, Callback) (Instance).SignalName.Connect(Callback)
#define NazaraConnectThis(Instance, SignalName, Callback) (Instance).SignalName.Connect(this, Callback)
#define NazaraDisconnect(SlotName) SlotName.GetConnection().Disconnect()


template<typename... Args>
class NzSignal
{
	public:
		using Callback = std::function<void(Args...)>;
		class Connection;
		class ConnectionGuard;

		NzSignal() = default;
		NzSignal(const NzSignal&) = delete;
		NzSignal(NzSignal&& signal);
		~NzSignal() = default;

		void Clear();

		Connection Connect(const Callback& func);
		Connection Connect(Callback&& func);
		template<typename O> Connection Connect(O& object, void (O::*method)(Args...));
		template<typename O> Connection Connect(O* object, void (O::*method)(Args...));

		void operator()(Args... args) const;

		NzSignal& operator=(const NzSignal&) = delete;
		NzSignal& operator=(NzSignal&& signal);

	private:
		struct Slot;

		using SlotPtr = std::shared_ptr<Slot>;
		using SlotList = std::vector<SlotPtr>;

		struct Slot
		{
			Slot(NzSignal* me) :
			signal(me)
			{
			}

			Callback callback;
			NzSignal* signal;
			typename SlotList::size_type index;
		};

		void Disconnect(const SlotPtr& slot);

		SlotList m_slots;
};

template<typename... Args>
class NzSignal<Args...>::Connection
{
	using BaseClass = NzSignal<Args...>;
	friend BaseClass;

	public:
		Connection() = default;
		Connection(const Connection& connection) = default;
		Connection(Connection&& connection) = default;
		~Connection() = default;

		void Disconnect();

		bool IsConnected() const;

		Connection& operator=(const Connection& connection) = default;
		Connection& operator=(Connection&& connection) = default;

	private:
		Connection(const SlotPtr& slot);

		std::weak_ptr<Slot> m_ptr;
};

template<typename... Args>
class NzSignal<Args...>::ConnectionGuard
{
	using BaseClass = NzSignal<Args...>;
	using Connection = BaseClass::Connection;

	public:
		ConnectionGuard() = default;
		ConnectionGuard(const Connection& connection);
		ConnectionGuard(const ConnectionGuard& connection) = delete;
		ConnectionGuard(Connection&& connection);
		ConnectionGuard(ConnectionGuard&& connection) = default;
		~ConnectionGuard();

		Connection& GetConnection();

		ConnectionGuard& operator=(const Connection& connection);
		ConnectionGuard& operator=(const ConnectionGuard& connection) = delete;
		ConnectionGuard& operator=(Connection&& connection);
		ConnectionGuard& operator=(ConnectionGuard&& connection);

	private:
		Connection m_connection;
};

#include <Nazara/Core/Signal.inl>

#endif // NAZARA_SIGNAL_HPP
