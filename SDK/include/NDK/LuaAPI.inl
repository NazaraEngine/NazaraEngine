// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

#include <Nazara/Core/Color.hpp>
#include <Nazara/Lua/LuaInstance.hpp>
#include <Nazara/Math/EulerAngles.hpp>
#include <Nazara/Math/Quaternion.hpp>
#include <Nazara/Math/Vector3.hpp>
#include <Nazara/Network/IpAddress.hpp>
#include <NDK/Components.hpp>
#include <NDK/Entity.hpp>
#include <NDK/World.hpp>
#include <algorithm>

#ifndef NDK_SERVER
#include <Nazara/Audio/SoundBuffer.hpp>
#include <Nazara/Graphics/Model.hpp>
#endif

namespace Nz
{
	inline unsigned int LuaImplQueryArg(const LuaInstance& instance, int index, Color* color, TypeTag<Color>)
	{
		instance.CheckType(index, Nz::LuaType_Table);

		color->r = instance.CheckField<UInt8>("r", index);
		color->g = instance.CheckField<UInt8>("g", index);
		color->b = instance.CheckField<UInt8>("b", index);
		color->a = instance.CheckField<UInt8>("a", 255, index);

		return 1;
	}

	inline unsigned int LuaImplQueryArg(const LuaInstance& instance, int index, EulerAnglesd* angles, TypeTag<EulerAnglesd>)
	{
		switch (instance.GetType(index))
		{
			case Nz::LuaType_Table:
				angles->Set(instance.CheckField<double>("pitch", index), instance.CheckField<double>("yaw", index), instance.CheckField<double>("roll", index));
				return 1;

			default:
			{
				if (instance.IsOfType(index, "EulerAngles"))
					angles->Set(*(*static_cast<EulerAnglesd**>(instance.ToUserdata(index))));
				else
					angles->Set(*(*static_cast<Quaterniond**>(instance.CheckUserdata(index, "Quaternion"))));

				return 1;
			}
		}
	}

	inline unsigned int LuaImplQueryArg(const LuaInstance& instance, int index, EulerAnglesf* angles, TypeTag<EulerAnglesf>)
	{
		EulerAnglesd anglesDouble;
		unsigned int ret = LuaImplQueryArg(instance, index, &anglesDouble, TypeTag<EulerAnglesd>());

		angles->Set(anglesDouble);
		return ret;
	}

	inline unsigned int LuaImplQueryArg(const LuaInstance& instance, int index, Quaterniond* quat, TypeTag<Quaterniond>)
	{
		switch (instance.GetType(index))
		{
			case Nz::LuaType_Table:
				quat->Set(instance.CheckField<double>("w", index), instance.CheckField<double>("x", index), instance.CheckField<double>("y", index), instance.CheckField<double>("z", index));
				return 1;

			default:
			{
				if (instance.IsOfType(index, "EulerAngles"))
					quat->Set(*(*static_cast<EulerAnglesd**>(instance.ToUserdata(index))));
				else
					quat->Set(*(*static_cast<Quaterniond**>(instance.CheckUserdata(index, "Quaternion"))));

				return 1;
			}
		}
	}

	inline unsigned int LuaImplQueryArg(const LuaInstance& instance, int index, Quaternionf* quat, TypeTag<Quaternionf>)
	{
		Quaterniond quatDouble;
		unsigned int ret = LuaImplQueryArg(instance, index, &quatDouble, TypeTag<Quaterniond>());

		quat->Set(quatDouble);
		return ret;
	}

	inline unsigned int LuaImplQueryArg(const LuaInstance& instance, int index, IpAddress* address, TypeTag<IpAddress>)
	{
		switch (instance.GetType(index))
		{
			case Nz::LuaType_String:
				address->BuildFromAddress(instance.CheckString(index));
				return 1;

			default:
				*address = *(*static_cast<IpAddress**>(instance.CheckUserdata(index, "IpAddress")));
				return 1;
		}
	}

	inline unsigned int LuaImplQueryArg(const LuaInstance& instance, int index, Vector2d* vec, TypeTag<Vector2d>)
	{
		switch (instance.GetType(index))
		{
			case Nz::LuaType_Number:
				if (index < 0 && index > -2)
					instance.Error("Vector2 expected, two numbers are required to convert it");

				vec->Set(instance.CheckNumber(index), instance.CheckNumber(index + 1));
				return 2;

			case Nz::LuaType_Table:
				vec->Set(instance.CheckField<double>("x", index), instance.CheckField<double>("y", index));
				return 1;

			default:
				vec->Set(*(*static_cast<Vector2d**>(instance.CheckUserdata(index, "Vector2"))));
				return 1;
		}
	}

	inline unsigned int LuaImplQueryArg(const LuaInstance& instance, int index, Vector2f* vec, TypeTag<Vector2f>)
	{
		Vector2d vecDouble;
		unsigned int ret = LuaImplQueryArg(instance, index, &vecDouble, TypeTag<Vector2d>());

		vec->Set(vecDouble);
		return ret;
	}

	inline unsigned int LuaImplQueryArg(const LuaInstance& instance, int index, Vector2ui* vec, TypeTag<Vector2ui>)
	{
		Vector2d vecDouble;
		unsigned int ret = LuaImplQueryArg(instance, index, &vecDouble, TypeTag<Vector2d>());

		vec->Set(vecDouble);
		return ret;
	}

	inline unsigned int LuaImplQueryArg(const LuaInstance& instance, int index, Vector3d* vec, TypeTag<Vector3d>)
	{
		switch (instance.GetType(index))
		{
			case Nz::LuaType_Number:
				if (index < 0 && index > -3)
					instance.Error("Vector3 expected, three numbers are required to convert it");

				vec->Set(instance.CheckNumber(index), instance.CheckNumber(index + 1), instance.CheckNumber(index + 2));
				return 3;

			case Nz::LuaType_Table:
				vec->Set(instance.CheckField<double>("x", index), instance.CheckField<double>("y", index), instance.CheckField<double>("z", index));
				return 1;

			default:
				vec->Set(*(*static_cast<Vector3d**>(instance.CheckUserdata(index, "Vector3"))));
				return 1;
		}
	}

	inline unsigned int LuaImplQueryArg(const LuaInstance& instance, int index, Vector3f* vec, TypeTag<Vector3f>)
	{
		Vector3d vecDouble;
		unsigned int ret = LuaImplQueryArg(instance, index, &vecDouble, TypeTag<Vector3d>());

		vec->Set(vecDouble);
		return ret;
	}

	inline unsigned int LuaImplQueryArg(const LuaInstance& instance, int index, Vector3ui* vec, TypeTag<Vector3ui>)
	{
		Vector3d vecDouble;
		unsigned int ret = LuaImplQueryArg(instance, index, &vecDouble, TypeTag<Vector3d>());

		vec->Set(vecDouble);
		return ret;
	}

#ifndef NDK_SERVER
	inline unsigned int LuaImplQueryArg(const LuaInstance& instance, int index, InstancedRenderableRef* renderable, TypeTag<InstancedRenderableRef>)
	{
		if (instance.IsOfType(index, "InstancedRenderable"))
			*renderable = *(*static_cast<InstancedRenderableRef**>(instance.CheckUserdata(index, "InstancedRenderable")));
		else
			*renderable = *(*static_cast<InstancedRenderableRef**>(instance.CheckUserdata(index, "Model")));
		return 1;
	}

	inline unsigned int LuaImplQueryArg(const LuaInstance& instance, int index, MaterialParams* params, TypeTag<MaterialParams>)
	{
		instance.CheckType(index, Nz::LuaType_Table);

		params->loadAlphaMap    = instance.CheckField<bool>("LoadAlphaMap", params->loadAlphaMap);
		params->loadDiffuseMap  = instance.CheckField<bool>("LoadDiffuseMap", params->loadDiffuseMap);
		params->loadEmissiveMap = instance.CheckField<bool>("LoadEmissiveMap", params->loadEmissiveMap);
		params->loadHeightMap   = instance.CheckField<bool>("LoadHeightMap", params->loadHeightMap);
		params->loadNormalMap   = instance.CheckField<bool>("LoadNormalMap", params->loadNormalMap);
		params->loadSpecularMap = instance.CheckField<bool>("LoadSpecularMap", params->loadSpecularMap);

		return 1;
	}

	inline unsigned int LuaImplQueryArg(const LuaInstance& instance, int index, MeshParams* params, TypeTag<MeshParams>)
	{
		instance.CheckType(index, Nz::LuaType_Table);

		params->animated             = instance.CheckField<bool>("Animated", params->animated);
		params->center               = instance.CheckField<bool>("Center", params->center);
		params->flipUVs              = instance.CheckField<bool>("FlipUVs", params->flipUVs);
		params->optimizeIndexBuffers = instance.CheckField<bool>("OptimizeIndexBuffers", params->optimizeIndexBuffers);
		params->scale                = instance.CheckField<Vector3f>("Scale", params->scale);

		return 1;
	}

	inline unsigned int LuaImplQueryArg(const LuaInstance& instance, int index, ModelParameters* params, TypeTag<ModelParameters>)
	{
		instance.CheckType(index, Nz::LuaType_Table);

		params->loadMaterials = instance.CheckField<bool>("LoadMaterials", params->loadMaterials);

		LuaImplQueryArg(instance, -1, &params->material, TypeTag<MaterialParams>());
		LuaImplQueryArg(instance, -1, &params->mesh, TypeTag<MeshParams>());

		return 1;
	}

	inline unsigned int LuaImplQueryArg(const LuaInstance& instance, int index, SoundBufferParams* params, TypeTag<SoundBufferParams>)
	{
		instance.CheckType(index, Nz::LuaType_Table);

		params->forceMono = instance.CheckField<bool>("ForceMono", params->forceMono);

		return 1;
	}
#endif



	inline int LuaImplReplyVal(const LuaInstance& instance, EulerAnglesd val, TypeTag<EulerAnglesd>)
	{
		instance.PushInstance<EulerAnglesd>("EulerAngles", val);
		return 1;
	}

	inline int LuaImplReplyVal(const LuaInstance& instance, EulerAnglesf val, TypeTag<EulerAnglesf>)
	{
		instance.PushInstance<EulerAnglesd>("EulerAngles", val);
		return 1;
	}

	inline int LuaImplReplyVal(const LuaInstance& instance, Quaterniond val, TypeTag<Quaterniond>)
	{
		instance.PushInstance<Quaterniond>("Quaternion", val);
		return 1;
	}

	inline int LuaImplReplyVal(const LuaInstance& instance, Quaternionf val, TypeTag<Quaternionf>)
	{
		instance.PushInstance<Quaterniond>("Quaternion", val);
		return 1;
	}

	inline int LuaImplReplyVal(const LuaInstance& instance, IpAddress val, TypeTag<IpAddress>)
	{
		instance.PushInstance<IpAddress>("IpAddress", val);
		return 1;
	}

	inline int LuaImplReplyVal(const LuaInstance& instance, Vector3d val, TypeTag<Vector3d>)
	{
		instance.PushInstance<Vector3d>("Vector3", val);
		return 1;
	}

	inline int LuaImplReplyVal(const LuaInstance& instance, Vector3f val, TypeTag<Vector3f>)
	{
		instance.PushInstance<Vector3d>("Vector3", val);
		return 1;
	}

	inline int LuaImplReplyVal(const LuaInstance& instance, Vector3ui val, TypeTag<Vector3ui>)
	{
		instance.PushInstance<Vector3d>("Vector3", val);
		return 1;
	}

	inline int LuaImplReplyVal(const LuaInstance& instance, Ndk::Entity* ptr, TypeTag<Ndk::Entity*>)
	{
		instance.PushInstance<Ndk::EntityHandle>("Entity", ptr);
		return 1;
	}

	inline int LuaImplReplyVal(const LuaInstance& instance, Ndk::EntityHandle handle, TypeTag<Ndk::EntityHandle>)
	{
		instance.PushInstance<Ndk::EntityHandle>("Entity", handle);
		return 1;
	}

	inline int LuaImplReplyVal(const LuaInstance& instance, Ndk::NodeComponentHandle handle, TypeTag<Ndk::NodeComponentHandle>)
	{
		instance.PushInstance<Ndk::NodeComponentHandle>("NodeComponent", handle);
		return 1;
	}

	inline int LuaImplReplyVal(const LuaInstance& instance, Ndk::World* ptr, TypeTag<Ndk::World*>)
	{
		instance.PushInstance<Ndk::WorldHandle>("World", ptr);
		return 1;
	}

	inline int LuaImplReplyVal(const LuaInstance& instance, Ndk::WorldHandle handle, TypeTag<Ndk::WorldHandle>)
	{
		instance.PushInstance<Ndk::WorldHandle>("World", handle);
		return 1;
	}

#ifndef NDK_SERVER
	inline int LuaImplReplyVal(const LuaInstance& instance, Ndk::GraphicsComponentHandle handle, TypeTag<Ndk::GraphicsComponentHandle>)
	{
		instance.PushInstance<Ndk::GraphicsComponentHandle>("GraphicsComponent", handle);
		return 1;
	}

	inline int LuaImplReplyVal(const LuaInstance& instance, const SoundBuffer* val, TypeTag<const SoundBuffer*>)
	{
		instance.PushInstance<SoundBufferConstRef>("SoundBuffer", val);
		return 1;
	}
#endif
}
