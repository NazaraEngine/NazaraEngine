// Copyright (C) 2012 Maxime Griot
// This file is part of the "Nazara Engine - Network module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef STRATEGY_HPP
#define STRATEGY_HPP

#include <Nazara/Prerequesites.hpp>
#include <string>

struct NAZARA_API INzStrategy
{
    virtual uint32_t GetHeaderSize() = 0;
    virtual uint32_t HandleHeader(const std::string&) = 0;
    virtual bool	 HandleContent(const std::string&) = 0;
    virtual bool	 HasPacket() = 0;
};

template <class T>
struct NzStrategy : public INzStrategy
{
public:

    virtual T		PopPacket() = 0;
};

#endif // STRATEGY_HPP
