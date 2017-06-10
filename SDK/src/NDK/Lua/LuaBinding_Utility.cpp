// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

#include <NDK/Lua/LuaBinding_Utility.hpp>
#include <NDK/LuaAPI.hpp>

namespace Ndk
{
	std::unique_ptr<LuaBinding_Base> LuaBinding_Base::BindUtility(LuaBinding& binding)
	{
		return std::make_unique<LuaBinding_Utility>(binding);
	}

	LuaBinding_Utility::LuaBinding_Utility(LuaBinding& binding) :
	LuaBinding_Base(binding)
	{
		/*********************************** Nz::AbstractImage **********************************/
		abstractImage.Reset("AbstractImage");
		{
			abstractImage.BindMethod("GetBytesPerPixel", &Nz::AbstractImage::GetBytesPerPixel);
			abstractImage.BindMethod("GetDepth", &Nz::AbstractImage::GetDepth, static_cast<Nz::UInt8>(0));
			abstractImage.BindMethod("GetFormat", &Nz::AbstractImage::GetFormat);
			abstractImage.BindMethod("GetHeight", &Nz::AbstractImage::GetHeight, static_cast<Nz::UInt8>(0));
			abstractImage.BindMethod("GetLevelCount", &Nz::AbstractImage::GetLevelCount);
			abstractImage.BindMethod("GetMaxLevel", &Nz::AbstractImage::GetMaxLevel);
			abstractImage.BindMethod("GetSize", &Nz::AbstractImage::GetSize, static_cast<Nz::UInt8>(0));
			abstractImage.BindMethod("GetType", &Nz::AbstractImage::GetType);
			abstractImage.BindMethod("GetWidth", &Nz::AbstractImage::GetWidth, static_cast<Nz::UInt8>(0));
			abstractImage.BindMethod("IsCompressed", &Nz::AbstractImage::IsCompressed);
			abstractImage.BindMethod("IsCubemap", &Nz::AbstractImage::IsCubemap);

			abstractImage.BindMethod("GetMemoryUsage", [] (Nz::LuaState& lua, Nz::AbstractImage* instance, std::size_t argumentCount) -> int
			{
				std::size_t argCount = std::min<std::size_t>(argumentCount, 1U);
				switch (argCount)
				{
					case 0:
						return lua.Push(instance->GetMemoryUsage());

					case 1:
					{
						int argIndex = 2;
						Nz::UInt8 level(lua.Check<Nz::UInt8>(&argIndex));

						return lua.Push(instance->GetMemoryUsage(level));
					}
				}

				lua.Error("No matching overload for method GetMemoryUsage");
				return 0;
			});

			abstractImage.BindMethod("Update", [] (Nz::LuaState& lua, Nz::AbstractImage* instance, std::size_t argumentCount) -> int
			{
				std::size_t argCount = std::min<std::size_t>(argumentCount, 6U);
				int argIndex = 2;

				std::size_t bufferSize = 0;
				const Nz::UInt8* pixels = reinterpret_cast<const Nz::UInt8*>(lua.CheckString(argIndex++, &bufferSize));

				if (argCount < 2 || lua.IsOfType(2, Nz::LuaType_Number))
				{
					// bool Update(const UInt8* pixels, unsigned int srcWidth = 0, unsigned int srcHeight = 0, UInt8 level = 0)
					unsigned int srcWidth = lua.Check<unsigned int>(&argIndex, 0);
					unsigned int srcHeight = lua.Check<unsigned int>(&argIndex, 0);
					Nz::UInt8 level = lua.Check<Nz::UInt8>(&argIndex, 0);

					///TODO: Buffer checks (Nz::ByteBufferView ?)
					return lua.Push(instance->Update(pixels, srcWidth, srcHeight, level));
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
		}

		/*********************************** Nz::Font **********************************/
		font.Reset("Font");
		{
			font.SetConstructor([] (Nz::LuaState& /*lua*/, Nz::FontRef* instance, std::size_t /*argumentCount*/)
			{
				Nz::PlacementNew(instance, Nz::Font::New());
				return true;
			});

			font.BindMethod("ClearGlyphCache", &Nz::Font::ClearGlyphCache);
			font.BindMethod("ClearKerningCache", &Nz::Font::ClearKerningCache);
			font.BindMethod("ClearSizeInfoCache", &Nz::Font::ClearSizeInfoCache);

			font.BindMethod("Destroy", &Nz::Font::Destroy);

			font.BindMethod("GetCachedGlyphCount", [] (Nz::LuaState& lua, Nz::FontRef& instance, std::size_t argumentCount) -> int
			{
				std::size_t argCount = std::min<std::size_t>(argumentCount, 2U);

				int argIndex = 2;
				switch (argCount)
				{
					case 0:
						lua.Push(instance->GetCachedGlyphCount());
						return 1;

					case 2:
					{
						unsigned int characterSize = lua.Check<unsigned int>(&argIndex);
						Nz::UInt32 style = lua.Check<Nz::UInt32>(&argIndex);

						lua.Push(instance->GetCachedGlyphCount(characterSize, style));
						return 1;
					}
				}

				lua.Error("No matching overload for method GetCachedGlyphCount");
				return 0;
			});

			font.BindMethod("GetFamilyName", &Nz::Font::GetFamilyName);
			font.BindMethod("GetKerning", &Nz::Font::GetKerning);
			font.BindMethod("GetGlyphBorder", &Nz::Font::GetGlyphBorder);
			font.BindMethod("GetMinimumStepSize", &Nz::Font::GetMinimumStepSize);
			font.BindMethod("GetSizeInfo", &Nz::Font::GetSizeInfo);
			font.BindMethod("GetStyleName", &Nz::Font::GetStyleName);

			font.BindMethod("IsValid", &Nz::Font::IsValid);

			font.BindMethod("Precache", (bool(Nz::Font::*)(unsigned int, Nz::UInt32, const Nz::String&) const) &Nz::Font::Precache);

			font.BindMethod("OpenFromFile", &Nz::Font::OpenFromFile, Nz::FontParams());

			font.BindMethod("SetGlyphBorder", &Nz::Font::SetGlyphBorder);
			font.BindMethod("SetMinimumStepSize", &Nz::Font::SetMinimumStepSize);

			font.BindStaticMethod("GetDefault", &Nz::Font::GetDefault);
			font.BindStaticMethod("GetDefaultGlyphBorder", &Nz::Font::GetDefaultGlyphBorder);
			font.BindStaticMethod("GetDefaultMinimumStepSize", &Nz::Font::GetDefaultMinimumStepSize);

			font.BindStaticMethod("SetDefaultGlyphBorder", &Nz::Font::SetDefaultGlyphBorder);
			font.BindStaticMethod("SetDefaultMinimumStepSize", &Nz::Font::SetDefaultMinimumStepSize);
		}

		/*********************************** Nz::Keyboard **********************************/
		keyboard.Reset("Keyboard");
		{
			keyboard.BindStaticMethod("GetKeyName", &Nz::Keyboard::GetKeyName);
			keyboard.BindStaticMethod("IsKeyPressed", &Nz::Keyboard::IsKeyPressed);
		}

		/*********************************** Nz::Node **********************************/
		node.Reset("Node");
		{
			node.BindMethod("GetBackward", &Nz::Node::GetBackward);
					//nodeClass.SetMethod("GetChilds", &Nz::Node::GetChilds);
			node.BindMethod("GetDown", &Nz::Node::GetDown);
			node.BindMethod("GetForward", &Nz::Node::GetForward);
			node.BindMethod("GetInheritPosition", &Nz::Node::GetInheritPosition);
			node.BindMethod("GetInheritRotation", &Nz::Node::GetInheritRotation);
			node.BindMethod("GetInheritScale", &Nz::Node::GetInheritScale);
			node.BindMethod("GetInitialPosition", &Nz::Node::GetInitialPosition);
			//nodeClass.SetMethod("GetInitialRotation", &Nz::Node::GetInitialRotation);
			node.BindMethod("GetInitialScale", &Nz::Node::GetInitialScale);
			node.BindMethod("GetLeft", &Nz::Node::GetLeft);
			node.BindMethod("GetNodeType", &Nz::Node::GetNodeType);
			//nodeClass.SetMethod("GetParent", &Nz::Node::GetParent);
			node.BindMethod("GetPosition", &Nz::Node::GetPosition, Nz::CoordSys_Global);
			node.BindMethod("GetRight", &Nz::Node::GetRight);
			//nodeClass.SetMethod("GetRotation", &Nz::Node::GetRotation, Nz::CoordSys_Global);
			node.BindMethod("GetScale", &Nz::Node::GetScale, Nz::CoordSys_Global);
			//nodeClass.SetMethod("GetTransformMatrix", &Nz::Node::GetTransformMatrix);
			node.BindMethod("GetUp", &Nz::Node::GetUp);

			node.BindMethod("HasChilds", &Nz::Node::HasChilds);

			node.BindMethod("GetBackward", &Nz::Node::GetBackward);
			node.BindMethod("GetDown", &Nz::Node::GetDown);
			node.BindMethod("GetForward", &Nz::Node::GetForward);
			node.BindMethod("GetInheritPosition", &Nz::Node::GetInheritPosition);
			node.BindMethod("GetInheritRotation", &Nz::Node::GetInheritRotation);
			node.BindMethod("GetInheritScale", &Nz::Node::GetInheritScale);
			node.BindMethod("GetInitialPosition", &Nz::Node::GetInitialPosition);
			node.BindMethod("GetInitialRotation", &Nz::Node::GetInitialRotation);
			node.BindMethod("GetInitialScale", &Nz::Node::GetInitialScale);
			node.BindMethod("GetLeft", &Nz::Node::GetLeft);
			node.BindMethod("GetNodeType", &Nz::Node::GetNodeType);
			node.BindMethod("GetPosition", &Nz::Node::GetPosition, Nz::CoordSys_Global);
			node.BindMethod("GetRight", &Nz::Node::GetRight);
			node.BindMethod("GetRotation", &Nz::Node::GetRotation, Nz::CoordSys_Global);
			node.BindMethod("GetScale", &Nz::Node::GetScale, Nz::CoordSys_Global);
			node.BindMethod("GetUp", &Nz::Node::GetUp);

			node.BindMethod("SetInitialPosition", (void(Nz::Node::*)(const Nz::Vector3f&)) &Nz::Node::SetInitialPosition);
			node.BindMethod("SetInitialRotation", (void(Nz::Node::*)(const Nz::Quaternionf&)) &Nz::Node::SetInitialRotation);

			node.BindMethod("SetPosition", (void(Nz::Node::*)(const Nz::Vector3f&, Nz::CoordSys)) &Nz::Node::SetPosition, Nz::CoordSys_Local);
			node.BindMethod("SetRotation", (void(Nz::Node::*)(const Nz::Quaternionf&, Nz::CoordSys)) &Nz::Node::SetRotation, Nz::CoordSys_Local);

			node.BindMethod("Move", [] (Nz::LuaState& lua, Nz::Node& node, std::size_t /*argumentCount*/) -> int
			{
				int argIndex = 2;

				Nz::Vector3f offset = lua.Check<Nz::Vector3f>(&argIndex);
				Nz::CoordSys coordSys = lua.Check<Nz::CoordSys>(&argIndex, Nz::CoordSys_Local);
				node.Move(offset, coordSys);

				return 0;
			});

			node.BindMethod("Rotate", [] (Nz::LuaState& lua, Nz::Node& node, std::size_t /*argumentCount*/) -> int
			{
				int argIndex = 2;

				Nz::Quaternionf rotation = lua.Check<Nz::Quaternionf>(&argIndex);
				Nz::CoordSys coordSys = lua.Check<Nz::CoordSys>(&argIndex, Nz::CoordSys_Local);
				node.Rotate(rotation, coordSys);

				return 0;
			});

			node.BindMethod("Scale", [] (Nz::LuaState& lua, Nz::Node& node, std::size_t argumentCount) -> int
			{
				std::size_t argCount = std::min<std::size_t>(argumentCount, 4U);

				int argIndex = 2;
				switch (argCount)
				{
					case 1:
					{
						if (lua.IsOfType(argIndex, Nz::LuaType_Number))
							node.Scale(lua.Check<float>(&argIndex));
						else
							node.Scale(lua.Check<Nz::Vector3f>(&argIndex));

						return 0;
					}

					case 3:
						node.Scale(lua.Check<Nz::Vector3f>(&argIndex));
						return 0;
				}

				lua.Error("No matching overload for method Scale");
				return 0;
			});

			node.BindMethod("SetScale", [] (Nz::LuaState& lua, Nz::Node& node, std::size_t argumentCount) -> int
			{
				std::size_t argCount = std::min<std::size_t>(argumentCount, 4U);

				int argIndex = 2;
				switch (argCount)
				{
					case 1:
					case 2:
					{
						if (lua.IsOfType(argIndex, Nz::LuaType_Number))
						{
							float scale = lua.Check<float>(&argIndex);
							Nz::CoordSys coordSys = lua.Check<Nz::CoordSys>(&argIndex, Nz::CoordSys_Local);
							node.SetScale(scale, coordSys);
						}
						else
							node.SetScale(lua.Check<Nz::Vector3f>(&argIndex));

						return 0;
					}

					case 3:
					case 4:
					{
						Nz::Vector3f scale = lua.Check<Nz::Vector3f>(&argIndex);
						Nz::CoordSys coordSys = lua.Check<Nz::CoordSys>(&argIndex, Nz::CoordSys_Local);

						node.SetScale(scale, coordSys);
						return 0;
					}
				}

				lua.Error("No matching overload for method SetScale");
				return 0;
			});

			node.BindMethod("SetInitialScale", [] (Nz::LuaState& lua, Nz::Node& node, std::size_t argumentCount) -> int
			{
				std::size_t argCount = std::min<std::size_t>(argumentCount, 4U);

				int argIndex = 2;
				switch (argCount)
				{
					case 1:
					{
						if (lua.IsOfType(argIndex, Nz::LuaType_Number))
							node.SetInitialScale(lua.Check<float>(&argIndex));
						else
							node.SetInitialScale(lua.Check<Nz::Vector2f>(&argIndex));

						return 0;
					}

					case 2:
					case 3:
						node.SetInitialScale(lua.Check<Nz::Vector3f>(&argIndex));
						return 0;
				}

				lua.Error("No matching overload for method SetInitialScale");
				return 0;
			});
		}
	}

	/*!
	* \brief Registers the classes that will be used by the Lua instance
	*
	* \param instance Lua instance that will interact with the Utility classes
	*/
	void LuaBinding_Utility::Register(Nz::LuaState& state)
	{
		abstractImage.Register(state);
		font.Register(state);
		keyboard.Register(state);
		node.Register(state);

		keyboard.PushGlobalTable(state);
		{
			static_assert(Nz::Keyboard::Count == 121, "Nz::Keyboard::Key has been updated but change was not reflected to Lua binding");

			state.PushField("Undefined", Nz::Keyboard::Undefined);

			// A-Z
			for (std::size_t i = 0; i < 26; ++i)
				state.PushField(Nz::String('A' + char(i)), Nz::Keyboard::A + i);

			// Numerical
			for (std::size_t i = 0; i < 10; ++i)
			{
				state.PushField("Num" + Nz::String::Number(i), Nz::Keyboard::Num0 + i);
				state.PushField("Numpad" + Nz::String::Number(i), Nz::Keyboard::Numpad0 + i);
			}

			// F1-F15
			for (std::size_t i = 0; i < 15; ++i)
				state.PushField('F' + Nz::String::Number(i+1), Nz::Keyboard::F1 + i);

			// And all the others...
			state.PushField("Down",              Nz::Keyboard::Down);
			state.PushField("Left",              Nz::Keyboard::Left);
			state.PushField("Right",             Nz::Keyboard::Right);
			state.PushField("Up",                Nz::Keyboard::Up);

			state.PushField("Add",               Nz::Keyboard::Add);
			state.PushField("Decimal",           Nz::Keyboard::Decimal);
			state.PushField("Divide",            Nz::Keyboard::Divide);
			state.PushField("Multiply",          Nz::Keyboard::Multiply);
			state.PushField("Subtract",          Nz::Keyboard::Subtract);

			state.PushField("Backslash",         Nz::Keyboard::Backslash);
			state.PushField("Backspace",         Nz::Keyboard::Backspace);
			state.PushField("Clear",             Nz::Keyboard::Clear);
			state.PushField("Comma",             Nz::Keyboard::Comma);
			state.PushField("Dash",              Nz::Keyboard::Dash);
			state.PushField("Delete",            Nz::Keyboard::Delete);
			state.PushField("End",               Nz::Keyboard::End);
			state.PushField("Equal",             Nz::Keyboard::Equal);
			state.PushField("Escape",            Nz::Keyboard::Escape);
			state.PushField("Home",              Nz::Keyboard::Home);
			state.PushField("Insert",            Nz::Keyboard::Insert);
			state.PushField("LAlt",              Nz::Keyboard::LAlt);
			state.PushField("LBracket",          Nz::Keyboard::LBracket);
			state.PushField("LControl",          Nz::Keyboard::LControl);
			state.PushField("LShift",            Nz::Keyboard::LShift);
			state.PushField("LSystem",           Nz::Keyboard::LSystem);
			state.PushField("PageDown",          Nz::Keyboard::PageDown);
			state.PushField("PageUp",            Nz::Keyboard::PageUp);
			state.PushField("Pause",             Nz::Keyboard::Pause);
			state.PushField("Period",            Nz::Keyboard::Period);
			state.PushField("Print",             Nz::Keyboard::Print);
			state.PushField("PrintScreen",       Nz::Keyboard::PrintScreen);
			state.PushField("Quote",             Nz::Keyboard::Quote);
			state.PushField("RAlt",              Nz::Keyboard::RAlt);
			state.PushField("RBracket",          Nz::Keyboard::RBracket);
			state.PushField("RControl",          Nz::Keyboard::RControl);
			state.PushField("Return",            Nz::Keyboard::Return);
			state.PushField("RShift",            Nz::Keyboard::RShift);
			state.PushField("RSystem",           Nz::Keyboard::RSystem);
			state.PushField("Semicolon",         Nz::Keyboard::Semicolon);
			state.PushField("Slash",             Nz::Keyboard::Slash);
			state.PushField("Space",             Nz::Keyboard::Space);
			state.PushField("Tab",               Nz::Keyboard::Tab);
			state.PushField("Tilde",             Nz::Keyboard::Tilde);
			state.PushField("Browser_Back",      Nz::Keyboard::Browser_Back);
			state.PushField("Browser_Favorites", Nz::Keyboard::Browser_Favorites);
			state.PushField("Browser_Forward",   Nz::Keyboard::Browser_Forward);
			state.PushField("Browser_Home",      Nz::Keyboard::Browser_Home);
			state.PushField("Browser_Refresh",   Nz::Keyboard::Browser_Refresh);
			state.PushField("Browser_Search",    Nz::Keyboard::Browser_Search);
			state.PushField("Browser_Stop",      Nz::Keyboard::Browser_Stop);
			state.PushField("Media_Next",        Nz::Keyboard::Media_Next);
			state.PushField("Media_Play",        Nz::Keyboard::Media_Play);
			state.PushField("Media_Previous",    Nz::Keyboard::Media_Previous);
			state.PushField("Media_Stop",        Nz::Keyboard::Media_Stop);
			state.PushField("Volume_Down",       Nz::Keyboard::Volume_Down);
			state.PushField("Volume_Mute",       Nz::Keyboard::Volume_Mute);
			state.PushField("Volume_Up",         Nz::Keyboard::Volume_Up);
			state.PushField("CapsLock",          Nz::Keyboard::CapsLock);
			state.PushField("NumLock",           Nz::Keyboard::NumLock);
			state.PushField("ScrollLock",        Nz::Keyboard::ScrollLock);
		}
		state.Pop();

		static_assert(Nz::WindowStyle_Max + 1 == 6, "Nz::WindowStyle has been updated but change was not reflected to Lua binding");
		state.PushTable(0, Nz::WindowStyle_Max + 1);
		{
			state.PushField("None",       Nz::WindowStyle_None);
			state.PushField("Fullscreen", Nz::WindowStyle_Fullscreen);
			state.PushField("Closable",   Nz::WindowStyle_Closable);
			state.PushField("Resizable",  Nz::WindowStyle_Resizable);
			state.PushField("Titlebar",   Nz::WindowStyle_Titlebar);
			state.PushField("Threaded",   Nz::WindowStyle_Threaded);
		}
		state.SetGlobal("WindowStyle");
	}
}
