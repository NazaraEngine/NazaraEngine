// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

#include <Nazara/Audio/SoundBuffer.hpp>
#include <Nazara/Lua/LuaInstance.hpp>
#include <Nazara/Math/Vector3.hpp>
#include <algorithm>

namespace Nz
{
	inline unsigned int LuaImplQueryArg(const LuaInstance& instance, int index, SoundBufferParams* params, TypeTag<SoundBufferParams>)
	{
		instance.CheckType(index, Nz::LuaType_Table);

		params->forceMono = instance.CheckField<bool>("ForceMono", params->forceMono);

		return 1;
	}

	inline unsigned int LuaImplQueryArg(const LuaInstance& lua, int index, Vector3d* vec, TypeTag<Vector3d>)
	{
		switch (lua.GetType(index))
		{
			case Nz::LuaType_Number:
				if (index < 0 && index > -3)
					lua.Error("Vector3 expected, three numbers are required to convert it");

				vec->Set(lua.CheckNumber(index), lua.CheckNumber(index + 1), lua.CheckNumber(index + 2));
				return 3;

			case Nz::LuaType_Table:
				vec->Set(lua.CheckField<double>("x", index), lua.CheckField<double>("y", index), lua.CheckField<double>("z", index));
				return 3;

			default:
				vec->Set(*(*static_cast<Vector3d**>(lua.CheckUserdata(index, "Vector3"))));
				return 1;
		}
	}

	inline unsigned int LuaImplQueryArg(const LuaInstance& lua, int index, Vector3f* vec, TypeTag<Vector3f>)
	{
		Vector3d vecDouble;
		unsigned int ret = LuaImplQueryArg(lua, index, &vecDouble, TypeTag<Vector3d>());

		vec->Set(vecDouble);
		return ret;
	}

	inline unsigned int LuaImplQueryArg(const LuaInstance& lua, int index, Vector3ui* vec, TypeTag<Vector3ui>)
	{
		Vector3d vecDouble;
		unsigned int ret = LuaImplQueryArg(lua, index, &vecDouble, TypeTag<Vector3d>());

		vec->Set(vecDouble);
		return ret;
	}


	inline int LuaImplReplyVal(const LuaInstance& instance, const SoundBuffer* val, TypeTag<const SoundBuffer*>)
	{
		instance.PushInstance<SoundBufferConstRef>("SoundBuffer", val);
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
}
