// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Assimp Plugin"
// For conditions of distribution and use, see copyright notice in Plugin.cpp

#pragma once

#ifndef NAZARA_ASSIMP_CUSTOM_STREAM_HPP
#define NAZARA_ASSIMP_CUSTOM_STREAM_HPP

#include <Nazara/Core/Stream.hpp>
#include <assimp/cfileio.h>

constexpr const char StreamPath[] = "<Nazara:Stream>";

void StreamFlush(aiFile* file);
size_t StreamRead(aiFile* file, char* buffer, size_t size, size_t count);
aiReturn StreamSeek(aiFile* file, size_t offset, aiOrigin origin);
size_t StreamSize(aiFile* file);
size_t StreamTell(aiFile* file);
size_t StreamWrite(aiFile* file, const char* buffer, size_t size, size_t count);

struct FileIOUserdata
{
    Nz::Stream* originalStream;
    const char* originalFilePath;
};

aiFile* StreamOpener(aiFileIO* fileIO, const char* filePath, const char* openMode);
void StreamCloser(aiFileIO* fileIO, aiFile* file);

#endif // NAZARA_ASSIMP_CUSTOM_STREAM_HPP