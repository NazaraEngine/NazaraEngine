// This file was automatically generated on 26 May 2014 at 01:05:31

#include <NDK/Lua/LuaBinding_Core.hpp>
#include <Nazara/Core/MemoryHelper.hpp>
#include <NDK/LuaAPI.hpp>

namespace Ndk
{
	std::unique_ptr<LuaBinding_Base> LuaBinding_Base::BindCore(LuaBinding& binding)
	{
		return std::make_unique<LuaBinding_Core>(binding);
	}

	LuaBinding_Core::LuaBinding_Core(LuaBinding& binding) :
	LuaBinding_Base(binding)
	{
		/*********************************** Nz::Stream ***********************************/
		stream.Reset("Stream");
		{
			stream.BindMethod("EnableTextMode", &Nz::Stream::EnableTextMode);
			stream.BindMethod("Flush", &Nz::Stream::Flush);
			stream.BindMethod("GetCursorPos", &Nz::Stream::GetCursorPos);
			stream.BindMethod("GetDirectory", &Nz::Stream::GetDirectory);
			stream.BindMethod("GetPath", &Nz::Stream::GetPath);
			stream.BindMethod("GetOpenMode", &Nz::Stream::GetOpenMode);
			stream.BindMethod("GetStreamOptions", &Nz::Stream::GetStreamOptions);
			stream.BindMethod("GetSize", &Nz::Stream::GetSize);
			stream.BindMethod("ReadLine", &Nz::Stream::ReadLine, 0U);
			stream.BindMethod("IsReadable", &Nz::Stream::IsReadable);
			stream.BindMethod("IsSequential", &Nz::Stream::IsSequential);
			stream.BindMethod("IsTextModeEnabled", &Nz::Stream::IsTextModeEnabled);
			stream.BindMethod("IsWritable", &Nz::Stream::IsWritable);
			stream.BindMethod("SetCursorPos", &Nz::Stream::SetCursorPos);

			stream.BindMethod("Read", [] (Nz::LuaState& lua, Nz::Stream& stream, std::size_t /*argumentCount*/) -> int {
				int argIndex = 2;

				std::size_t length = lua.Check<std::size_t>(&argIndex);

				std::unique_ptr<char[]> buffer(new char[length]);
				std::size_t readLength = stream.Read(buffer.get(), length);

				lua.PushString(Nz::String(buffer.get(), readLength));
				return 1;
			});

			stream.BindMethod("Write", [] (Nz::LuaState& lua, Nz::Stream& stream, std::size_t /*argumentCount*/) -> int {
				int argIndex = 2;

				std::size_t bufferSize = 0;
				const char* buffer = lua.CheckString(argIndex, &bufferSize);

				if (stream.IsTextModeEnabled())
					lua.Push(stream.Write(Nz::String(buffer, bufferSize)));
				else
					lua.Push(stream.Write(buffer, bufferSize));
				return 1;
			});
		}

		/*********************************** Nz::Clock **********************************/
		clock.Reset("Clock");
		{
			clock.SetConstructor([] (Nz::LuaState& lua, Nz::Clock* instance, std::size_t argumentCount)
			{
				std::size_t argCount = std::min<std::size_t>(argumentCount, 2U);

				int argIndex = 2;
				switch (argCount)
				{
					case 0:
						Nz::PlacementNew(instance);
						return true;

					case 1:
					{
						Nz::Int64 startingValue = lua.Check<Nz::Int64>(&argIndex, 0);

						Nz::PlacementNew(instance, startingValue);
						return true;
					}

					case 2:
					{
						Nz::Int64 startingValue = lua.Check<Nz::Int64>(&argIndex, 0);
						bool paused = lua.Check<bool>(&argIndex, false);

						Nz::PlacementNew(instance, startingValue, paused);
						return true;
					}
				}

				lua.Error("No matching overload for Clock constructor");
				return false;
			});

			clock.BindMethod("GetMicroseconds", &Nz::Clock::GetMicroseconds);
			clock.BindMethod("GetMilliseconds", &Nz::Clock::GetMilliseconds);
			clock.BindMethod("GetSeconds", &Nz::Clock::GetSeconds);
			clock.BindMethod("IsPaused", &Nz::Clock::IsPaused);
			clock.BindMethod("Pause", &Nz::Clock::Pause);
			clock.BindMethod("Restart", &Nz::Clock::Restart);
			clock.BindMethod("Unpause", &Nz::Clock::Unpause);

			// Manual
			clock.BindMethod("__tostring", [] (Nz::LuaState& lua, Nz::Clock& clock, std::size_t /*argumentCount*/) -> int {
				Nz::StringStream ss("Clock(Elapsed: ");
				ss << clock.GetSeconds();
				ss << "s, Paused: ";
				ss << clock.IsPaused();
				ss << ')';

				lua.PushString(ss);
				return 1;
			});
		}

		/********************************* Nz::Directory ********************************/
		directory.Reset("Directory");
		{
			directory.SetConstructor([] (Nz::LuaState& lua, Nz::Directory* instance, std::size_t argumentCount)
			{
				std::size_t argCount = std::min<std::size_t>(argumentCount, 1U);

				int argIndex = 2;
				switch (argCount)
				{
					case 0:
						Nz::PlacementNew(instance);
						return true;

					case 1:
						Nz::PlacementNew(instance, lua.Check<Nz::String>(&argIndex));
						return true;
				}

				return false;
			});

			directory.BindMethod("Close", &Nz::Directory::Close);
			directory.BindMethod("Exists", &Nz::Directory::Exists);
			directory.BindMethod("GetPath", &Nz::Directory::GetPath);
			directory.BindMethod("GetPattern", &Nz::Directory::GetPattern);
			directory.BindMethod("GetResultName", &Nz::Directory::GetResultName);
			directory.BindMethod("GetResultPath", &Nz::Directory::GetResultPath);
			directory.BindMethod("GetResultSize", &Nz::Directory::GetResultSize);
			directory.BindMethod("IsOpen", &Nz::Directory::IsOpen);
			directory.BindMethod("IsResultDirectory", &Nz::Directory::IsResultDirectory);
			directory.BindMethod("NextResult", &Nz::Directory::NextResult, true);
			directory.BindMethod("Open", &Nz::Directory::Open);
			directory.BindMethod("SetPath", &Nz::Directory::SetPath);
			directory.BindMethod("SetPattern", &Nz::Directory::SetPattern);

			directory.BindStaticMethod("Copy", Nz::Directory::Copy);
			directory.BindStaticMethod("Create", Nz::Directory::Create);
			directory.BindStaticMethod("Exists", Nz::Directory::Exists);
			directory.BindStaticMethod("GetCurrent", Nz::Directory::GetCurrent);
			directory.BindStaticMethod("Remove", Nz::Directory::Remove);
			directory.BindStaticMethod("SetCurrent", Nz::Directory::SetCurrent);

			// Manual
			directory.BindMethod("__tostring", [] (Nz::LuaState& lua, Nz::Directory& dir, std::size_t /*argumentCount*/) -> int {
				Nz::StringStream ss("Directory(");
				ss << dir.GetPath();
				ss << ')';

				lua.PushString(ss);
				return 1;
			});
		}

		/*********************************** Nz::File ***********************************/
		file.Reset("File");
		{
			file.Inherit(stream);

			file.SetConstructor([] (Nz::LuaState& lua, Nz::File* instance, std::size_t argumentCount)
			{
				std::size_t argCount = std::min<std::size_t>(argumentCount, 1U);

				int argIndex = 2;
				switch (argCount)
				{
					case 0:
						Nz::PlacementNew(instance);
						return true;

					case 1:
					{
						Nz::String filePath = lua.Check<Nz::String>(&argIndex);

						Nz::PlacementNew(instance, filePath);
						return true;
					}

					case 2:
					{
						Nz::String filePath = lua.Check<Nz::String>(&argIndex);
						Nz::UInt32 openMode = lua.Check<Nz::UInt32>(&argIndex);

						Nz::PlacementNew(instance, filePath, openMode);
						return true;
					}
				}

				lua.Error("No matching overload for File constructor");
				return false;
			});

			file.BindMethod("Close", &Nz::File::Close);
			file.BindMethod("Copy", &Nz::File::Copy);
			file.BindMethod("Delete", &Nz::File::Delete);
			file.BindMethod("EndOfFile", &Nz::File::EndOfFile);
			file.BindMethod("Exists", &Nz::File::Exists);
			file.BindMethod("GetCreationTime", &Nz::File::GetCreationTime);
			file.BindMethod("GetFileName", &Nz::File::GetFileName);
			file.BindMethod("GetLastAccessTime", &Nz::File::GetLastAccessTime);
			file.BindMethod("GetLastWriteTime", &Nz::File::GetLastWriteTime);
			file.BindMethod("IsOpen", &Nz::File::IsOpen);
			file.BindMethod("Rename", &Nz::File::GetLastWriteTime);
			file.BindMethod("GetLastWriteTime", &Nz::File::GetLastWriteTime);
			file.BindMethod("SetFile", &Nz::File::GetLastWriteTime);

			file.BindStaticMethod("AbsolutePath", &Nz::File::AbsolutePath);
			file.BindStaticMethod("ComputeHash", (Nz::ByteArray(*)(Nz::HashType, const Nz::String&)) &Nz::File::ComputeHash);
			file.BindStaticMethod("Copy", &Nz::File::Copy);
			file.BindStaticMethod("Delete", &Nz::File::Delete);
			file.BindStaticMethod("Exists", &Nz::File::Exists);
			//fileClass.SetStaticMethod("GetCreationTime", &Nz::File::GetCreationTime);
			file.BindStaticMethod("GetDirectory", &Nz::File::GetDirectory);
			//fileClass.SetStaticMethod("GetLastAccessTime", &Nz::File::GetLastAccessTime);
			//fileClass.SetStaticMethod("GetLastWriteTime", &Nz::File::GetLastWriteTime);
			file.BindStaticMethod("GetSize", &Nz::File::GetSize);
			file.BindStaticMethod("IsAbsolute", &Nz::File::IsAbsolute);
			file.BindStaticMethod("NormalizePath", &Nz::File::NormalizePath);
			file.BindStaticMethod("NormalizeSeparators", &Nz::File::NormalizeSeparators);
			file.BindStaticMethod("Rename", &Nz::File::Rename);

			// Manual
			file.BindMethod("Open", [] (Nz::LuaState& lua, Nz::File& file, std::size_t argumentCount) -> int
			{
				std::size_t argCount = std::min<std::size_t>(argumentCount, 2U);

				int argIndex = 2;
				switch (argCount)
				{
					case 0:
					case 1:
						return lua.Push(file.Open(lua.Check<Nz::UInt32>(&argIndex, Nz::OpenMode_NotOpen)));

					case 2:
					{
						Nz::String filePath = lua.Check<Nz::String>(&argIndex);
						Nz::UInt32 openMode = lua.Check<Nz::UInt32>(&argIndex, Nz::OpenMode_NotOpen);
						return lua.Push(file.Open(filePath, openMode));
					}
				}

				lua.Error("No matching overload for method Open");
				return 0;
			});

			file.BindMethod("SetCursorPos", [] (Nz::LuaState& lua, Nz::File& file, std::size_t argumentCount) -> int
			{
				std::size_t argCount = std::min<std::size_t>(argumentCount, 2U);

				int argIndex = 2;
				switch (argCount)
				{
					case 1:
						return lua.Push(file.SetCursorPos(lua.Check<Nz::UInt64>(&argIndex)));

					case 2:
					{
						Nz::CursorPosition curPos = lua.Check<Nz::CursorPosition>(&argIndex);
						Nz::Int64 offset = lua.Check<Nz::Int64>(&argIndex);
						return lua.Push(file.SetCursorPos(curPos, offset));
					}
				}

				lua.Error("No matching overload for method SetCursorPos");
				return 0;
			});

			file.BindMethod("__tostring", [] (Nz::LuaState& lua, Nz::File& file, std::size_t /*argumentCount*/) -> int {
				Nz::StringStream ss("File(");
				if (file.IsOpen())
					ss << "Path: " << file.GetPath();

				ss << ')';

				lua.PushString(ss);
				return 1;
			});
		}
	}

	/*!
	* \brief Registers the classes that will be used by the Lua instance
	*
	* \param instance Lua instance that will interact with the Core classes
	*/
	void LuaBinding_Core::Register(Nz::LuaState& state)
	{
		// Classes
		clock.Register(state);
		directory.Register(state);
		file.Register(state);
		stream.Register(state);

		// Enums

		// Nz::CursorPosition
		static_assert(Nz::CursorPosition_Max + 1 == 3, "Nz::CursorPosition has been updated but change was not reflected to Lua binding");
		state.PushTable(0, 3);
		{
			state.PushField("AtBegin", Nz::CursorPosition_AtBegin);
			state.PushField("AtCurrent", Nz::CursorPosition_AtCurrent);
			state.PushField("AtEnd", Nz::CursorPosition_AtEnd);
		}
		state.SetGlobal("CursorPosition");

		// Nz::HashType
		static_assert(Nz::HashType_Max + 1 == 9, "Nz::HashType has been updated but change was not reflected to Lua binding");
		state.PushTable(0, 9);
		{
			state.PushField("CRC32", Nz::HashType_CRC32);
			state.PushField("Fletcher16", Nz::HashType_Fletcher16);
			state.PushField("MD5", Nz::HashType_MD5);
			state.PushField("SHA1", Nz::HashType_SHA1);
			state.PushField("SHA224", Nz::HashType_SHA224);
			state.PushField("SHA256", Nz::HashType_SHA256);
			state.PushField("SHA384", Nz::HashType_SHA384);
			state.PushField("SHA512", Nz::HashType_SHA512);
			state.PushField("Whirlpool", Nz::HashType_Whirlpool);
		}
		state.SetGlobal("HashType");

		// Nz::OpenMode
		static_assert(Nz::OpenMode_Max + 1 == 8, "Nz::OpenModeFlags has been updated but change was not reflected to Lua binding");
		state.PushTable(0, Nz::OpenMode_Max + 1);
		{
			state.PushField("Append",    Nz::OpenMode_Append);
			state.PushField("NotOpen",   Nz::OpenMode_NotOpen);
			state.PushField("Lock",      Nz::OpenMode_Lock);
			state.PushField("ReadOnly",  Nz::OpenMode_ReadOnly);
			state.PushField("ReadWrite", Nz::OpenMode_ReadWrite);
			state.PushField("Text",      Nz::OpenMode_Text);
			state.PushField("Truncate",  Nz::OpenMode_Truncate);
			state.PushField("WriteOnly", Nz::OpenMode_WriteOnly);
		}
		state.SetGlobal("OpenMode");
	}
}
