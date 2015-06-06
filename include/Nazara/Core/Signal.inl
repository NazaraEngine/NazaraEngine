// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Error.hpp>
#include <utility>
#include <Nazara/Core/Debug.hpp>

template<typename... Args>
NzSignal<Args...>::NzSignal(NzSignal&& signal)
{
	operator=(std::move(signal));
}

template<typename... Args>
void NzSignal<Args...>::Clear()
{
	m_slots.clear();
}

template<typename... Args>
typename NzSignal<Args...>::Connection&& NzSignal<Args...>::Connect(const Callback& func)
{
	return Connect(std::move(Callback(func)));
}

template<typename... Args>
typename NzSignal<Args...>::Connection&& NzSignal<Args...>::Connect(Callback&& func)
{
	NazaraAssert(func, "Invalid function");

	auto tempPtr = std::make_shared<Slot>(this);
	tempPtr->callback = std::move(func);
	tempPtr->index = m_slots.size();

	m_slots.emplace_back(std::move(tempPtr));

	return Connection(m_slots.back());
}

template<typename... Args>
template<typename O>
typename NzSignal<Args...>::Connection&& NzSignal<Args...>::Connect(O& object, void (O::*method) (Args...))
{
    return Connect([&object, method] (Args&&... args)
    {
        return (object .* method) (std::forward<Args>(args)...);
    });
}

template<typename... Args>
template<typename O>
typename NzSignal<Args...>::Connection&& NzSignal<Args...>::Connect(O* object, void (O::*method)(Args...))
{
    return Connect([object, method] (Args&&... args)
    {
        return (object ->* method) (std::forward<Args>(args)...);
    });
}

template<typename... Args>
void NzSignal<Args...>::operator()(Args... args)
{
	for (const SlotPtr& slot : m_slots)
		slot->callback(args...);
}

template<typename... Args>
NzSignal<Args...>& NzSignal<Args...>::operator=(NzSignal&& signal)
{
	m_slots = std::move(signal.m_slots);

	// We need to update the signal pointer inside of each slot
	for (SlotPtr& slot : m_slots)
		slot->signal = this;

	return *this;
}

template<typename... Args>
void NzSignal<Args...>::Disconnect(const SlotPtr& slot)
{
	NazaraAssert(slot, "Invalid slot pointer");
	NazaraAssert(slot->index < m_slots.size(), "Invalid slot index");

	// "Swap this slot with the last one and pop" idiom
	// This will preserve slot indexes
	SlotPtr& current = m_slots[slot->index];

	std::swap(current, m_slots.back());
	m_slots.pop_back();

	current->index = slot->index; //< Update the moved slot index
}


template<typename... Args>
NzSignal<Args...>::Connection::Connection(const SlotPtr& slot) :
m_ptr(slot)
{
}

template<typename... Args>
void NzSignal<Args...>::Connection::Disconnect()
{
	if (SlotPtr ptr = m_ptr.lock())
		ptr->signal->Disconnect(ptr);
}

template<typename... Args>
bool NzSignal<Args...>::Connection::IsConnected() const
{
	return !m_ptr.expired();
}


template<typename... Args>
NzSignal<Args...>::ConnectionGuard::ConnectionGuard(const Connection& connection) :
m_connection(connection)
{
}

template<typename... Args>
NzSignal<Args...>::ConnectionGuard::ConnectionGuard(Connection&& connection) :
m_connection(std::move(connection))
{
}

template<typename... Args>
NzSignal<Args...>::ConnectionGuard::~ConnectionGuard()
{
	m_connection.Disconnect();
}

#include <Nazara/Core/DebugOff.hpp>
