// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Network/SocketPoller.hpp>

#if defined(NAZARA_PLATFORM_WINDOWS)
#include <Nazara/Network/Win32/SocketPollerImpl.hpp>
#elif defined(NAZARA_PLATFORM_LINUX)
#include <Nazara/Network/Linux/SocketPollerImpl.hpp>
#elif defined(NAZARA_PLATFORM_POSIX)
#include <Nazara/Network/Posix/SocketPollerImpl.hpp>
#else
#error Missing implementation: SocketPoller
#endif

#include <Nazara/Network/Debug.hpp>

namespace Nz
{
	/*!
	* \ingroup network
	* \class Nz::SocketPoller
	* \brief Network class allowing an application to wait on multiples sockets for them to become active (readable)
	*/

	/*!
	* \brief Constructs an empty SocketPoller object
	*/
	SocketPoller::SocketPoller() :
	m_impl(new SocketPollerImpl)
	{
	}

	/*!
	* \brief Destructs the SocketPoller
	*
	* \remark When the SocketPoller gets destroyed, all sockets are automatically unregistered from it.
	*/
	SocketPoller::~SocketPoller()
	{
		delete m_impl;
	}

	/*!
	* \brief Clears the SocketPoller
	*
	* This function reverts the SocketPoller to the initial state, unregistering every socket from the SocketPoller.
	*
	* \see Unregister
	*/
	void SocketPoller::Clear()
	{
		m_impl->Clear();
	}

	/*!
	* \brief Checks if a specific socket is ready to read data
	*
	* This function allows you to read the results of the last Wait operation and if a specific socket is ready.
	*
	* A socket in the ready state (with the exception of TcpServer) has incoming data and can be read without blocking.
	*
	* \remark When used on a TcpServer socket, this function returns true if the server is ready to accept a new client.
	* \remark You must call Wait before using this function in order to refresh the state.
	*
	* \param socket Reference to the socket to check
	*
	* \return True if the socket is available for reading without blocking, false otherwise
	*
	* \see Wait
	*/
	bool SocketPoller::IsReady(const AbstractSocket& socket) const
	{
		NazaraAssert(IsRegistered(socket), "Socket is not registered in the poller");

		return m_impl->IsReady(socket.GetNativeHandle());
	}

	/*!
	* \brief Checks if a specific socket is registered in the SocketPoller
	*
	* A registered socket is part of the SocketPoller and will be checked by the next Wait operations.
	*
	* \param socket Reference to the socket to check
	*
	* \return True if the socket is registered, false otherwise
	*
	* \see RegisterSocket
	* \see UnregisterSocket
	*/
	bool SocketPoller::IsRegistered(const AbstractSocket& socket) const
	{
		return m_impl->IsRegistered(socket.GetNativeHandle());
	}

	/*!
	* \brief Register a socket in the SocketPoller
	*
	* A registered socket is part of the SocketPoller and will be checked by the next Wait operations.
	*
	* The SocketPoller keeps a reference to the internal handle of registered socket, which should not be freed while it is registered in the SocketPooler.
	*
	* It is possible for this function to fail if too many sockets are registered in the SocketPoller, the maximum number of socket handled limit is OS-dependent.
	*
	* \remark It is an error to register a socket twice in the same SocketPoller.
	* \remark The socket should not be freed while it is registered in the SocketPooler.
	*
	* \param socket Reference to the socket to register
	*
	* \return True if the socket is registered, false otherwise
	*
	* \see IsRegistered
	* \see UnregisterSocket
	*/
	bool SocketPoller::RegisterSocket(AbstractSocket& socket)
	{
		NazaraAssert(!IsRegistered(socket), "This socket is already registered in this SocketPoller");

		return m_impl->RegisterSocket(socket.GetNativeHandle());
	}

	/*!
	* \brief Unregister a socket from the SocketPoller
	*
	* After calling UnregisterSocket, the socket is no longer part of the SocketPoller and thus, not taken into account by any further Wait call until registered again.
	*
	* This function must be called before destroying a socket part of the SocketPoller.
	*
	* \remark It is an error to try to unregister a non-registered socket from a SocketPoller.
	*
	* \param socket Reference to the socket to unregister
	*
	* \see IsRegistered
	* \see RegisterSocket
	*/
	void SocketPoller::UnregisterSocket(AbstractSocket& socket)
	{
		NazaraAssert(IsRegistered(socket), "This socket is not registered in this SocketPoller");

		return m_impl->UnregisterSocket(socket.GetNativeHandle());
	}

	/*!
	* \brief Wait until any registered socket switches to a ready state.
	*
	* Waits a specific/undetermined amount of time until at least one socket part of the SocketPoller becomes ready.
	* To query the ready state of the registered socket, use the IsReady function.
	*
	* \param msTimeout Maximum time to wait in milliseconds, 0 for infinity
	*
	* \return True if at least one socket registered to the poller is ready.
	*
	* \remark It is an error to try to unregister a non-registered socket from a SocketPoller.
	*
	* \see IsReady
	* \see RegisterSocket
	*/
	bool SocketPoller::Wait(UInt64 msTimeout)
	{
		SocketError error;

		int readySockets = m_impl->Wait(msTimeout, &error);
		if (error != SocketError_NoError)
		{
			NazaraError("SocketPoller encountered an error (code: 0x" + String::Number(error, 16) + ')');
			return false;
		}

		return readySockets > 0;
	}
}
