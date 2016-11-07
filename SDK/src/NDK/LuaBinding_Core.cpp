// This file was automatically generated on 26 May 2014 at 01:05:31

#include <NDK/LuaBinding.hpp>
#include <Nazara/Core/MemoryHelper.hpp>
#include <NDK/LuaAPI.hpp>

namespace Ndk
{
	/*!
	* \brief Binds Core module to Lua
	*/

	void LuaBinding::BindCore()
	{
		/*********************************** Nz::Clock **********************************/
		clock.SetConstructor([](Nz::LuaInstance& lua, Nz::Clock* instance, std::size_t /*argumentCount*/)
		{
			int argIndex = 2;
			Nz::Int64 startingValue = lua.Check<Nz::Int64>(&argIndex, 0);
			bool paused = lua.Check<bool>(&argIndex, false);

			Nz::PlacementNew(instance, startingValue, paused);
			return true;
		});

		clock.BindMethod("GetMicroseconds", &Nz::Clock::GetMicroseconds);
		clock.BindMethod("GetMilliseconds", &Nz::Clock::GetMilliseconds);
		clock.BindMethod("GetSeconds", &Nz::Clock::GetSeconds);
		clock.BindMethod("IsPaused", &Nz::Clock::IsPaused);
		clock.BindMethod("Pause", &Nz::Clock::Pause);
		clock.BindMethod("Restart", &Nz::Clock::Restart);
		clock.BindMethod("Unpause", &Nz::Clock::Unpause);

		// Manual
		clock.BindMethod("__tostring", [] (Nz::LuaInstance& lua, Nz::Clock& instance, std::size_t /*argumentCount*/) -> int {
			Nz::StringStream ss("Clock(Elapsed: ");
			ss << instance.GetSeconds();
			ss << "s, Paused: ";
			ss << instance.IsPaused();
			ss << ')';

			lua.PushString(ss);
			return 1;
		});

		/********************************* Nz::Directory ********************************/
		directory.SetConstructor([](Nz::LuaInstance& lua, Nz::Directory* instance, std::size_t argumentCount)
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
		directory.BindMethod("IsOpen",  &Nz::Directory::IsOpen);
		directory.BindMethod("IsResultDirectory",  &Nz::Directory::IsResultDirectory);
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
		directory.BindMethod("__tostring", [] (Nz::LuaInstance& lua, Nz::Directory& instance, std::size_t /*argumentCount*/) -> int {
			Nz::StringStream ss("Directory(");
			ss << instance.GetPath();
			ss << ')';

			lua.PushString(ss);
			return 1;
		});

		/*********************************** Nz::Stream ***********************************/
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

		stream.BindMethod("Read", [] (Nz::LuaInstance& lua, Nz::Stream& instance, std::size_t /*argumentCount*/) -> int {
			int argIndex = 2;

			std::size_t length = lua.Check<std::size_t>(&argIndex);

			std::unique_ptr<char[]> buffer(new char[length]);
			std::size_t readLength = instance.Read(buffer.get(), length);

			lua.PushString(Nz::String(buffer.get(), readLength));
			return 1;
		});

		stream.BindMethod("Write", [] (Nz::LuaInstance& lua, Nz::Stream& instance, std::size_t /*argumentCount*/) -> int {
			int argIndex = 2;

			std::size_t bufferSize = 0;
			const char* buffer = lua.CheckString(argIndex, &bufferSize);

			if (instance.IsTextModeEnabled())
				lua.Push(instance.Write(Nz::String(buffer, bufferSize)));
			else
				lua.Push(instance.Write(buffer, bufferSize));
			return 1;
		});

		/*********************************** Nz::File ***********************************/
		file.Inherit(stream);

		file.SetConstructor([] (Nz::LuaInstance& lua, Nz::File* instance, std::size_t argumentCount)
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
		file.BindStaticMethod("ComputeHash", (Nz::ByteArray (*)(Nz::HashType, const Nz::String&)) &Nz::File::ComputeHash);
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
		file.BindMethod("Open", [] (Nz::LuaInstance& lua, Nz::File& instance, std::size_t argumentCount) -> int
		{
			std::size_t argCount = std::min<std::size_t>(argumentCount, 2U);

			int argIndex = 2;
			switch (argCount)
			{
				case 0:
				case 1:
					return lua.Push(instance.Open(lua.Check<Nz::UInt32>(&argIndex, Nz::OpenMode_NotOpen)));

				case 2:
				{
					Nz::String filePath = lua.Check<Nz::String>(&argIndex);
					Nz::UInt32 openMode = lua.Check<Nz::UInt32>(&argIndex, Nz::OpenMode_NotOpen);
					return lua.Push(instance.Open(filePath, openMode));
				}
			}

			lua.Error("No matching overload for method Open");
			return 0;
		});

		file.BindMethod("SetCursorPos", [] (Nz::LuaInstance& lua, Nz::File& instance, std::size_t argumentCount) -> int
		{
			std::size_t argCount = std::min<std::size_t>(argumentCount, 2U);

			int argIndex = 2;
			switch (argCount)
			{
				case 1:
					return lua.Push(instance.SetCursorPos(lua.Check<Nz::UInt64>(&argIndex)));

				case 2:
				{
					Nz::CursorPosition curPos = lua.Check<Nz::CursorPosition>(&argIndex);
					Nz::Int64 offset = lua.Check<Nz::Int64>(&argIndex);
					return lua.Push(instance.SetCursorPos(curPos, offset));
				}
			}

			lua.Error("No matching overload for method SetCursorPos");
			return 0;
		});

		file.BindMethod("__tostring", [] (Nz::LuaInstance& lua, Nz::File& instance, std::size_t /*argumentCount*/) -> int {
			Nz::StringStream ss("File(");
			if (instance.IsOpen())
				ss << "Path: " << instance.GetPath();

			ss << ')';

			lua.PushString(ss);
			return 1;
		});
	}

	/*!
	* \brief Registers the classes that will be used by the Lua instance
	*
	* \param instance Lua instance that will interact with the Core classes
	*/

	void LuaBinding::RegisterCore(Nz::LuaInstance& instance)
	{
		// Classes
		clock.Register(instance);
		directory.Register(instance);
		file.Register(instance);
		stream.Register(instance);

		// Enums

		// Nz::CursorPosition
		static_assert(Nz::CursorPosition_Max + 1 == 3, "Nz::CursorPosition has been updated but change was not reflected to Lua binding");
		instance.PushTable(0, 3);
		{
			instance.PushField("AtBegin", Nz::CursorPosition_AtBegin);
			instance.PushField("AtCurrent", Nz::CursorPosition_AtCurrent);
			instance.PushField("AtEnd", Nz::CursorPosition_AtEnd);
		}
		instance.SetGlobal("CursorPosition");

		// Nz::HashType
		static_assert(Nz::HashType_Max + 1 == 9, "Nz::HashType has been updated but change was not reflected to Lua binding");
		instance.PushTable(0, 9);
		{
			instance.PushField("CRC32", Nz::HashType_CRC32);
			instance.PushField("Fletcher16", Nz::HashType_Fletcher16);
			instance.PushField("MD5", Nz::HashType_MD5);
			instance.PushField("SHA1", Nz::HashType_SHA1);
			instance.PushField("SHA224", Nz::HashType_SHA224);
			instance.PushField("SHA256", Nz::HashType_SHA256);
			instance.PushField("SHA384", Nz::HashType_SHA384);
			instance.PushField("SHA512", Nz::HashType_SHA512);
			instance.PushField("Whirlpool", Nz::HashType_Whirlpool);
		}
		instance.SetGlobal("HashType");

		// Nz::OpenMode
		static_assert(Nz::OpenMode_Max + 1 == 2 * (64), "Nz::OpenModeFlags has been updated but change was not reflected to Lua binding");
		instance.PushTable(0, 8);
		{
			instance.PushField("Append", Nz::OpenMode_Append);
			instance.PushField("NotOpen", Nz::OpenMode_NotOpen);
			instance.PushField("Lock", Nz::OpenMode_Lock);
			instance.PushField("ReadOnly", Nz::OpenMode_ReadOnly);
			instance.PushField("ReadWrite", Nz::OpenMode_ReadWrite);
			instance.PushField("Text", Nz::OpenMode_Text);
			instance.PushField("Truncate", Nz::OpenMode_Truncate);
			instance.PushField("WriteOnly", Nz::OpenMode_WriteOnly);
		}
		instance.SetGlobal("OpenMode");
	}
}
