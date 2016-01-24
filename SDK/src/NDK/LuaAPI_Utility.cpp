// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp


#include <NDK/LuaAPI.hpp>
#include <Nazara/Utility.hpp>
#include <Nazara/Lua/LuaClass.hpp>

namespace Ndk
{
	void LuaAPI::Register_Utility(Nz::LuaInstance& instance)
	{
		/*********************************** Nz::AbstractImage **********************************/
		Nz::LuaClass<Nz::AbstractImage*> abstractImage("AbstractImage");

		abstractImage.SetMethod("GetBytesPerPixel", &Nz::AbstractImage::GetBytesPerPixel);
		abstractImage.SetMethod("GetDepth", &Nz::AbstractImage::GetDepth, static_cast<Nz::UInt8>(0));
		abstractImage.SetMethod("GetFormat", &Nz::AbstractImage::GetFormat);
		abstractImage.SetMethod("GetHeight", &Nz::AbstractImage::GetHeight, static_cast<Nz::UInt8>(0));
		abstractImage.SetMethod("GetLevelCount", &Nz::AbstractImage::GetLevelCount);
		abstractImage.SetMethod("GetMaxLevel", &Nz::AbstractImage::GetMaxLevel);
		abstractImage.SetMethod("GetSize", &Nz::AbstractImage::GetSize, static_cast<Nz::UInt8>(0));
		abstractImage.SetMethod("GetType", &Nz::AbstractImage::GetType);
		abstractImage.SetMethod("GetWidth", &Nz::AbstractImage::GetWidth, static_cast<Nz::UInt8>(0));
		abstractImage.SetMethod("IsCompressed", &Nz::AbstractImage::IsCompressed);
		abstractImage.SetMethod("IsCubemap", &Nz::AbstractImage::IsCubemap);

		abstractImage.SetMethod("GetMemoryUsage", [] (Nz::LuaInstance& lua, Nz::AbstractImage* abstractImage) -> int
		{
			unsigned int argCount = std::min(lua.GetStackTop(), 1U);
			switch (argCount)
			{
				case 0:
					return lua.Push(abstractImage->GetMemoryUsage());

				case 1:
				{
					int index = 1;
					Nz::UInt8 level(lua.Check<Nz::UInt8>(&index));

					return lua.Push(abstractImage->GetMemoryUsage(level));
				}
			}

			lua.Error("No matching overload for method GetMemoryUsage");
			return 0;
		});

		abstractImage.SetMethod("Update", [] (Nz::LuaInstance& lua, Nz::AbstractImage* abstractImage) -> int
		{
			unsigned int argCount = std::min(lua.GetStackTop(), 6U);
			int argIndex = 1;

			std::size_t bufferSize = 0;
			const Nz::UInt8* pixels = reinterpret_cast<const Nz::UInt8*>(lua.CheckString(argIndex++, &bufferSize));

			if (argCount < 2 || lua.IsOfType(2, Nz::LuaType_Number))
			{
				// bool Update(const UInt8* pixels, unsigned int srcWidth = 0, unsigned int srcHeight = 0, UInt8 level = 0)
				unsigned int srcWidth = lua.Check<unsigned int>(&argIndex, 0);
				unsigned int srcHeight = lua.Check<unsigned int>(&argIndex, 0);
				Nz::UInt8 level = lua.Check<Nz::UInt8>(&argIndex, 0);

				///TODO: Buffer checks (Nz::ByteBufferView ?)
				return lua.Push(abstractImage->Update(pixels, srcWidth, srcHeight, level));
			}
			/* Disabled until Box and Rect have been ported
			else if (lua.IsOfType(2, "Box"))
			{
				// bool Update(const UInt8* pixels, const Boxui& box, unsigned int srcWidth = 0, unsigned int srcHeight = 0, UInt8 level = 0)
				Nz::Boxui box = lua.Check<Nz::Boxui>(&argIndex);
				unsigned int srcWidth = lua.Check<unsigned int>(&argIndex, 0);
				unsigned int srcHeight = lua.Check<unsigned int>(&argIndex, 0);
				Nz::UInt8 level = lua.Check<Nz::UInt8>(&argIndex, 0);

				///TODO: Buffer checks (Nz::ByteBufferView ?)
				return lua.Push(abstractImage->Update(pixels, srcWidth, srcHeight, level));
			}
			else if (lua.IsOfType(2, "Rect"))
			{
				// bool Update(const UInt8* pixels, const Rectui& rect, unsigned int z = 0, unsigned int srcWidth = 0, unsigned int srcHeight = 0, UInt8 level = 0)
				Nz::Rectui box = lua.Check<Nz::Rectui>(&argIndex);
				unsigned int srcWidth = lua.Check<unsigned int>(&argIndex, 0);
				unsigned int srcHeight = lua.Check<unsigned int>(&argIndex, 0);
				Nz::UInt8 level = lua.Check<Nz::UInt8>(&argIndex, 0);

				///TODO: Buffer checks (Nz::ByteBufferView ?)
				return lua.Push(abstractImage->Update(pixels, srcWidth, srcHeight, level));
			}*/

			lua.Error("No matching overload for method Update");
			return 0;
		});

		abstractImage.Register(instance);
	}
}