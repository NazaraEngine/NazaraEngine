#pragma once

// Copyright (C) 2012 Maxime Griot
// This file is part of the "Nazara Engine - Network module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef TCP_SERVER_HPP
#define TCP_SERVER_HPP

#include <Nazara/Prerequesites.hpp>

#include <Nazara/Network/TcpConnection.hpp>
#include <Nazara/Network/IoServicePool.hpp>


class NAZARA_API NzTcpServer
    : public boost::enable_shared_from_this<NzTcpServer>, private NzNonCopyable
{
public:

    /**
     * Instantiate the NzTcpServer
     * @param pPort The port to start the NzTcpServer on
     */
    NzTcpServer(uint16_t pPort);
    NzTcpServer(const NzTcpServer&) = delete;

    ~NzTcpServer();
    /**
     * Start the NzTcpServer, will return on exit
     */
    void Start();
    /**
     * Authentication and sync loop
     */
    void Run();

    void RunOnce();
    /**
     * Stops the NzTcpServer, Run will return
     */
    void Stop();

    std::function<void(NzTcpConnection::pointer)>	OnConnection;
    std::function<void(uint32_t)>				OnUpdate;

protected:

    void Accept();
    void HandleAccept(NzTcpConnection::pointer pSession, const asio::error_code& pError);

    /**
     * Scale the thread count according to the runtime infos.
     */
    void Scale();

private:

    std::unique_ptr<asio::ip::tcp::acceptor>	mAcceptor;
    std::unique_ptr<NzIoServicePool>		    mIoPool;
    uint16_t								    mPort;
    uint32_t									mTotalElapsed;

    NzMutex							mGuard;

    bool mStarted;
};

#endif // TCP_SERVER_HPP
