// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Error.hpp>
#include <utility>
#include <Nazara/Core/Debug.hpp>

template<typename... Args>
NzSignal<Args...>::NzSignal() :
m_slotIterator(0)
{
}

template<typename... Args>
NzSignal<Args...>::NzSignal(NzSignal&& signal)
{
	operator=(std::move(signal));
}

template<typename... Args>
void NzSignal<Args...>::Clear()
{
	m_slots.clear();
	m_slotIterator = 0;
}

template<typename... Args>
typename NzSignal<Args...>::Connection NzSignal<Args...>::Connect(const Callback& func)
{
	return Connect(Callback(func));
}

template<typename... Args>
typename NzSignal<Args...>::Connection NzSignal<Args...>::Connect(Callback&& func)
{
	NazaraAssert(func, "Invalid function");

	// Since we're incrementing the slot vector size, we need to replace our iterator at the end
	// (Except when we are iterating on the signal)
	bool resetIt = (m_slotIterator >= m_slots.size());

	auto tempPtr = std::make_shared<Slot>(this);
	tempPtr->callback = std::move(func);
	tempPtr->index = m_slots.size();

	m_slots.emplace_back(std::move(tempPtr));

	if (resetIt)
		m_slotIterator = m_slots.size(); //< Replace the iterator to the end

	return Connection(m_slots.back());
}

template<typename... Args>
template<typename O>
typename NzSignal<Args...>::Connection NzSignal<Args...>::Connect(O& object, void (O::*method) (Args...))
{
    return Connect([&object, method] (Args&&... args)
    {
        return (object .* method) (std::forward<Args>(args)...);
    });
}

template<typename... Args>
template<typename O>
typename NzSignal<Args...>::Connection NzSignal<Args...>::Connect(O* object, void (O::*method)(Args...))
{
    return Connect([object, method] (Args&&... args)
    {
        return (object ->* method) (std::forward<Args>(args)...);
    });
}

template<typename... Args>
template<typename O>
typename NzSignal<Args...>::Connection NzSignal<Args...>::Connect(const O& object, void (O::*method) (Args...) const)
{
    return Connect([&object, method] (Args&&... args)
    {
        return (object .* method) (std::forward<Args>(args)...);
    });
}

template<typename... Args>
template<typename O>
typename NzSignal<Args...>::Connection NzSignal<Args...>::Connect(const O* object, void (O::*method)(Args...) const)
{
    return Connect([object, method] (Args&&... args)
    {
        return (object ->* method) (std::forward<Args>(args)...);
    });
}

template<typename... Args>
void NzSignal<Args...>::operator()(Args... args) const
{
	for (m_slotIterator = 0; m_slotIterator < m_slots.size(); ++m_slotIterator)
		m_slots[m_slotIterator]->callback(args...);
}

template<typename... Args>
NzSignal<Args...>& NzSignal<Args...>::operator=(NzSignal&& signal)
{
	m_slots = std::move(signal.m_slots);
	m_slotIterator = signal.m_slotIterator;

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

	// Can we safely "remove" this slot?
	if (m_slotIterator >= m_slots.size()-1 || slot->index > m_slotIterator)
	{
		// Yes we can
		SlotPtr& newSlot = m_slots[slot->index];
		newSlot = std::move(m_slots.back());
		newSlot->index = slot->index; //< Update the moved slot index before resizing (in case it's the last one)
	}
	else
	{
		// Nope, let's be tricky
		SlotPtr& current = m_slots[m_slotIterator];
		SlotPtr& newSlot = m_slots[slot->index];

		newSlot = std::move(current);
		newSlot->index = slot->index; //< Update the moved slot index

		current = std::move(m_slots.back());
		current->index = m_slotIterator; //< Update the moved slot index

		--m_slotIterator;
	}

	// Pop the last entry (from where we moved our slot)
	m_slots.pop_back();
}


template<typename... Args>
NzSignal<Args...>::Connection::Connection(const SlotPtr& slot) :
m_ptr(slot)
{
}

template<typename... Args>
template<typename... ConnectArgs>
void NzSignal<Args...>::Connection::Connect(BaseClass& signal, ConnectArgs&&... args)
{
	operator=(signal.Connect(std::forward<ConnectArgs>(args)...));
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

template<typename... Args>
template<typename... ConnectArgs>
void NzSignal<Args...>::ConnectionGuard::Connect(BaseClass& signal, ConnectArgs&&... args)
{
	m_connection.Disconnect();
	m_connection.Connect(signal, std::forward<ConnectArgs>(args)...);
}

template<typename... Args>
void NzSignal<Args...>::ConnectionGuard::Disconnect()
{
	m_connection.Disconnect();
}

template<typename... Args>
typename NzSignal<Args...>::Connection& NzSignal<Args...>::ConnectionGuard::GetConnection()
{
	return m_connection;
}

template<typename... Args>
bool NzSignal<Args...>::ConnectionGuard::IsConnected() const
{
	return m_connection.IsConnected();
}

template<typename... Args>
typename NzSignal<Args...>::ConnectionGuard& NzSignal<Args...>::ConnectionGuard::operator=(const Connection& connection)
{
	m_connection.Disconnect();
	m_connection = connection;

	return *this;
}

template<typename... Args>
typename NzSignal<Args...>::ConnectionGuard& NzSignal<Args...>::ConnectionGuard::operator=(Connection&& connection)
{
	m_connection.Disconnect();
	m_connection = std::move(connection);

	return *this;
}

template<typename... Args>
typename NzSignal<Args...>::ConnectionGuard& NzSignal<Args...>::ConnectionGuard::operator=(ConnectionGuard&& connection)
{
	m_connection.Disconnect();
	m_connection = std::move(connection.m_connection);

	return *this;
}

#include <Nazara/Core/DebugOff.hpp>
