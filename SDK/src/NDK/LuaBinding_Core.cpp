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
		clockClass.SetConstructor([](Nz::LuaInstance& lua, Nz::Clock* clock)
		{
			int argIndex = 1;
			Nz::Int64 startingValue = lua.Check<Nz::Int64>(&argIndex, 0);
			bool paused = lua.Check<bool>(&argIndex, false);

			Nz::PlacementNew(clock, startingValue, paused);
			return true;
		});

		clockClass.BindMethod("GetMicroseconds", &Nz::Clock::GetMicroseconds);
		clockClass.BindMethod("GetMilliseconds", &Nz::Clock::GetMilliseconds);
		clockClass.BindMethod("GetSeconds", &Nz::Clock::GetSeconds);
		clockClass.BindMethod("IsPaused", &Nz::Clock::IsPaused);
		clockClass.BindMethod("Pause", &Nz::Clock::Pause);
		clockClass.BindMethod("Restart", &Nz::Clock::Restart);
		clockClass.BindMethod("Unpause", &Nz::Clock::Unpause);

		// Manual
		clockClass.BindMethod("__tostring", [] (Nz::LuaInstance& lua, Nz::Clock& clock) -> int {
			Nz::StringStream stream("Clock(Elapsed: ");
			stream << clock.GetSeconds();
			stream << "s, Paused: ";
			stream << clock.IsPaused();
			stream << ')';

			lua.PushString(stream);
			return 1;
		});

		/********************************* Nz::Directory ********************************/
		directoryClass.SetConstructor([](Nz::LuaInstance& lua, Nz::Directory* directory)
		{
			unsigned int argCount = std::min(lua.GetStackTop(), 1U);

			int argIndex = 1;
			switch (argCount)
			{
				case 0:
					Nz::PlacementNew(directory);
					return true;

				case 1:
					Nz::PlacementNew(directory, lua.Check<Nz::String>(&argIndex));
					return true;
			}

			return false;
		});

		directoryClass.BindMethod("Close", &Nz::Directory::Close);
		directoryClass.BindMethod("Exists", &Nz::Directory::Exists);
		directoryClass.BindMethod("GetPath", &Nz::Directory::GetPath);
		directoryClass.BindMethod("GetPattern", &Nz::Directory::GetPattern);
		directoryClass.BindMethod("GetResultName", &Nz::Directory::GetResultName);
		directoryClass.BindMethod("GetResultPath", &Nz::Directory::GetResultPath);
		directoryClass.BindMethod("GetResultSize", &Nz::Directory::GetResultSize);
		directoryClass.BindMethod("IsOpen",  &Nz::Directory::IsOpen);
		directoryClass.BindMethod("IsResultDirectory",  &Nz::Directory::IsResultDirectory);
		directoryClass.BindMethod("NextResult", &Nz::Directory::NextResult, true);
		directoryClass.BindMethod("Open", &Nz::Directory::Open);
		directoryClass.BindMethod("SetPath", &Nz::Directory::SetPath);
		directoryClass.BindMethod("SetPattern", &Nz::Directory::SetPattern);

		directoryClass.BindStaticMethod("Copy", Nz::Directory::Copy);
		directoryClass.BindStaticMethod("Create", Nz::Directory::Create);
		directoryClass.BindStaticMethod("Exists", Nz::Directory::Exists);
		directoryClass.BindStaticMethod("GetCurrent", Nz::Directory::GetCurrent);
		directoryClass.BindStaticMethod("Remove", Nz::Directory::Remove);
		directoryClass.BindStaticMethod("SetCurrent", Nz::Directory::SetCurrent);

		// Manual
		directoryClass.BindMethod("__tostring", [] (Nz::LuaInstance& lua, Nz::Directory& directory) -> int {
			Nz::StringStream stream("Directory(");
			stream << directory.GetPath();
			stream << ')';

			lua.PushString(stream);
			return 1;
		});

		/*********************************** Nz::Stream ***********************************/
		streamClass.BindMethod("EnableTextMode", &Nz::Stream::EnableTextMode);
		streamClass.BindMethod("Flush", &Nz::Stream::Flush);
		streamClass.BindMethod("GetCursorPos", &Nz::Stream::GetCursorPos);
		streamClass.BindMethod("GetDirectory", &Nz::Stream::GetDirectory);
		streamClass.BindMethod("GetPath", &Nz::Stream::GetPath);
		streamClass.BindMethod("GetOpenMode", &Nz::Stream::GetOpenMode);
		streamClass.BindMethod("GetStreamOptions", &Nz::Stream::GetStreamOptions);
		streamClass.BindMethod("GetSize", &Nz::Stream::GetSize);
		streamClass.BindMethod("ReadLine", &Nz::Stream::ReadLine, 0U);
		streamClass.BindMethod("IsReadable", &Nz::Stream::IsReadable);
		streamClass.BindMethod("IsSequential", &Nz::Stream::IsSequential);
		streamClass.BindMethod("IsTextModeEnabled", &Nz::Stream::IsTextModeEnabled);
		streamClass.BindMethod("IsWritable", &Nz::Stream::IsWritable);
		streamClass.BindMethod("SetCursorPos", &Nz::Stream::SetCursorPos);

		streamClass.BindMethod("Read", [] (Nz::LuaInstance& lua, Nz::Stream& stream) -> int {
			int argIndex = 1;

			std::size_t length = lua.Check<std::size_t>(&argIndex);

			std::unique_ptr<char[]> buffer(new char[length]);
			std::size_t readLength = stream.Read(buffer.get(), length);

			lua.PushString(Nz::String(buffer.get(), readLength));
			return 1;
		});

		streamClass.BindMethod("Write", [] (Nz::LuaInstance& lua, Nz::Stream& stream) -> int {
			int argIndex = 1;

			std::size_t bufferSize = 0;
			const char* buffer = lua.CheckString(argIndex, &bufferSize);

			if (stream.IsTextModeEnabled())
				lua.Push(stream.Write(Nz::String(buffer, bufferSize)));
			else
				lua.Push(stream.Write(buffer, bufferSize));
			return 1;
		});

		/*********************************** Nz::File ***********************************/
		fileClass.Inherit(streamClass);

		fileClass.SetConstructor([](Nz::LuaInstance& lua, Nz::File* file)
		{
			unsigned int argCount = std::min(lua.GetStackTop(), 2U);

			int argIndex = 1;
			switch (argCount)
			{
				case 0:
					Nz::PlacementNew(file);
					return true;

				case 1:
				{
					Nz::String filePath = lua.Check<Nz::String>(&argIndex);

					Nz::PlacementNew(file, filePath);
					return true;
				}

				case 2:
				{
					Nz::String filePath = lua.Check<Nz::String>(&argIndex);
					Nz::UInt32 openMode = lua.Check<Nz::UInt32>(&argIndex);

					Nz::PlacementNew(file, filePath, openMode);
					return true;
				}
			}

			lua.Error("No matching overload for File constructor");
			return false;
		});

		fileClass.BindMethod("Close", &Nz::File::Close);
		fileClass.BindMethod("Copy", &Nz::File::Copy);
		fileClass.BindMethod("Delete", &Nz::File::Delete);
		fileClass.BindMethod("EndOfFile", &Nz::File::EndOfFile);
		fileClass.BindMethod("Exists", &Nz::File::Exists);
		fileClass.BindMethod("GetCreationTime", &Nz::File::GetCreationTime);
		fileClass.BindMethod("GetFileName", &Nz::File::GetFileName);
		fileClass.BindMethod("GetLastAccessTime", &Nz::File::GetLastAccessTime);
		fileClass.BindMethod("GetLastWriteTime", &Nz::File::GetLastWriteTime);
		fileClass.BindMethod("IsOpen", &Nz::File::IsOpen);
		fileClass.BindMethod("Rename", &Nz::File::GetLastWriteTime);
		fileClass.BindMethod("GetLastWriteTime", &Nz::File::GetLastWriteTime);
		fileClass.BindMethod("SetFile", &Nz::File::GetLastWriteTime);

		fileClass.BindStaticMethod("AbsolutePath", &Nz::File::AbsolutePath);
		fileClass.BindStaticMethod("ComputeHash", (Nz::ByteArray (*)(Nz::HashType, const Nz::String&)) &Nz::File::ComputeHash);
		fileClass.BindStaticMethod("Copy", &Nz::File::Copy);
		fileClass.BindStaticMethod("Delete", &Nz::File::Delete);
		fileClass.BindStaticMethod("Exists", &Nz::File::Exists);
		//fileClass.SetStaticMethod("GetCreationTime", &Nz::File::GetCreationTime);
		fileClass.BindStaticMethod("GetDirectory", &Nz::File::GetDirectory);
		//fileClass.SetStaticMethod("GetLastAccessTime", &Nz::File::GetLastAccessTime);
		//fileClass.SetStaticMethod("GetLastWriteTime", &Nz::File::GetLastWriteTime);
		fileClass.BindStaticMethod("GetSize", &Nz::File::GetSize);
		fileClass.BindStaticMethod("IsAbsolute", &Nz::File::IsAbsolute);
		fileClass.BindStaticMethod("NormalizePath", &Nz::File::NormalizePath);
		fileClass.BindStaticMethod("NormalizeSeparators", &Nz::File::NormalizeSeparators);
		fileClass.BindStaticMethod("Rename", &Nz::File::Rename);

		// Manual
		fileClass.BindMethod("Open", [] (Nz::LuaInstance& lua, Nz::File& file) -> int
		{
			unsigned int argCount = std::min(lua.GetStackTop(), 2U);

			int argIndex = 1;
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

		fileClass.BindMethod("SetCursorPos", [] (Nz::LuaInstance& lua, Nz::File& file) -> int
		{
			unsigned int argCount = std::min(lua.GetStackTop(), 2U);

			int argIndex = 1;
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

		fileClass.BindMethod("__tostring", [] (Nz::LuaInstance& lua, Nz::File& file) -> int {
			Nz::StringStream stream("File(");
			if (file.IsOpen())
				stream << "Path: " << file.GetPath();

			stream << ')';

			lua.PushString(stream);
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
		clockClass.Register(instance);
		directoryClass.Register(instance);
		fileClass.Register(instance);
		streamClass.Register(instance);

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
