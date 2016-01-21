// Copyright (C) 2016 Jérôme Leclercq, Arnaud Cadot
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp


#include <NDK/LuaAPI.hpp>
#include <Nazara/Renderer.hpp>
#include <Nazara/Lua/LuaClass.hpp>

namespace Ndk
{
	void LuaAPI::Register_Renderer(Nz::LuaInstance& instance)
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
		
		abstractImage.SetMethod("GetMemoryUsage",
			[](Nz::LuaInstance& lua, Nz::AbstractImage* abstractImage) -> int
			{
				unsigned int memory = 0;

				unsigned int argCount = std::min(lua.GetStackTop(), 2U);
				if (argCount == 1 && lua.IsOfType(1, Nz::LuaType_Number))
					memory = abstractImage->GetMemoryUsage(Nz::ranged_cast<Nz::UInt8>(lua.CheckInteger(1)));
				else
					memory = abstractImage->GetMemoryUsage();

				lua.PushInteger(memory);
				return 1;
			});
		
		abstractImage.SetMethod("__tostring",
			[](Nz::LuaInstance& lua, Nz::AbstractImage* abstractImage) -> int
			{
				 lua.PushString(Nz::StringStream("AbstractImage()"));
				 return 1;
			});

		abstractImage.SetMethod("Update",
			[](Nz::LuaInstance& lua, Nz::AbstractImage* abstractImage) -> int
			{
				unsigned int argCount = std::min(lua.GetStackTop(), 2U);
				
				if (argCount < 1)
				{
					lua.Error("No matching overload for method AbstractImage::Update");
					return 0;
				}

				bool rValue;
				const Nz::UInt8* pixels = reinterpret_cast<const Nz::UInt8*>(lua.CheckString(1));
					
				if (argCount == 1)
					rValue = abstractImage->Update(pixels);
				else
				{
					// Three possible overloads, based on second argument type
					if (lua.IsOfType(2, Nz::LuaType_Number)) // Overload #1
					{
						/* Prototype is:
						virtual bool Update(const UInt8* pixels, unsigned int srcWidth = 0, unsigned int srcHeight = 0, UInt8 level = 0)
						*/

						unsigned int	srcWidth	= Nz::ranged_cast<unsigned int>(lua.CheckInteger(2));
						unsigned int	srcHeight	= 0;
						Nz::UInt8		level		= 0;

						if (argCount >= 3)
						{
							srcHeight = Nz::ranged_cast<unsigned int>(lua.CheckInteger(3));
							if (argCount >= 4)
								level = Nz::ranged_cast<Nz::UInt8>(lua.CheckInteger(3));
						}

						rValue = abstractImage->Update(pixels, srcWidth, srcHeight, level);
					}
					else if (lua.IsOfType(2, "Box")) // Overload #2
					{
						/* Prototype is:
						virtual bool Update(const UInt8* pixels, const Boxui& box, unsigned int srcWidth = 0, unsigned int srcHeight = 0, UInt8 level = 0)
						*/

						Nz::Boxui		box			(*(static_cast<Nz::Boxui*>(lua.ToUserdata(2)))); // Placeholder. Ask Lynix about templates & bindings. Nz::Box<T> has to be bound, too.
						unsigned int	srcWidth	= 0;
						unsigned int	srcHeight	= 0;
						Nz::UInt8		level		= 0;

						if (argCount >= 3)
						{
							srcWidth = Nz::ranged_cast<unsigned int>(lua.CheckInteger(3));
							if (argCount >= 4)
							{
								srcHeight = Nz::ranged_cast<unsigned int>(lua.CheckInteger(4));
								if (argCount >= 5)
									level = Nz::ranged_cast<Nz::UInt8>(lua.CheckInteger(5));
							}
						}
						rValue = abstractImage->Update(pixels, box, srcWidth, srcHeight, level);
					}
					else if (lua.IsOfType(2, "Rect")) // Overload #3
					{
						/* Prototype is:
						virtual bool Update(const UInt8* pixels, const Rectui& rect, unsigned int z = 0, unsigned int srcWidth = 0, unsigned int srcHeight = 0, UInt8 level = 0)
						*/

						Nz::Rectui		rect(*(static_cast<Nz::Rectui*>(lua.ToUserdata(2)))); // Placeholder. See comment at box declaration in overload #2
						unsigned int	z			= 0;
						unsigned int	srcWidth	= 0;
						unsigned int	srcHeight	= 0;
						Nz::UInt8		level		= 0;

						if (argCount >= 3)
						{
							z = Nz::ranged_cast<unsigned int>(lua.CheckInteger(3));
							if (argCount >= 4)
							{
								srcWidth = Nz::ranged_cast<unsigned int>(lua.CheckInteger(4));
								if (argCount >= 5)
								{
									srcHeight = Nz::ranged_cast<unsigned int>(lua.CheckInteger(5));
									if (argCount >= 6)
										level = Nz::ranged_cast<Nz::UInt8>(lua.CheckInteger(6));
								}
							}
						}
						rValue = abstractImage->Update(pixels, rect, z, srcWidth, srcHeight, level);
					}
				}
					
				lua.PushBoolean(rValue);
				return 1;
			});

		abstractImage.Register(instance);
	}
}