// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

#include <NDK/Lua/LuaBinding_Platform.hpp>
#include <NDK/LuaAPI.hpp>

namespace Ndk
{
	std::unique_ptr<LuaBinding_Base> LuaBinding_Base::BindPlatform(LuaBinding& binding)
	{
		return std::make_unique<LuaBinding_Platform>(binding);
	}

	LuaBinding_Platform::LuaBinding_Platform(LuaBinding& binding) :
	LuaBinding_Base(binding)
	{
		/*********************************** Nz::Keyboard **********************************/
		keyboard.Reset("Keyboard");
		{
			keyboard.BindStaticMethod("GetKeyName", &Nz::Keyboard::GetKeyName);
			keyboard.BindStaticMethod("IsKeyPressed", &Nz::Keyboard::IsKeyPressed);
		}
	}

	/*!
	* \brief Registers the classes that will be used by the Lua instance
	*
	* \param instance Lua instance that will interact with the Utility classes
	*/
	void LuaBinding_Platform::Register(Nz::LuaState& state)
	{
		keyboard.Register(state);

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
