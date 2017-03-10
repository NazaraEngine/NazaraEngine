// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Signal.hpp>
#include <Nazara/Core/Error.hpp>
#include <utility>
#include <Nazara/Core/Debug.hpp>

namespace Nz
{
	/*!
	* \ingroup core
	* \class Nz::Signal
	* \brief Core class that represents a signal, a list of objects waiting for its message
	*/

	/*!
	* \brief Constructs a Signal object by default
	*/

	template<typename... Args>
	Signal<Args...>::Signal() :
	m_slotIterator(0)
	{
	}

	/*!
	* \brief Constructs a Signal object by move semantic
	*
	* \param signal Signal to move in this
	*/

	template<typename... Args>
	Signal<Args...>::Signal(Signal&& signal) noexcept
	{
		operator=(std::move(signal));
	}

	/*!
	* \brief Clears the list of actions attached to the signal
	*/

	template<typename... Args>
	void Signal<Args...>::Clear()
	{
		m_slots.clear();
		m_slotIterator = 0;
	}

	/*!
	* \brief Connects a function to the signal
	* \return Connection attached to the signal
	*
	* \param func Non-member function
	*/

	template<typename... Args>
	typename Signal<Args...>::Connection Signal<Args...>::Connect(const Callback& func)
	{
		return Connect(Callback(func));
	}

	/*!
	* \brief Connects a function to the signal
	* \return Connection attached to the signal
	*
	* \param func Non-member function
	*/

	template<typename... Args>
	typename Signal<Args...>::Connection Signal<Args...>::Connect(Callback&& func)
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

	/*!
	* \brief Connects a member function and its object to the signal
	* \return Connection attached to the signal
	*
	* \param object Object to send the message
	* \param method Member function
	*/

	template<typename... Args>
	template<typename O>
	typename Signal<Args...>::Connection Signal<Args...>::Connect(O& object, void (O::*method) (Args...))
	{
		return Connect([&object, method] (Args&&... args)
		{
			return (object .* method) (std::forward<Args>(args)...);
		});
	}

	/*!
	* \brief Connects a member function and its object to the signal
	* \return Connection attached to the signal
	*
	* \param object Object to send the message
	* \param method Member function
	*/

	template<typename... Args>
	template<typename O>
	typename Signal<Args...>::Connection Signal<Args...>::Connect(O* object, void (O::*method)(Args...))
	{
		return Connect([object, method] (Args&&... args)
		{
			return (object ->* method) (std::forward<Args>(args)...);
		});
	}

	/*!
	* \brief Connects a member function and its object to the signal
	* \return Connection attached to the signal
	*
	* \param object Object to send the message
	* \param method Member function
	*/

	template<typename... Args>
	template<typename O>
	typename Signal<Args...>::Connection Signal<Args...>::Connect(const O& object, void (O::*method) (Args...) const)
	{
		return Connect([&object, method] (Args&&... args)
		{
			return (object .* method) (std::forward<Args>(args)...);
		});
	}

	/*!
	* \brief Connects a member function and its object to the signal
	* \return Connection attached to the signal
	*
	* \param object Object to send the message
	* \param method Member function
	*/

	template<typename... Args>
	template<typename O>
	typename Signal<Args...>::Connection Signal<Args...>::Connect(const O* object, void (O::*method)(Args...) const)
	{
		return Connect([object, method] (Args&&... args)
		{
			return (object ->* method) (std::forward<Args>(args)...);
		});
	}

	/*!
	* \brief Applies the list of arguments to every callback functions
	*
	* \param args Arguments to send with the message
	*/

	template<typename... Args>
	void Signal<Args...>::operator()(Args... args) const
	{
		for (m_slotIterator = 0; m_slotIterator < m_slots.size(); ++m_slotIterator)
			m_slots[m_slotIterator]->callback(args...);
	}

	/*!
	* \brief Moves the signal into this
	* \return A reference to this
	*
	* \param signal Signal to move in this
	*/

	template<typename... Args>
	Signal<Args...>& Signal<Args...>::operator=(Signal&& signal) noexcept
	{
		m_slots = std::move(signal.m_slots);
		m_slotIterator = signal.m_slotIterator;

		// We need to update the signal pointer inside of each slot
		for (SlotPtr& slot : m_slots)
			slot->signal = this;

		return *this;
	}

	/*!
	* \brief Disconnects a listener from this signal
	*
	* \param slot Pointer to the ith listener of the signal
	*
	* \remark Produces a NazaraAssert if slot is invalid (nullptr)
	* \remark Produces a NazaraAssert if index of slot is invalid
	* \remark Produces a NazaraAssert if slot is not attached to this signal
	*/

	template<typename... Args>
	void Signal<Args...>::Disconnect(const SlotPtr& slot) noexcept
	{
		NazaraAssert(slot, "Invalid slot pointer");
		NazaraAssert(slot->index < m_slots.size(), "Invalid slot index");
		NazaraAssert(slot->signal == this, "Slot is not attached to this signal");

		// "Swap this slot with the last one and pop" idiom
		// This will preserve slot indexes

		// Can we safely "remove" this slot?
		if (m_slotIterator >= (m_slots.size() - 1) || slot->index > m_slotIterator)
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

	/*!
	* \class Nz::Signal::Connection
	* \brief Core class that represents a connection attached to a signal
	*/

	/*!
	* \brief Constructs a Signal::Connection object with a slot
	*
	* \param slot Slot of the listener
	*/

	template<typename... Args>
	Signal<Args...>::Connection::Connection(const SlotPtr& slot) :
	m_ptr(slot)
	{
	}

	/*!
	* \brief Connects to a signal with arguments
	*
	* \param signal New signal to listen
	* \param args Arguments for the signal
	*/

	template<typename... Args>
	template<typename... ConnectArgs>
	void Signal<Args...>::Connection::Connect(BaseClass& signal, ConnectArgs&&... args)
	{
		operator=(signal.Connect(std::forward<ConnectArgs>(args)...));
	}

	/*!
	* \brief Disconnects the connection from the signal
	*/

	template<typename... Args>
	void Signal<Args...>::Connection::Disconnect() noexcept
	{
		if (SlotPtr ptr = m_ptr.lock())
			ptr->signal->Disconnect(ptr);
	}

	/*!
	* \brief Checks whether the connection is still active with the signal
	* \return true if signal is still active
	*/

	template<typename... Args>
	bool Signal<Args...>::Connection::IsConnected() const
	{
		return !m_ptr.expired();
	}

	/*!
	* \class Nz::Signal::ConnectionGuard
	* \brief Core class that represents a RAII for a connection attached to a signal
	*/

	/*!
	* \brief Constructs a Signal::ConnectionGuard object with a connection
	*
	* \param connection Connection for the scope
	*/

	template<typename... Args>
	Signal<Args...>::ConnectionGuard::ConnectionGuard(const Connection& connection) :
	m_connection(connection)
	{
	}

	/*!
	* \brief Constructs a Signal::ConnectionGuard object with a connection by move semantic
	*
	* \param connection Connection for the scope
	*/

	template<typename... Args>
	Signal<Args...>::ConnectionGuard::ConnectionGuard(Connection&& connection) :
	m_connection(std::move(connection))
	{
	}

	/*!
	* \brief Destructs the object and disconnects the connection
	*/

	template<typename... Args>
	Signal<Args...>::ConnectionGuard::~ConnectionGuard()
	{
		m_connection.Disconnect();
	}

	/*!
	* \brief Connects to a signal with arguments
	*
	* \param signal New signal to listen
	* \param args Arguments for the signal
	*/

	template<typename... Args>
	template<typename... ConnectArgs>
	void Signal<Args...>::ConnectionGuard::Connect(BaseClass& signal, ConnectArgs&&... args)
	{
		m_connection.Disconnect();
		m_connection.Connect(signal, std::forward<ConnectArgs>(args)...);
	}

	/*!
	* \brief Disconnects the connection from the signal
	*/

	template<typename... Args>
	void Signal<Args...>::ConnectionGuard::Disconnect() noexcept
	{
		m_connection.Disconnect();
	}

	/*!
	* \brief Gets the connection attached to the signal
	* \return Connection of the signal
	*/

	template<typename... Args>
	typename Signal<Args...>::Connection& Signal<Args...>::ConnectionGuard::GetConnection()
	{
		return m_connection;
	}

	/*!
	* \brief Checks whether the connection is still active with the signal
	* \return true if signal is still active
	*/

	template<typename... Args>
	bool Signal<Args...>::ConnectionGuard::IsConnected() const
	{
		return m_connection.IsConnected();
	}

	/*!
	* \brief Assigns the connection into this
	* \return A reference to this
	*
	* \param connection Connection to assign into this
	*/

	template<typename... Args>
	typename Signal<Args...>::ConnectionGuard& Signal<Args...>::ConnectionGuard::operator=(const Connection& connection)
	{
		m_connection.Disconnect();
		m_connection = connection;

		return *this;
	}

	/*!
	* \brief Moves the Connection into this
	* \return A reference to this
	*
	* \param connection Connection to move in this
	*/

	template<typename... Args>
	typename Signal<Args...>::ConnectionGuard& Signal<Args...>::ConnectionGuard::operator=(Connection&& connection)
	{
		m_connection.Disconnect();
		m_connection = std::move(connection);

		return *this;
	}

	/*!
	* \brief Moves the ConnectionGuard into this
	* \return A reference to this
	*
	* \param connection ConnectionGuard to move in this
	*/

	template<typename... Args>
	typename Signal<Args...>::ConnectionGuard& Signal<Args...>::ConnectionGuard::operator=(ConnectionGuard&& connection) noexcept
	{
		m_connection.Disconnect();
		m_connection = std::move(connection.m_connection);

		return *this;
	}
}

#include <Nazara/Core/DebugOff.hpp>
