// Copyright (C) 2012 Maxime Griot
// This file is part of the "Nazara Engine - Network module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef IO_SERVICE_POOL_HPP
#define IO_SERVICE_POOL_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/Thread.hpp>

#include <asio.hpp>

class NAZARA_API NzIoServicePool
{
public:
    explicit NzIoServicePool(std::size_t pool_size);
    NzIoServicePool(const NzIoServicePool&) = delete;
    ~NzIoServicePool();

    void Run();

    void Stop();

    asio::io_service& GetIoService();

private:
    typedef std::shared_ptr<asio::io_service> io_service_ptr;
    typedef std::shared_ptr<asio::io_service::work> work_ptr;

    std::vector<io_service_ptr> m_IoServices;
    std::vector<std::shared_ptr<NzThread> > m_Threads;

    // Using std::atomic <bool> here (or boost equivalent) would be preferable
    std::vector<bool> m_Running;

    std::size_t m_NextIndex;
};

 #endif // IO_SERVICE_POOL_HPP
