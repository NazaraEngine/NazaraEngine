// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequisites.hpp

#include <Nazara/Core/Color.hpp>
#include <Nazara/Lua/LuaState.hpp>
#include <Nazara/Math/EulerAngles.hpp>
#include <Nazara/Math/Quaternion.hpp>
#include <Nazara/Math/Vector3.hpp>
#include <Nazara/Network/IpAddress.hpp>
#include <Nazara/Utility/Font.hpp>
#include <Nazara/Utility/Image.hpp>
#include <Nazara/Utility/Mesh.hpp>
#include <NDK/Application.hpp>
#include <NDK/Components.hpp>
#include <NDK/Entity.hpp>
#include <NDK/World.hpp>

#ifndef NDK_SERVER
#include <Nazara/Audio/Music.hpp>
#include <Nazara/Audio/SoundBuffer.hpp>
#include <Nazara/Graphics/Model.hpp>
#include <Nazara/Renderer/Texture.hpp>
#include <NDK/Console.hpp>
#endif

namespace Nz
{
	inline unsigned int LuaImplQueryArg(const LuaState& state, int index, Color* color, TypeTag<Color>)
	{
		state.CheckType(index, Nz::LuaType_Table);

		color->r = state.CheckField<UInt8>("r", index);
		color->g = state.CheckField<UInt8>("g", index);
		color->b = state.CheckField<UInt8>("b", index);
		color->a = state.CheckField<UInt8>("a", 255, index);

		return 1;
	}

	inline unsigned int LuaImplQueryArg(const LuaState& state, int index, EulerAnglesd* angles, TypeTag<EulerAnglesd>)
	{
		switch (state.GetType(index))
		{
			case Nz::LuaType_Table:
				angles->Set(state.CheckField<double>("pitch", index), state.CheckField<double>("yaw", index), state.CheckField<double>("roll", index));
				return 1;

			default:
			{
				if (state.IsOfType(index, "EulerAngles"))
					angles->Set(*static_cast<EulerAnglesd*>(state.ToUserdata(index)));
				else
					angles->Set(*static_cast<Quaterniond*>(state.CheckUserdata(index, "Quaternion")));

				return 1;
			}
		}
	}

	inline unsigned int LuaImplQueryArg(const LuaState& state, int index, EulerAnglesf* angles, TypeTag<EulerAnglesf>)
	{
		EulerAnglesd anglesDouble;
		unsigned int ret = LuaImplQueryArg(state, index, &anglesDouble, TypeTag<EulerAnglesd>());

		angles->Set(anglesDouble);
		return ret;
	}

	inline unsigned int LuaImplQueryArg(const LuaState& state, int index, FontRef* fontRef, TypeTag<FontRef>)
	{
		*fontRef = *static_cast<FontRef*>(state.CheckUserdata(index, "Font"));

		return 1;
	}

	inline unsigned int LuaImplQueryArg(const LuaState& state, int index, FontParams* params, TypeTag<FontParams>)
	{
		NazaraUnused(params);

		state.CheckType(index, Nz::LuaType_Table);

		// Structure is empty for now

		return 1;
	}

	inline unsigned int LuaImplQueryArg(const LuaState& state, int index, ImageParams* params, TypeTag<ImageParams>)
	{
		state.CheckType(index, Nz::LuaType_Table);

		params->levelCount = state.CheckField<Nz::UInt8>("LevelCount");
		params->loadFormat = state.CheckField<Nz::PixelFormatType>("LoadFormat");

		return 1;
	}

	inline unsigned int LuaImplQueryArg(const LuaState& state, int index, IpAddress* address, TypeTag<IpAddress>)
	{
		switch (state.GetType(index))
		{
			case Nz::LuaType_String:
				address->BuildFromAddress(state.CheckString(index));
				return 1;

			default:
				*address = *static_cast<IpAddress*>(state.CheckUserdata(index, "IpAddress"));
				return 1;
		}
	}

	inline unsigned int LuaImplQueryArg(const LuaState& state, int index, Matrix4d* mat, TypeTag<Matrix4d>)
	{
		switch (state.GetType(index))
		{
			case Nz::LuaType_Table:
			{
				double values[16];
				for (std::size_t i = 0; i < 16; ++i)
				{
					state.PushInteger(i + 1);
					state.GetTable();

					values[i] = state.CheckNumber(-1);
					state.Pop();
				}

				mat->Set(values);
				return 1;
			}

			default:
			{
				if (state.IsOfType(index, "Matrix4"))
					mat->Set(*static_cast<Matrix4d*>(state.ToUserdata(index)));

				return 1;
			}
		}
	}

	inline unsigned int LuaImplQueryArg(const LuaState& state, int index, Matrix4f* mat, TypeTag<Matrix4f>)
	{
		Matrix4d matDouble = Matrix4d::Identity();
		unsigned int ret = LuaImplQueryArg(state, index, &matDouble, TypeTag<Matrix4d>());

		mat->Set(matDouble);
		return ret;
	}

	inline unsigned int LuaImplQueryArg(const LuaState& state, int index, MeshParams* params, TypeTag<MeshParams>)
	{
		state.CheckType(index, Nz::LuaType_Table);

		params->animated             = state.CheckField<bool>("Animated", params->animated);
		params->center               = state.CheckField<bool>("Center", params->center);
		params->matrix               = state.CheckField<Matrix4f>("Matrix", params->matrix);
		params->optimizeIndexBuffers = state.CheckField<bool>("OptimizeIndexBuffers", params->optimizeIndexBuffers);
		params->texCoordOffset       = state.CheckField<Vector2f>("TexCoordOffset", params->texCoordOffset);
		params->texCoordScale        = state.CheckField<Vector2f>("TexCoordScale", params->texCoordScale);

		return 1;
	}

	inline unsigned int LuaImplQueryArg(const LuaState& state, int index, Quaterniond* quat, TypeTag<Quaterniond>)
	{
		switch (state.GetType(index))
		{
			case Nz::LuaType_Table:
				quat->Set(state.CheckField<double>("w", index), state.CheckField<double>("x", index), state.CheckField<double>("y", index), state.CheckField<double>("z", index));
				return 1;

			default:
			{
				if (state.IsOfType(index, "EulerAngles"))
					quat->Set(*static_cast<EulerAnglesd*>(state.ToUserdata(index)));
				else
					quat->Set(*static_cast<Quaterniond*>(state.CheckUserdata(index, "Quaternion")));

				return 1;
			}
		}
	}

	inline unsigned int LuaImplQueryArg(const LuaState& state, int index, Quaternionf* quat, TypeTag<Quaternionf>)
	{
		Quaterniond quatDouble;
		unsigned int ret = LuaImplQueryArg(state, index, &quatDouble, TypeTag<Quaterniond>());

		quat->Set(quatDouble);
		return ret;
	}

	inline unsigned int LuaImplQueryArg(const LuaState& state, int index, Rectd* rect, TypeTag<Rectd>)
	{
		state.CheckType(index, LuaType_Table);

		rect->x      = state.CheckField<double>("x", index);
		rect->y      = state.CheckField<double>("y", index);
		rect->width  = state.CheckField<double>("width", index);
		rect->height = state.CheckField<double>("height", index);

		return 1;
	}

	inline unsigned int LuaImplQueryArg(const LuaState& state, int index, Rectf* rect, TypeTag<Rectf>)
	{
		Rectd rectDouble;
		unsigned int ret = LuaImplQueryArg(state, index, &rectDouble, TypeTag<Rectd>());

		rect->Set(rectDouble);
		return ret;
	}

	inline unsigned int LuaImplQueryArg(const LuaState& state, int index, Recti* rect, TypeTag<Recti>)
	{
		Rectd rectDouble;
		unsigned int ret = LuaImplQueryArg(state, index, &rectDouble, TypeTag<Rectd>());

		rect->Set(rectDouble);
		return ret;
	}

	inline unsigned int LuaImplQueryArg(const LuaState& state, int index, Rectui* rect, TypeTag<Rectui>)
	{
		Rectd rectDouble;
		unsigned int ret = LuaImplQueryArg(state, index, &rectDouble, TypeTag<Rectd>());

		rect->Set(rectDouble);
		return ret;
	}

	inline unsigned int LuaImplQueryArg(const LuaState& state, int index, Vector2d* vec, TypeTag<Vector2d>)
	{
		switch (state.GetType(index))
		{
			case Nz::LuaType_Number:
				if (index < 0 && index > -2)
					state.Error("Vector2 expected, two numbers are required to convert it");

				vec->Set(state.CheckNumber(index), state.CheckNumber(index + 1));
				return 2;

			case Nz::LuaType_Table:
				vec->Set(state.CheckField<double>("x", index), state.CheckField<double>("y", index));
				return 1;

			default:
				vec->Set(*static_cast<Vector2d*>(state.CheckUserdata(index, "Vector2")));
				return 1;
		}
	}

	inline unsigned int LuaImplQueryArg(const LuaState& state, int index, Vector2f* vec, TypeTag<Vector2f>)
	{
		Vector2d vecDouble;
		unsigned int ret = LuaImplQueryArg(state, index, &vecDouble, TypeTag<Vector2d>());

		vec->Set(vecDouble);
		return ret;
	}

	inline unsigned int LuaImplQueryArg(const LuaState& state, int index, Vector2ui* vec, TypeTag<Vector2ui>)
	{
		Vector2d vecDouble;
		unsigned int ret = LuaImplQueryArg(state, index, &vecDouble, TypeTag<Vector2d>());

		vec->Set(vecDouble);
		return ret;
	}

	inline unsigned int LuaImplQueryArg(const LuaState& state, int index, Vector2i* vec, TypeTag<Vector2i>)
	{
		Vector2d vecDouble;
		unsigned int ret = LuaImplQueryArg(state, index, &vecDouble, TypeTag<Vector2d>());

		vec->Set(vecDouble);
		return ret;
	}

	inline unsigned int LuaImplQueryArg(const LuaState& state, int index, Vector3d* vec, TypeTag<Vector3d>)
	{
		switch (state.GetType(index))
		{
			case Nz::LuaType_Number:
				if (index < 0 && index > -3)
					state.Error("Vector3 expected, three numbers are required to convert it");

				vec->Set(state.CheckNumber(index), state.CheckNumber(index + 1), state.CheckNumber(index + 2, 0.0));
				return 3;

			case Nz::LuaType_Table:
				vec->Set(state.CheckField<double>("x", index), state.CheckField<double>("y", index), state.CheckField<double>("z", 0.0, index));
				return 1;

			default:
				vec->Set(*static_cast<Vector3d*>(state.CheckUserdata(index, "Vector3")));
				return 1;
		}
	}

	inline unsigned int LuaImplQueryArg(const LuaState& state, int index, Vector3f* vec, TypeTag<Vector3f>)
	{
		Vector3d vecDouble;
		unsigned int ret = LuaImplQueryArg(state, index, &vecDouble, TypeTag<Vector3d>());

		vec->Set(vecDouble);
		return ret;
	}

	inline unsigned int LuaImplQueryArg(const LuaState& state, int index, Vector3ui* vec, TypeTag<Vector3ui>)
	{
		Vector3d vecDouble;
		unsigned int ret = LuaImplQueryArg(state, index, &vecDouble, TypeTag<Vector3d>());

		vec->Set(vecDouble);
		return ret;
	}

	inline unsigned int LuaImplQueryArg(const LuaState& state, int index, Vector3i* vec, TypeTag<Vector3i>)
	{
		Vector3d vecDouble;
		unsigned int ret = LuaImplQueryArg(state, index, &vecDouble, TypeTag<Vector3d>());

		vec->Set(vecDouble);
		return ret;
	}

	inline unsigned int LuaImplQueryArg(const LuaState& state, int index, Ndk::Entity** handle, TypeTag<Ndk::Entity*>)
	{
		if (!state.IsOfType(index, LuaType_Nil))
			*handle = *static_cast<Ndk::EntityHandle*>(state.CheckUserdata(index, "Entity"));
		else
			*handle = nullptr;

		return 1;
	}

	inline unsigned int LuaImplQueryArg(const LuaState& state, int index, Ndk::EntityHandle* handle, TypeTag<Ndk::EntityHandle>)
	{
		if (!state.IsOfType(index, LuaType_Nil))
			*handle = *static_cast<Ndk::EntityHandle*>(state.CheckUserdata(index, "Entity"));
		else
			handle->Reset();

		return 1;
	}

	inline unsigned int LuaImplQueryArg(const LuaState& state, int index, Ndk::WorldHandle* handle, TypeTag<Ndk::WorldHandle>)
	{
		*handle = *static_cast<Ndk::WorldHandle*>(state.CheckUserdata(index, "World"));

		return 1;
	}

#ifndef NDK_SERVER
	inline unsigned int LuaImplQueryArg(const LuaState& state, int index, InstancedRenderableRef* renderable, TypeTag<InstancedRenderableRef>)
	{
		if (state.IsOfType(index, "InstancedRenderable") ||
		    state.IsOfType(index, "Model") ||
		    state.IsOfType(index, "Sprite"))
		{
			*renderable = *static_cast<InstancedRenderableRef*>(state.ToUserdata(index));
		}
		else
			state.ArgError(index, "is not a InstancedRenderable instance");

		return 1;
	}

	inline unsigned int LuaImplQueryArg(const LuaState& state, int index, MaterialRef* materialRef, TypeTag<MaterialRef>)
	{
		*materialRef = *static_cast<MaterialRef*>(state.CheckUserdata(index, "Material"));

		return 1;
	}

	inline unsigned int LuaImplQueryArg(const LuaState& state, int index, MaterialParams* params, TypeTag<MaterialParams>)
	{
		state.CheckType(index, Nz::LuaType_Table);

		params->loadAlphaMap    = state.CheckField<bool>("LoadAlphaMap", params->loadAlphaMap);
		params->loadDiffuseMap  = state.CheckField<bool>("LoadDiffuseMap", params->loadDiffuseMap);
		params->loadEmissiveMap = state.CheckField<bool>("LoadEmissiveMap", params->loadEmissiveMap);
		params->loadHeightMap   = state.CheckField<bool>("LoadHeightMap", params->loadHeightMap);
		params->loadNormalMap   = state.CheckField<bool>("LoadNormalMap", params->loadNormalMap);
		params->loadSpecularMap = state.CheckField<bool>("LoadSpecularMap", params->loadSpecularMap);
		params->shaderName      = state.CheckField<String>("ShaderName", params->shaderName);

		return 1;
	}

	inline unsigned int LuaImplQueryArg(const LuaState& state, int index, ModelParameters* params, TypeTag<ModelParameters>)
	{
		state.CheckType(index, Nz::LuaType_Table);

		params->loadMaterials = state.CheckField<bool>("LoadMaterials", params->loadMaterials);

		LuaImplQueryArg(state, -1, &params->material, TypeTag<MaterialParams>());
		LuaImplQueryArg(state, -1, &params->mesh, TypeTag<MeshParams>());

		return 1;
	}

	inline unsigned int LuaImplQueryArg(const LuaState& state, int index, SoundBufferParams* params, TypeTag<SoundBufferParams>)
	{
		state.CheckType(index, Nz::LuaType_Table);

		params->forceMono = state.CheckField<bool>("ForceMono", params->forceMono);

		return 1;
	}

	inline unsigned int LuaImplQueryArg(const LuaState& state, int index, SoundStreamParams* params, TypeTag<SoundStreamParams>)
	{
		state.CheckType(index, Nz::LuaType_Table);

		params->forceMono = state.CheckField<bool>("ForceMono", params->forceMono);

		return 1;
	}

	inline unsigned int LuaImplQueryArg(const LuaState& state, int index, SpriteRef* spriteRef, TypeTag<SpriteRef>)
	{
		*spriteRef = *static_cast<SpriteRef*>(state.CheckUserdata(index, "Sprite"));

		return 1;
	}

	inline unsigned int LuaImplQueryArg(const LuaState& state, int index, TextureRef* textureRef, TypeTag<TextureRef>)
	{
		*textureRef = *static_cast<TextureRef*>(state.CheckUserdata(index, "Texture"));

		return 1;
	}
#endif

	inline int LuaImplReplyVal(const LuaState& state, Color&& val, TypeTag<Color>)
	{
		state.PushTable();
		state.PushField("r", val.r);
		state.PushField("g", val.g);
		state.PushField("b", val.b);
		state.PushField("a", val.a);

		return 1;
	}

	inline int LuaImplReplyVal(const LuaState& state, EulerAnglesd&& val, TypeTag<EulerAnglesd>)
	{
		state.PushInstance<EulerAnglesd>("EulerAngles", val);
		return 1;
	}

	inline int LuaImplReplyVal(const LuaState& state, EulerAnglesf&& val, TypeTag<EulerAnglesf>)
	{
		state.PushInstance<EulerAnglesd>("EulerAngles", val);
		return 1;
	}

	inline int LuaImplReplyVal(const LuaState& state, FontRef&& val, TypeTag<FontRef>)
	{
		state.PushInstance<FontRef>("Font", val);
		return 1;
	}

	inline int LuaImplReplyVal(const LuaState& state, Font::SizeInfo&& val, TypeTag<Font::SizeInfo>)
	{
		state.PushTable();
		state.PushField("LineHeight", val.lineHeight);
		state.PushField("SpaceAdvance", val.spaceAdvance);
		state.PushField("UnderlinePosition", val.underlinePosition);
		state.PushField("UnderlineThickness", val.underlineThickness);

		return 1;
	}

	inline int LuaImplReplyVal(const LuaState& state, ImageParams&& val, TypeTag<ImageParams>)
	{
		state.PushTable(0, 2);
			state.PushField("LevelCount", val.levelCount);
			state.PushField("LoadFormat", val.loadFormat);

		return 1;
	}

	inline int LuaImplReplyVal(const LuaState& state, IpAddress&& val, TypeTag<IpAddress>)
	{
		state.PushInstance<IpAddress>("IpAddress", val);
		return 1;
	}

	inline int LuaImplReplyVal(const LuaState& state, Matrix4d&& val, TypeTag<Matrix4d>)
	{
		state.PushInstance<Matrix4d>("Matrix4", val);
		return 1;
	}

	inline int LuaImplReplyVal(const LuaState& state, Matrix4f&& val, TypeTag<Matrix4f>)
	{
		state.PushInstance<Matrix4d>("Matrix4", val);
		return 1;
	}

	inline int LuaImplReplyVal(const LuaState& state, Quaterniond&& val, TypeTag<Quaterniond>)
	{
		state.PushInstance<Quaterniond>("Quaternion", val);
		return 1;
	}

	inline int LuaImplReplyVal(const LuaState& state, Quaternionf&& val, TypeTag<Quaternionf>)
	{
		state.PushInstance<Quaterniond>("Quaternion", val);
		return 1;
	}

	inline int LuaImplReplyVal(const LuaState& state, Rectd&& val, TypeTag<Rectd>)
	{
		state.PushInstance<Rectd>("Rect", val);
		return 1;
	}

	inline int LuaImplReplyVal(const LuaState& state, Rectf&& val, TypeTag<Rectf>)
	{
		state.PushInstance<Rectd>("Rect", val);
		return 1;
	}

	inline int LuaImplReplyVal(const LuaState& state, Recti&& val, TypeTag<Recti>)
	{
		state.PushInstance<Rectd>("Rect", val);
		return 1;
	}

	inline int LuaImplReplyVal(const LuaState& state, Rectui&& val, TypeTag<Rectui>)
	{
		state.PushInstance<Rectd>("Rect", val);
		return 1;
	}

	inline int LuaImplReplyVal(const LuaState& state, Vector2d&& val, TypeTag<Vector2d>)
	{
		state.PushInstance<Vector2d>("Vector2", val);
		return 1;
	}

	inline int LuaImplReplyVal(const LuaState& state, Vector2f&& val, TypeTag<Vector2f>)
	{
		state.PushInstance<Vector2d>("Vector2", val);
		return 1;
	}

	inline int LuaImplReplyVal(const LuaState& state, Vector2ui&& val, TypeTag<Vector2ui>)
	{
		state.PushInstance<Vector2d>("Vector2", val);
		return 1;
	}

	inline int LuaImplReplyVal(const LuaState& state, Vector2i&& val, TypeTag<Vector2i>)
	{
		state.PushInstance<Vector2d>("Vector2", val);
		return 1;
	}

	inline int LuaImplReplyVal(const LuaState& state, Vector3d&& val, TypeTag<Vector3d>)
	{
		state.PushInstance<Vector3d>("Vector3", val);
		return 1;
	}

	inline int LuaImplReplyVal(const LuaState& state, Vector3f&& val, TypeTag<Vector3f>)
	{
		state.PushInstance<Vector3d>("Vector3", val);
		return 1;
	}

	inline int LuaImplReplyVal(const LuaState& state, Vector3ui&& val, TypeTag<Vector3ui>)
	{
		state.PushInstance<Vector3d>("Vector3", val);
		return 1;
	}

	inline int LuaImplReplyVal(const LuaState& state, Vector3i&& val, TypeTag<Vector3i>)
	{
		state.PushInstance<Vector3d>("Vector3", val);
		return 1;
	}

	inline int LuaImplReplyVal(const LuaState& state, Ndk::Entity* ptr, TypeTag<Ndk::Entity*>)
	{
		state.PushInstance<Ndk::EntityHandle>("Entity", ptr);
		return 1;
	}

	inline int LuaImplReplyVal(const LuaState& state, Ndk::Application* ptr, TypeTag<Ndk::Application*>)
	{
		state.PushInstance<Ndk::Application*>("Application", ptr);
		return 1;
	}

	inline int LuaImplReplyVal(const LuaState& state, Ndk::EntityHandle&& handle, TypeTag<Ndk::EntityHandle>)
	{
		state.PushInstance<Ndk::EntityHandle>("Entity", handle);
		return 1;
	}

	inline int LuaImplReplyVal(const LuaState& state, Ndk::NodeComponentHandle&& handle, TypeTag<Ndk::NodeComponentHandle>)
	{
		state.PushInstance<Ndk::NodeComponentHandle>("NodeComponent", handle);
		return 1;
	}

	inline int LuaImplReplyVal(const LuaState& state, Ndk::VelocityComponentHandle&& handle, TypeTag<Ndk::VelocityComponentHandle>)
	{
		state.PushInstance<Ndk::VelocityComponentHandle>("VelocityComponent", handle);
		return 1;
	}

	inline int LuaImplReplyVal(const LuaState& state, Ndk::World* ptr, TypeTag<Ndk::World*>)
	{
		state.PushInstance<Ndk::WorldHandle>("World", ptr);
		return 1;
	}

	inline int LuaImplReplyVal(const LuaState& state, Ndk::WorldHandle&& handle, TypeTag<Ndk::WorldHandle>)
	{
		state.PushInstance<Ndk::WorldHandle>("World", handle);
		return 1;
	}

#ifndef NDK_SERVER
	inline int LuaImplReplyVal(const LuaState& state, MaterialRef&& handle, TypeTag<MaterialRef>)
	{
		state.PushInstance<MaterialRef>("Material", handle);
		return 1;
	}

	inline int LuaImplReplyVal(const LuaState& state, ModelRef&& handle, TypeTag<ModelRef>)
	{
		state.PushInstance<ModelRef>("Model", handle);
		return 1;
	}

	inline int LuaImplReplyVal(const LuaState& state, const SoundBuffer* val, TypeTag<const SoundBuffer*>)
	{
		state.PushInstance<SoundBufferConstRef>("SoundBuffer", val);
		return 1;
	}

	inline int LuaImplReplyVal(const LuaState& state, SoundBufferRef&& handle, TypeTag<SoundBufferRef>)
	{
		state.PushInstance<SoundBufferRef>("SoundBuffer", handle);
		return 1;
	}

	inline int LuaImplReplyVal(const LuaState& state, SpriteRef&& handle, TypeTag<SpriteRef>)
	{
		state.PushInstance<SpriteRef>("Sprite", handle);
		return 1;
	}

	inline int LuaImplReplyVal(const LuaState& state, TextureRef&& handle, TypeTag<TextureRef>)
	{
		state.PushInstance<TextureRef>("Texture", handle);
		return 1;
	}

	inline int LuaImplReplyVal(const LuaState& state, Ndk::CameraComponentHandle&& handle, TypeTag<Ndk::CameraComponentHandle>)
	{
		state.PushInstance<Ndk::CameraComponentHandle>("CameraComponent", handle);
		return 1;
	}

	inline int LuaImplReplyVal(const LuaState& state, Ndk::ConsoleHandle&& handle, TypeTag<Ndk::ConsoleHandle>)
	{
		state.PushInstance<Ndk::ConsoleHandle>("Console", handle);
		return 1;
	}

	inline int LuaImplReplyVal(const LuaState& state, Ndk::GraphicsComponentHandle&& handle, TypeTag<Ndk::GraphicsComponentHandle>)
	{
		state.PushInstance<Ndk::GraphicsComponentHandle>("GraphicsComponent", handle);
		return 1;
	}
#endif

}
