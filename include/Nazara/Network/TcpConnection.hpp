// Copyright (C) 2012 Maxime Griot
// This file is part of the "Nazara Engine - Network module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef TCP_CONNECTION_HPP
#define TCP_CONNECTION_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/Mutex.hpp>
#include <Nazara/Network/Strategy.hpp>

#include <asio.hpp>
#include <boost/enable_shared_from_this.hpp>

#include <memory>
#include <list>

/**
 * @class TcpConnect
 * @brief Handles a single client as a session
 */
class NAZARA_API NzTcpConnection :
    public boost::enable_shared_from_this<NzTcpConnection>, private NzNonCopyable
{
public:

    typedef boost::shared_ptr<NzTcpConnection> pointer;
    typedef boost::weak_ptr  <NzTcpConnection> weak_pointer;

    NzTcpConnection(asio::io_service& pIoService);
    virtual ~NzTcpConnection();

    /**
     * Starts listening to the session
     */
    void Start();

    /**
     * Write a packet to the session, non blocking, undefined timing
     * @param pData The packet to send
     */
    void Write(const std::string& pData);

    /**
     * Close the TcpConnect, will trigger scalar destruction
     */
    void Close();

    /**
     * Get the session's socket
     * @return The socket
     */
    asio::ip::tcp::socket& GetSocket();

    /**
     * True if queue > 0, false otherwise
     * @return
     */
    bool HasPacket();

    /**
     * Too complicated to be explained xD
     */
    bool IsOffline();

    void SetStrategy(INzStrategy* pStrategy);

    void OnTimeout(const asio::error_code& e);

    void Connect(const std::string& pAddress, const std::string& pPort);

    std::function<void(const std::string&)> OnError;
    std::function<void(bool)>			    OnConnect;

protected:

    void async_write(const std::string& pPacket);
    void async_read();
    void handle_read_header(const asio::error_code& e, size_t transfered);
    void handle_read_data(const asio::error_code& e, size_t pBytes);
    void handle_resolve(const asio::error_code& ec, asio::ip::tcp::resolver::iterator pEndpointItor);
    void handle_connect(const asio::error_code& ec);

    void HandleRead	(const asio::error_code& pError);
    void HandleWrite(const asio::error_code& pError);
    void DoWrite(std::string data);

private:

    std::string mOutboundData; //< Outbound data buffer, kept for reference
    std::vector<char>  mInboundHeader;
    std::vector<char>  mInboundData;

    INzStrategy* mStrategy;
    NzMutex mPacketLock;

    std::list<std::string> mToSend;

    asio::deadline_timer mTimeout;

    asio::ip::tcp::socket	mSocket;
    asio::ip::tcp::resolver  mResolver;
};

#endif // TCP_CONNECTIO_HPP
