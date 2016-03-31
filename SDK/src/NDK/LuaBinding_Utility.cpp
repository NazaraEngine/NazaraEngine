// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

#include <NDK/LuaBinding.hpp>
#include <NDK/LuaAPI.hpp>

namespace Ndk
{
	void LuaBinding::BindUtility()
	{
		/*********************************** Nz::AbstractImage **********************************/
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

		/*********************************** Nz::Node **********************************/
		nodeClass.SetMethod("GetBackward", &Nz::Node::GetBackward);
		//nodeClass.SetMethod("GetChilds", &Nz::Node::GetChilds);
		nodeClass.SetMethod("GetDown", &Nz::Node::GetDown);
		nodeClass.SetMethod("GetForward", &Nz::Node::GetForward);
		nodeClass.SetMethod("GetInheritPosition", &Nz::Node::GetInheritPosition);
		nodeClass.SetMethod("GetInheritRotation", &Nz::Node::GetInheritRotation);
		nodeClass.SetMethod("GetInheritScale", &Nz::Node::GetInheritScale);
		nodeClass.SetMethod("GetInitialPosition", &Nz::Node::GetInitialPosition);
		//nodeClass.SetMethod("GetInitialRotation", &Nz::Node::GetInitialRotation);
		nodeClass.SetMethod("GetInitialScale", &Nz::Node::GetInitialScale);
		nodeClass.SetMethod("GetLeft", &Nz::Node::GetLeft);
		nodeClass.SetMethod("GetNodeType", &Nz::Node::GetNodeType);
		//nodeClass.SetMethod("GetParent", &Nz::Node::GetParent);
		nodeClass.SetMethod("GetPosition", &Nz::Node::GetPosition, Nz::CoordSys_Global);
		nodeClass.SetMethod("GetRight", &Nz::Node::GetRight);
		//nodeClass.SetMethod("GetRotation", &Nz::Node::GetRotation, Nz::CoordSys_Global);
		nodeClass.SetMethod("GetScale", &Nz::Node::GetScale, Nz::CoordSys_Global);
		//nodeClass.SetMethod("GetTransformMatrix", &Nz::Node::GetTransformMatrix);
		nodeClass.SetMethod("GetUp", &Nz::Node::GetUp);

		nodeClass.SetMethod("HasChilds", &Nz::Node::HasChilds);

		nodeClass.SetMethod("GetBackward", &Nz::Node::GetBackward);
		nodeClass.SetMethod("GetDown", &Nz::Node::GetDown);
		nodeClass.SetMethod("GetForward", &Nz::Node::GetForward);
		nodeClass.SetMethod("GetInheritPosition", &Nz::Node::GetInheritPosition);
		nodeClass.SetMethod("GetInheritRotation", &Nz::Node::GetInheritRotation);
		nodeClass.SetMethod("GetInheritScale", &Nz::Node::GetInheritScale);
		nodeClass.SetMethod("GetInitialPosition", &Nz::Node::GetInitialPosition);
		nodeClass.SetMethod("GetInitialRotation", &Nz::Node::GetInitialRotation);
		nodeClass.SetMethod("GetInitialScale", &Nz::Node::GetInitialScale);
		nodeClass.SetMethod("GetLeft", &Nz::Node::GetLeft);
		nodeClass.SetMethod("GetNodeType", &Nz::Node::GetNodeType);
		nodeClass.SetMethod("GetPosition", &Nz::Node::GetPosition, Nz::CoordSys_Global);
		nodeClass.SetMethod("GetRight", &Nz::Node::GetRight);
		nodeClass.SetMethod("GetRotation", &Nz::Node::GetRotation, Nz::CoordSys_Global);
		nodeClass.SetMethod("GetScale", &Nz::Node::GetScale, Nz::CoordSys_Global);
		nodeClass.SetMethod("GetUp", &Nz::Node::GetUp);

		nodeClass.SetMethod("SetInitialPosition", (void(Nz::Node::*)(const Nz::Vector3f&)) &Nz::Node::SetInitialPosition);
		nodeClass.SetMethod("SetInitialRotation", (void(Nz::Node::*)(const Nz::Quaternionf&)) &Nz::Node::SetInitialRotation);

		nodeClass.SetMethod("SetPosition", (void(Nz::Node::*)(const Nz::Vector3f&, Nz::CoordSys)) &Nz::Node::SetPosition, Nz::CoordSys_Local);
		nodeClass.SetMethod("SetRotation", (void(Nz::Node::*)(const Nz::Quaternionf&, Nz::CoordSys)) &Nz::Node::SetRotation, Nz::CoordSys_Local);

		nodeClass.SetMethod("Move", [] (Nz::LuaInstance& lua, Nz::Node& node) -> int
		{
			int argIndex = 1;

			Nz::Vector3f offset = lua.Check<Nz::Vector3f>(&argIndex);
			Nz::CoordSys coordSys = lua.Check<Nz::CoordSys>(&argIndex, Nz::CoordSys_Local);
			node.Move(offset, coordSys);

			return 0;
		});

		nodeClass.SetMethod("Rotate", [] (Nz::LuaInstance& lua, Nz::Node& node) -> int
		{
			int argIndex = 1;

			Nz::Quaternionf rotation = lua.Check<Nz::Quaternionf>(&argIndex);
			Nz::CoordSys coordSys = lua.Check<Nz::CoordSys>(&argIndex, Nz::CoordSys_Local);
			node.Rotate(rotation, coordSys);

			return 0;
		});

		nodeClass.SetMethod("Scale", [] (Nz::LuaInstance& lua, Nz::Node& node) -> int
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

		nodeClass.SetMethod("SetScale", [] (Nz::LuaInstance& lua, Nz::Node& node) -> int
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

		nodeClass.SetMethod("SetInitialScale", [] (Nz::LuaInstance& lua, Nz::Node& node) -> int
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

	void LuaBinding::RegisterUtility(Nz::LuaInstance& instance)
	{
		abstractImage.Register(instance);
		nodeClass.Register(instance);
	}
}