// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

#include <NDK/LuaBinding.hpp>
#include <NDK/LuaAPI.hpp>

namespace Ndk
{
	/*!
	* \brief Binds Utility module to Lua
	*/

	void LuaBinding::BindUtility()
	{
		/*********************************** Nz::AbstractImage **********************************/
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

		abstractImage.BindMethod("GetMemoryUsage", [] (Nz::LuaInstance& lua, Nz::AbstractImage* abstractImage) -> int
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

		abstractImage.BindMethod("Update", [] (Nz::LuaInstance& lua, Nz::AbstractImage* abstractImage) -> int
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

		/*********************************** Nz::Font **********************************/
		font.SetConstructor([] (Nz::LuaInstance& /*lua*/, Nz::FontRef* font, std::size_t /*argumentCount*/)
		{
			Nz::PlacementNew(font, Nz::Font::New());
			return true;
		});

		font.BindMethod("ClearGlyphCache",    &Nz::Font::ClearGlyphCache);
		font.BindMethod("ClearKerningCache",  &Nz::Font::ClearKerningCache);
		font.BindMethod("ClearSizeInfoCache", &Nz::Font::ClearSizeInfoCache);

		font.BindMethod("Destroy", &Nz::Font::Destroy);

		font.BindMethod("GetCachedGlyphCount", [] (Nz::LuaInstance& lua, Nz::FontRef& instance) -> int
		{
			unsigned int argCount = std::min(lua.GetStackTop(), 2U);

			int argIndex = 1;
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

		font.BindMethod("GetFamilyName",      &Nz::Font::GetFamilyName);
		font.BindMethod("GetKerning",         &Nz::Font::GetKerning);
		font.BindMethod("GetGlyphBorder",     &Nz::Font::GetGlyphBorder);
		font.BindMethod("GetMinimumStepSize", &Nz::Font::GetMinimumStepSize);
		font.BindMethod("GetSizeInfo",        &Nz::Font::GetSizeInfo);
		font.BindMethod("GetStyleName",       &Nz::Font::GetStyleName);

		font.BindMethod("IsValid", &Nz::Font::IsValid);

		font.BindMethod("Precache", (bool(Nz::Font::*)(unsigned int, Nz::UInt32, const Nz::String&) const) &Nz::Font::Precache);

		font.BindMethod("OpenFromFile", &Nz::Font::OpenFromFile, Nz::FontParams());

		font.BindMethod("SetGlyphBorder",     &Nz::Font::SetGlyphBorder);
		font.BindMethod("SetMinimumStepSize", &Nz::Font::SetMinimumStepSize);

		font.BindStaticMethod("GetDefault",                &Nz::Font::GetDefault);
		font.BindStaticMethod("GetDefaultGlyphBorder",     &Nz::Font::GetDefaultGlyphBorder);
		font.BindStaticMethod("GetDefaultMinimumStepSize", &Nz::Font::GetDefaultMinimumStepSize);

		font.BindStaticMethod("SetDefaultGlyphBorder",     &Nz::Font::SetDefaultGlyphBorder);
		font.BindStaticMethod("SetDefaultMinimumStepSize", &Nz::Font::SetDefaultMinimumStepSize);

		/*********************************** Nz::Node **********************************/
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

		node.BindMethod("Move", [] (Nz::LuaInstance& lua, Nz::Node& node) -> int
		{
			int argIndex = 1;

			Nz::Vector3f offset = lua.Check<Nz::Vector3f>(&argIndex);
			Nz::CoordSys coordSys = lua.Check<Nz::CoordSys>(&argIndex, Nz::CoordSys_Local);
			node.Move(offset, coordSys);

			return 0;
		});

		node.BindMethod("Rotate", [] (Nz::LuaInstance& lua, Nz::Node& node) -> int
		{
			int argIndex = 1;

			Nz::Quaternionf rotation = lua.Check<Nz::Quaternionf>(&argIndex);
			Nz::CoordSys coordSys = lua.Check<Nz::CoordSys>(&argIndex, Nz::CoordSys_Local);
			node.Rotate(rotation, coordSys);

			return 0;
		});

		node.BindMethod("Scale", [] (Nz::LuaInstance& lua, Nz::Node& node) -> int
		{
			unsigned int argCount = std::min(lua.GetStackTop(), 4U);

			int argIndex = 1;
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

		node.BindMethod("SetScale", [] (Nz::LuaInstance& lua, Nz::Node& node) -> int
		{
			unsigned int argCount = std::min(lua.GetStackTop(), 4U);

			int argIndex = 1;
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

		node.BindMethod("SetInitialScale", [] (Nz::LuaInstance& lua, Nz::Node& node) -> int
		{
			unsigned int argCount = std::min(lua.GetStackTop(), 4U);

			int argIndex = 1;
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

	/*!
	* \brief Registers the classes that will be used by the Lua instance
	*
	* \param instance Lua instance that will interact with the Utility classes
	*/

	void LuaBinding::RegisterUtility(Nz::LuaInstance& instance)
	{
		abstractImage.Register(instance);
		font.Register(instance);
		node.Register(instance);
	}
}