// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Assimp Plugin"
// For conditions of distribution and use, see copyright notice in Plugin.cpp

#include <CustomStream.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/ErrorFlags.hpp>
#include <Nazara/Core/File.hpp>
#include <Nazara/Utility/Mesh.hpp>
#include <assimp/cfileio.h>
#include <assimp/cimport.h>
#include <cstring>
#include <memory>

using namespace Nz;

void StreamFlush(aiFile* file)
{
	Stream* stream = reinterpret_cast<Stream*>(file->UserData);
	stream->Flush();
}

size_t StreamRead(aiFile* file, char* buffer, size_t size, size_t count)
{
	Stream* stream = reinterpret_cast<Stream*>(file->UserData);
	return stream->Read(buffer, size * count) / size;
}

aiReturn StreamSeek(aiFile* file, size_t offset, aiOrigin origin)
{
	Stream* stream = reinterpret_cast<Stream*>(file->UserData);
	switch (origin)
	{
		case aiOrigin_CUR:
			return (stream->SetCursorPos(stream->GetCursorPos() + offset)) ? aiReturn_SUCCESS : aiReturn_FAILURE;

		case aiOrigin_END:
			return (stream->SetCursorPos(stream->GetSize() - offset)) ? aiReturn_SUCCESS : aiReturn_FAILURE;

		case aiOrigin_SET:
			return (stream->SetCursorPos(offset)) ? aiReturn_SUCCESS : aiReturn_FAILURE;

		case _AI_ORIGIN_ENFORCE_ENUM_SIZE: // To prevent a warning
			break;
	}

	NazaraWarning("Unhandled aiOrigin enum (value: 0x" + std::string(origin, 16) + ')');
	return aiReturn_FAILURE;
}

size_t StreamSize(aiFile* file)
{
	Stream* stream = reinterpret_cast<Stream*>(file->UserData);
	return static_cast<std::size_t>(stream->GetSize());
}

size_t StreamTell(aiFile* file)
{
	Stream* stream = reinterpret_cast<Stream*>(file->UserData);
	return static_cast<std::size_t>(stream->GetCursorPos());
}

size_t StreamWrite(aiFile* file, const char* buffer, size_t size, size_t count)
{
	Stream* stream = reinterpret_cast<Stream*>(file->UserData);
	return stream->Write(buffer, size * count);
}

aiFile* StreamOpener(aiFileIO* fileIO, const char* filePath, const char* openMode)
{
	FileIOUserdata* fileIOUserdata = reinterpret_cast<FileIOUserdata*>(fileIO->UserData);

	bool isOriginalStream = (std::strcmp(filePath, fileIOUserdata->originalFilePath) == 0);
	if (!isOriginalStream && strstr(filePath, StreamPath) != nullptr)
		return nullptr;

	aiUserData stream;
	if (isOriginalStream)
	{
		stream = reinterpret_cast<aiUserData>(fileIOUserdata->originalStream);
		fileIOUserdata->originalStream->SetCursorPos(0);
	}
	else
	{
		ErrorFlags errFlags({}, ErrorMode::ThrowException);

		Result<OpenModeFlags, std::string> openModes = File::DecodeOpenMode(openMode);

		if (openModes.IsErr())
		{
			NazaraErrorFmt("{0} for file {1}", openModes.GetError(), filePath);
			return nullptr;
		}

		std::unique_ptr<File> file = std::make_unique<File>();
		if (!file->Open(filePath, openModes.GetValue()))
			return nullptr;

		stream = reinterpret_cast<char*>(file.release());
	}

	std::unique_ptr<aiFile> file = std::make_unique<aiFile>();
	file->FileSizeProc = StreamSize;
	file->FlushProc = StreamFlush;
	file->ReadProc = StreamRead;
	file->SeekProc = StreamSeek;
	file->TellProc = StreamTell;
	file->WriteProc = StreamWrite;
	file->UserData = stream;

	return file.release();
}

void StreamCloser(aiFileIO* fileIO, aiFile* file)
{
	FileIOUserdata* fileIOUserdata = reinterpret_cast<FileIOUserdata*>(fileIO->UserData);
	Stream* fileUserdata = reinterpret_cast<Stream*>(file->UserData);

	if (fileUserdata != fileIOUserdata->originalStream)
		delete reinterpret_cast<File*>(file->UserData);

	delete file;
}
