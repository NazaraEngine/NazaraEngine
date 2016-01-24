// This file was automatically generated on 26 May 2014 at 01:05:31

#include <NDK/LuaAPI.hpp>
#include <Nazara/Core.hpp>
#include <Nazara/Lua/LuaClass.hpp>

#include <Nazara/Core/Hash/MD5.hpp>

namespace Ndk
{
	void LuaAPI::Register_Core(Nz::LuaInstance& instance)
	{
		/*********************************** Nz::Clock **********************************/
		Nz::LuaClass<Nz::Clock> clockClass("Clock");

		clockClass.SetConstructor([](Nz::LuaInstance& lua) -> Nz::Clock*
		{
			int argIndex = 1;
			return new Nz::Clock(lua.Check<Nz::Int64>(&argIndex, 0), lua.Check<bool>(&argIndex, false));
		});

		clockClass.SetMethod("GetMicroseconds", &Nz::Clock::GetMicroseconds);
		clockClass.SetMethod("GetMilliseconds", &Nz::Clock::GetMilliseconds);
		clockClass.SetMethod("GetSeconds", &Nz::Clock::GetSeconds);
		clockClass.SetMethod("IsPaused", &Nz::Clock::IsPaused);
		clockClass.SetMethod("Pause", &Nz::Clock::Pause);
		clockClass.SetMethod("Restart", &Nz::Clock::Restart);
		clockClass.SetMethod("Unpause", &Nz::Clock::Unpause);

		// Manual
		clockClass.SetMethod("__tostring", [] (Nz::LuaInstance& lua, Nz::Clock& clock) -> int {
			Nz::StringStream stream("Clock(Elapsed: ");
			stream << clock.GetSeconds();
			stream << "s, Paused: ";
			stream << clock.IsPaused();
			stream << ')';

			lua.PushString(stream);
			return 1;
		});


		clockClass.Register(instance);

		/********************************* Nz::Directory ********************************/
		Nz::LuaClass<Nz::Directory> directoryClass("Directory");

		directoryClass.SetConstructor([](Nz::LuaInstance& lua) -> Nz::Directory*
		{
			unsigned int argCount = std::min(lua.GetStackTop(), 1U);

			int argIndex = 1;
			switch (argCount)
			{
				case 0:
					return new Nz::Directory;

				case 1:
					return new Nz::Directory(lua.Check<Nz::String>(&argIndex));
			}

			return nullptr;
		});

		directoryClass.SetMethod("Close", &Nz::Directory::Close);
		directoryClass.SetMethod("Exists", &Nz::Directory::Exists);
		directoryClass.SetMethod("GetPath", &Nz::Directory::GetPath);
		directoryClass.SetMethod("GetPattern", &Nz::Directory::GetPattern);
		directoryClass.SetMethod("GetResultName", &Nz::Directory::GetResultName);
		directoryClass.SetMethod("GetResultPath", &Nz::Directory::GetResultPath);
		directoryClass.SetMethod("GetResultSize", &Nz::Directory::GetResultSize);
		directoryClass.SetMethod("IsOpen",  &Nz::Directory::IsOpen);
		directoryClass.SetMethod("IsResultDirectory",  &Nz::Directory::IsResultDirectory);
		directoryClass.SetMethod("NextResult", &Nz::Directory::NextResult, true);
		directoryClass.SetMethod("Open", &Nz::Directory::Open);
		directoryClass.SetMethod("SetPath", &Nz::Directory::SetPath);
		directoryClass.SetMethod("SetPattern", &Nz::Directory::SetPattern);

		directoryClass.SetStaticMethod("Copy", Nz::Directory::Copy);
		directoryClass.SetStaticMethod("Create", Nz::Directory::Create);
		directoryClass.SetStaticMethod("Exists", Nz::Directory::Exists);
		directoryClass.SetStaticMethod("GetCurrent", Nz::Directory::GetCurrent);
		directoryClass.SetStaticMethod("Remove", Nz::Directory::Remove);
		directoryClass.SetStaticMethod("SetCurrent", Nz::Directory::SetCurrent);

		// Manual
		directoryClass.SetMethod("__tostring", [] (Nz::LuaInstance& lua, Nz::Directory& directory) -> int {
			Nz::StringStream stream("Directory(");
			stream << directory.GetPath();
			stream << ')';

			lua.PushString(stream);
			return 1;
		});


		directoryClass.Register(instance);

		/*********************************** Nz::Stream ***********************************/
		Nz::LuaClass<Nz::Stream> streamClass("Stream");

		streamClass.SetMethod("EnableTextMode", &Nz::Stream::EnableTextMode);
		streamClass.SetMethod("Flush", &Nz::Stream::Flush);
		streamClass.SetMethod("GetCursorPos", &Nz::Stream::GetCursorPos);
		streamClass.SetMethod("GetDirectory", &Nz::Stream::GetDirectory);
		streamClass.SetMethod("GetPath", &Nz::Stream::GetPath);
		streamClass.SetMethod("GetOpenMode", &Nz::Stream::GetOpenMode);
		streamClass.SetMethod("GetStreamOptions", &Nz::Stream::GetStreamOptions);
		streamClass.SetMethod("GetSize", &Nz::Stream::GetSize);
		streamClass.SetMethod("ReadLine", &Nz::Stream::ReadLine, 0U);
		streamClass.SetMethod("IsReadable", &Nz::Stream::IsReadable);
		streamClass.SetMethod("IsSequential", &Nz::Stream::IsSequential);
		streamClass.SetMethod("IsTextModeEnabled", &Nz::Stream::IsTextModeEnabled);
		streamClass.SetMethod("IsWritable", &Nz::Stream::IsWritable);
		streamClass.SetMethod("SetCursorPos", &Nz::Stream::SetCursorPos);

		streamClass.SetMethod("Read", [] (Nz::LuaInstance& lua, Nz::Stream& stream) -> int {
			int argIndex = 1;

			std::size_t length = lua.Check<std::size_t>(&argIndex);

			std::unique_ptr<char[]> buffer(new char[length]);
			std::size_t readLength = stream.Read(buffer.get(), length);

			lua.PushString(Nz::String(buffer.get(), readLength));
			return 1;
		});

		streamClass.SetMethod("Write", [] (Nz::LuaInstance& lua, Nz::Stream& stream) -> int {
			int argIndex = 1;

			std::size_t bufferSize = 0;
			const char* buffer = lua.CheckString(argIndex, &bufferSize);

			if (stream.IsTextModeEnabled())
				lua.Push(stream.Write(Nz::String(buffer, bufferSize)));
			else
				lua.Push(stream.Write(buffer, bufferSize));
			return 1;
		});

		streamClass.Register(instance);

		/*********************************** Nz::File ***********************************/
		Nz::LuaClass<Nz::File> fileClass("File");
		fileClass.Inherit(streamClass);

		fileClass.SetConstructor([](Nz::LuaInstance& lua) -> Nz::File*
		{
			unsigned int argCount = std::min(lua.GetStackTop(), 2U);

			int argIndex = 1;
			switch (argCount)
			{
				case 0:
					return new Nz::File;

				case 1:
					return new Nz::File(lua.Check<Nz::String>(&argIndex));

				case 2:
					return new Nz::File(lua.Check<Nz::String>(&argIndex), lua.Check<Nz::UInt32>(&argIndex));
			}

			return nullptr;
		});

		fileClass.SetMethod("Close", &Nz::File::Close);
		fileClass.SetMethod("Copy", &Nz::File::Copy);
		fileClass.SetMethod("Delete", &Nz::File::Delete);
		fileClass.SetMethod("EndOfFile", &Nz::File::EndOfFile);
		fileClass.SetMethod("Exists", &Nz::File::Exists);
		fileClass.SetMethod("GetCreationTime", &Nz::File::GetCreationTime);
		fileClass.SetMethod("GetFileName", &Nz::File::GetFileName);
		fileClass.SetMethod("GetLastAccessTime", &Nz::File::GetLastAccessTime);
		fileClass.SetMethod("GetLastWriteTime", &Nz::File::GetLastWriteTime);
		fileClass.SetMethod("IsOpen", &Nz::File::IsOpen);
		fileClass.SetMethod("Rename", &Nz::File::GetLastWriteTime);
		fileClass.SetMethod("GetLastWriteTime", &Nz::File::GetLastWriteTime);
		fileClass.SetMethod("SetFile", &Nz::File::GetLastWriteTime);

		fileClass.SetStaticMethod("AbsolutePath", &Nz::File::AbsolutePath);
		fileClass.SetStaticMethod("ComputeHash", (Nz::ByteArray (*)(Nz::HashType, const Nz::String&)) &Nz::File::ComputeHash);
		fileClass.SetStaticMethod("Copy", &Nz::File::Copy);
		fileClass.SetStaticMethod("Delete", &Nz::File::Delete);
		fileClass.SetStaticMethod("Exists", &Nz::File::Exists);
		//fileClass.SetStaticMethod("GetCreationTime", &Nz::File::GetCreationTime);
		fileClass.SetStaticMethod("GetDirectory", &Nz::File::GetDirectory);
		//fileClass.SetStaticMethod("GetLastAccessTime", &Nz::File::GetLastAccessTime);
		//fileClass.SetStaticMethod("GetLastWriteTime", &Nz::File::GetLastWriteTime);
		fileClass.SetStaticMethod("GetSize", &Nz::File::GetSize);
		fileClass.SetStaticMethod("IsAbsolute", &Nz::File::IsAbsolute);
		fileClass.SetStaticMethod("NormalizePath", &Nz::File::NormalizePath);
		fileClass.SetStaticMethod("NormalizeSeparators", &Nz::File::NormalizeSeparators);
		fileClass.SetStaticMethod("Rename", &Nz::File::Rename);

		// Manual
		fileClass.SetMethod("Open", [] (Nz::LuaInstance& lua, Nz::File& file) -> int
		{
			unsigned int argCount = std::min(lua.GetStackTop(), 2U);

			int argIndex = 1;
			switch (argCount)
			{
				case 0:
				case 1:
					return lua.Push(file.Open(lua.Check<Nz::UInt32>(&argIndex, Nz::OpenMode_NotOpen)));

				case 2:
					return lua.Push(file.Open(lua.Check<Nz::String>(&argIndex), lua.Check<Nz::UInt32>(&argIndex, Nz::OpenMode_NotOpen)));
			}

			lua.Error("No matching overload for method Open");
			return 0;
		});

		fileClass.SetMethod("SetCursorPos", [] (Nz::LuaInstance& lua, Nz::File& file) -> int
		{
			unsigned int argCount = std::min(lua.GetStackTop(), 2U);

			int argIndex = 1;
			switch (argCount)
			{
				case 1:
					return lua.Push(file.SetCursorPos(lua.Check<Nz::UInt64>(&argIndex)));

				case 2:
					return lua.Push(file.SetCursorPos(lua.Check<Nz::CursorPosition>(&argIndex), lua.Check<Nz::Int64>(&argIndex)));
			}

			lua.Error("No matching overload for method SetCursorPos");
			return 0;
		});

		fileClass.SetMethod("__tostring", [] (Nz::LuaInstance& lua, Nz::File& file) -> int {
			Nz::StringStream stream("File(");
			if (file.IsOpen())
				stream << "Path: " << file.GetPath();

			stream << ')';

			lua.PushString(stream);
			return 1;
		});

		fileClass.Register(instance);

		// Enums

		// Nz::CursorPosition
		static_assert(Nz::CursorPosition_Max + 1 == 3, "Nz::CursorPosition has been updated but change was not reflected to Lua binding");
		instance.PushTable(0, 3);
		{
			instance.SetField("AtBegin",   Nz::CursorPosition_AtBegin);
			instance.SetField("AtCurrent", Nz::CursorPosition_AtCurrent);
			instance.SetField("AtEnd",     Nz::CursorPosition_AtEnd);
		}
		instance.SetGlobal("CursorPosition");

		// Nz::HashType
		static_assert(Nz::HashType_Max + 1 == 9, "Nz::HashType has been updated but change was not reflected to Lua binding");
		instance.PushTable(0, 9);
		{
			instance.SetField("CRC32", Nz::HashType_CRC32);
			instance.SetField("Fletcher16", Nz::HashType_Fletcher16);
			instance.SetField("MD5", Nz::HashType_MD5);
			instance.SetField("SHA1", Nz::HashType_SHA1);
			instance.SetField("SHA224", Nz::HashType_SHA224);
			instance.SetField("SHA256", Nz::HashType_SHA256);
			instance.SetField("SHA384", Nz::HashType_SHA384);
			instance.SetField("SHA512", Nz::HashType_SHA512);
			instance.SetField("Whirlpool", Nz::HashType_Whirlpool);
		}
		instance.SetGlobal("HashType");

		// Nz::OpenMode
		static_assert(Nz::OpenMode_Max + 1 == 2 * (64), "Nz::OpenModeFlags has been updated but change was not reflected to Lua binding");
		instance.PushTable(0, 8);
		{
			instance.SetField("Append",    Nz::OpenMode_Append);
			instance.SetField("NotOpen",   Nz::OpenMode_NotOpen);
			instance.SetField("Lock",      Nz::OpenMode_Lock);
			instance.SetField("ReadOnly",  Nz::OpenMode_ReadOnly);
			instance.SetField("ReadWrite", Nz::OpenMode_ReadWrite);
			instance.SetField("Text",      Nz::OpenMode_Text);
			instance.SetField("Truncate",  Nz::OpenMode_Truncate);
			instance.SetField("WriteOnly", Nz::OpenMode_WriteOnly);
		}
		instance.SetGlobal("OpenMode");
	}
}
