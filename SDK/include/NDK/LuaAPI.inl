// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

#include <NDK/LuaAPI.hpp>
#include <Nazara/Core/Color.hpp>
#include <Nazara/Lua/LuaInstance.hpp>
#include <Nazara/Math/EulerAngles.hpp>
#include <Nazara/Math/Quaternion.hpp>
#include <Nazara/Math/Vector3.hpp>
#include <Nazara/Network/IpAddress.hpp>
#include <Nazara/Utility/Font.hpp>
#include <Nazara/Utility/Mesh.hpp>
#include <NDK/Application.hpp>
#include <NDK/Components.hpp>
#include <NDK/Entity.hpp>
#include <NDK/World.hpp>
#include <algorithm>

#ifndef NDK_SERVER
#include <Nazara/Audio/Music.hpp>
#include <Nazara/Audio/SoundBuffer.hpp>
#include <Nazara/Graphics/Model.hpp>
#include <NDK/Console.hpp>
#endif

namespace Ndk
{
	/*!
	* \brief Gets the internal binding for Lua
	* \return A pointer to the binding
	*/

	inline LuaBinding* LuaAPI::GetBinding()
	{
		return s_binding;
	}
}

namespace Nz
{
	/*!
	* \brief Queries arguments for Lua
	* \return 1 in case of success
	*
	* \param instance Lua instance to interact with
	* \param index Index type
	* \param color Resulting color
	*/

	inline unsigned int LuaImplQueryArg(const LuaInstance& instance, int index, Color* color, TypeTag<Color>)
	{
		instance.CheckType(index, Nz::LuaType_Table);

		color->r = instance.CheckField<UInt8>("r", index);
		color->g = instance.CheckField<UInt8>("g", index);
		color->b = instance.CheckField<UInt8>("b", index);
		color->a = instance.CheckField<UInt8>("a", 255, index);

		return 1;
	}

	/*!
	* \brief Queries arguments for Lua
	* \return 1 in case of success
	*
	* \param instance Lua instance to interact with
	* \param index Index type
	* \param angles Resulting euler angles
	*/

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
					angles->Set(*static_cast<EulerAnglesd*>(instance.ToUserdata(index)));
				else
					angles->Set(*static_cast<Quaterniond*>(instance.CheckUserdata(index, "Quaternion")));

				return 1;
			}
		}
	}

	/*!
	* \brief Queries arguments for Lua
	* \return 1 in case of success
	*
	* \param instance Lua instance to interact with
	* \param index Index type
	* \param angles Resulting euler angles
	*/

	inline unsigned int LuaImplQueryArg(const LuaInstance& instance, int index, EulerAnglesf* angles, TypeTag<EulerAnglesf>)
	{
		EulerAnglesd anglesDouble;
		unsigned int ret = LuaImplQueryArg(instance, index, &anglesDouble, TypeTag<EulerAnglesd>());

		angles->Set(anglesDouble);
		return ret;
	}

	/*!
	* \brief Queries arguments for Lua
	* \return 1 in case of success
	*
	* \param instance Lua instance to interact with
	* \param index Index type
	* \param fontRef Resulting reference to a font
	*/

	inline unsigned int LuaImplQueryArg(const LuaInstance& instance, int index, FontRef* fontRef, TypeTag<FontRef>)
	{
		*fontRef = *static_cast<FontRef*>(instance.CheckUserdata(index, "Font"));

		return 1;
	}

	/*!
	* \brief Queries arguments for Lua
	* \return 1 in case of success
	*
	* \param instance Lua instance to interact with
	* \param index Index type
	* \param params Resulting parameters for a font
	*/

	inline unsigned int LuaImplQueryArg(const LuaInstance& instance, int index, FontParams* params, TypeTag<FontParams>)
	{
		NazaraUnused(params);

		instance.CheckType(index, Nz::LuaType_Table);

		// Structure is empty for now

		return 1;
	}

	/*!
	* \brief Queries arguments for Lua
	* \return 1 in case of success
	*
	* \param instance Lua instance to interact with
	* \param index Index type
	* \param params Resulting parameters for an image
	*/
	inline unsigned int LuaImplQueryArg(const LuaInstance& instance, int index, ImageParams* params, TypeTag<ImageParams>)
	{
		instance.CheckType(index, Nz::LuaType_Table);

		params->levelCount = instance.CheckField<Nz::UInt8>("LevelCount");
		params->loadFormat = instance.CheckField<Nz::PixelFormatType>("LoadFormat");

		return 1;
	}

	/*!
	* \brief Queries arguments for Lua
	* \return 1 in case of success
	*
	* \param instance Lua instance to interact with
	* \param index Index type
	* \param address Resulting IP address
	*/

	inline unsigned int LuaImplQueryArg(const LuaInstance& instance, int index, IpAddress* address, TypeTag<IpAddress>)
	{
		switch (instance.GetType(index))
		{
			case Nz::LuaType_String:
				address->BuildFromAddress(instance.CheckString(index));
				return 1;

			default:
				*address = *static_cast<IpAddress*>(instance.CheckUserdata(index, "IpAddress"));
				return 1;
		}
	}

	/*!
	* \brief Queries arguments for Lua
	* \return 1 in case of success
	*
	* \param instance Lua instance to interact with
	* \param index Index type
	* \param quat Resulting quaternion
	*/

	inline unsigned int LuaImplQueryArg(const LuaInstance& instance, int index, Matrix4d* mat, TypeTag<Matrix4d>)
	{
		switch (instance.GetType(index))
		{
			case Nz::LuaType_Table:
			{
				double values[16];
				for (std::size_t i = 0; i < 16; ++i)
				{
					instance.PushInteger(i + 1);
					instance.GetTable();

					values[i] = instance.CheckNumber(-1);
					instance.Pop();
				}

				mat->Set(values);
				return 1;
			}

			default:
			{
				if (instance.IsOfType(index, "Matrix4"))
					mat->Set(*static_cast<Matrix4d*>(instance.ToUserdata(index)));

				return 1;
			}
		}
	}

	/*!
	* \brief Queries arguments for Lua
	* \return 1 in case of success
	*
	* \param instance Lua instance to interact with
	* \param index Index type
	* \param quat Resulting quaternion
	*/

	inline unsigned int LuaImplQueryArg(const LuaInstance& instance, int index, Matrix4f* mat, TypeTag<Matrix4f>)
	{
		Matrix4d matDouble;
		unsigned int ret = LuaImplQueryArg(instance, index, &matDouble, TypeTag<Matrix4d>());

		mat->Set(matDouble);
		return ret;
	}

	/*!
	* \brief Queries arguments for Lua
	* \return 1 in case of success
	*
	* \param instance Lua instance to interact with
	* \param index Index type
	* \param params Resulting parameters for a mesh
	*/

	inline unsigned int LuaImplQueryArg(const LuaInstance& instance, int index, MeshParams* params, TypeTag<MeshParams>)
	{
		instance.CheckType(index, Nz::LuaType_Table);

		params->animated = instance.CheckField<bool>("Animated", params->animated);
		params->center = instance.CheckField<bool>("Center", params->center);
		params->flipUVs = instance.CheckField<bool>("FlipUVs", params->flipUVs);
		//params->matrix = instance.CheckField<Matrix4f>("Matrix", params->matrix);
		params->optimizeIndexBuffers = instance.CheckField<bool>("OptimizeIndexBuffers", params->optimizeIndexBuffers);

		return 1;
	}

	/*!
	* \brief Queries arguments for Lua
	* \return 1 in case of success
	*
	* \param instance Lua instance to interact with
	* \param index Index type
	* \param quat Resulting quaternion
	*/

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
					quat->Set(*static_cast<EulerAnglesd*>(instance.ToUserdata(index)));
				else
					quat->Set(*static_cast<Quaterniond*>(instance.CheckUserdata(index, "Quaternion")));

				return 1;
			}
		}
	}

	/*!
	* \brief Queries arguments for Lua
	* \return 1 in case of success
	*
	* \param instance Lua instance to interact with
	* \param index Index type
	* \param quat Resulting quaternion
	*/

	inline unsigned int LuaImplQueryArg(const LuaInstance& instance, int index, Quaternionf* quat, TypeTag<Quaternionf>)
	{
		Quaterniond quatDouble;
		unsigned int ret = LuaImplQueryArg(instance, index, &quatDouble, TypeTag<Quaterniond>());

		quat->Set(quatDouble);
		return ret;
	}

	/*!
	* \brief Queries arguments for Lua
	* \return 1 in case of success
	*
	* \param instance Lua instance to interact with
	* \param index Index type
	* \param rect Resulting rectangle
	*/

	inline unsigned int LuaImplQueryArg(const LuaInstance& instance, int index, Rectd* rect, TypeTag<Rectd>)
	{
		instance.CheckType(index, LuaType_Table);

		rect->x      = instance.CheckField<double>("x", index);
		rect->y      = instance.CheckField<double>("y", index);
		rect->width  = instance.CheckField<double>("width", index);
		rect->height = instance.CheckField<double>("height", index);

		return 1;
	}

	/*!
	* \brief Queries arguments for Lua
	* \return 1 in case of success
	*
	* \param instance Lua instance to interact with
	* \param index Index type
	* \param rect Resulting rectangle
	*/

	inline unsigned int LuaImplQueryArg(const LuaInstance& instance, int index, Rectf* rect, TypeTag<Rectf>)
	{
		Rectd rectDouble;
		unsigned int ret = LuaImplQueryArg(instance, index, &rectDouble, TypeTag<Rectd>());

		rect->Set(rectDouble);
		return ret;
	}

	/*!
	* \brief Queries arguments for Lua
	* \return 1 in case of success
	*
	* \param instance Lua instance to interact with
	* \param index Index type
	* \param rect Resulting rectangle
	*/

	inline unsigned int LuaImplQueryArg(const LuaInstance& instance, int index, Rectui* rect, TypeTag<Rectui>)
	{
		Rectd rectDouble;
		unsigned int ret = LuaImplQueryArg(instance, index, &rectDouble, TypeTag<Rectd>());

		rect->Set(rectDouble);
		return ret;
	}

	/*!
	* \brief Queries arguments for Lua
	* \return 1 in case of success
	*
	* \param instance Lua instance to interact with
	* \param index Index type
	* \param vec Resulting vector2D
	*/

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
				vec->Set(*static_cast<Vector2d*>(instance.CheckUserdata(index, "Vector2")));
				return 1;
		}
	}

	/*!
	* \brief Queries arguments for Lua
	* \return 1 in case of success
	*
	* \param instance Lua instance to interact with
	* \param index Index type
	* \param vec Resulting vector2D
	*/

	inline unsigned int LuaImplQueryArg(const LuaInstance& instance, int index, Vector2f* vec, TypeTag<Vector2f>)
	{
		Vector2d vecDouble;
		unsigned int ret = LuaImplQueryArg(instance, index, &vecDouble, TypeTag<Vector2d>());

		vec->Set(vecDouble);
		return ret;
	}

	/*!
	* \brief Queries arguments for Lua
	* \return 1 in case of success
	*
	* \param instance Lua instance to interact with
	* \param index Index type
	* \param vec Resulting vector2D
	*/

	inline unsigned int LuaImplQueryArg(const LuaInstance& instance, int index, Vector2ui* vec, TypeTag<Vector2ui>)
	{
		Vector2d vecDouble;
		unsigned int ret = LuaImplQueryArg(instance, index, &vecDouble, TypeTag<Vector2d>());

		vec->Set(vecDouble);
		return ret;
	}

	/*!
	* \brief Queries arguments for Lua
	* \return 1 in case of success
	*
	* \param instance Lua instance to interact with
	* \param index Index type
	* \param vec Resulting vector3D
	*/

	inline unsigned int LuaImplQueryArg(const LuaInstance& instance, int index, Vector3d* vec, TypeTag<Vector3d>)
	{
		switch (instance.GetType(index))
		{
			case Nz::LuaType_Number:
				if (index < 0 && index > -3)
					instance.Error("Vector3 expected, three numbers are required to convert it");

				vec->Set(instance.CheckNumber(index), instance.CheckNumber(index + 1), instance.CheckNumber(index + 2, 0.0));
				return 3;

			case Nz::LuaType_Table:
				vec->Set(instance.CheckField<double>("x", index), instance.CheckField<double>("y", index), instance.CheckField<double>("z", 0.0, index));
				return 1;

			default:
				vec->Set(*static_cast<Vector3d*>(instance.CheckUserdata(index, "Vector3")));
				return 1;
		}
	}

	/*!
	* \brief Queries arguments for Lua
	* \return 1 in case of success
	*
	* \param instance Lua instance to interact with
	* \param index Index type
	* \param vec Resulting vector3D
	*/

	inline unsigned int LuaImplQueryArg(const LuaInstance& instance, int index, Vector3f* vec, TypeTag<Vector3f>)
	{
		Vector3d vecDouble;
		unsigned int ret = LuaImplQueryArg(instance, index, &vecDouble, TypeTag<Vector3d>());

		vec->Set(vecDouble);
		return ret;
	}

	/*!
	* \brief Queries arguments for Lua
	* \return 1 in case of success
	*
	* \param instance Lua instance to interact with
	* \param index Index type
	* \param vec Resulting vector3D
	*/

	inline unsigned int LuaImplQueryArg(const LuaInstance& instance, int index, Vector3ui* vec, TypeTag<Vector3ui>)
	{
		Vector3d vecDouble;
		unsigned int ret = LuaImplQueryArg(instance, index, &vecDouble, TypeTag<Vector3d>());

		vec->Set(vecDouble);
		return ret;
	}

	/*!
	* \brief Queries arguments for Lua
	* \return 1 in case of success
	*
	* \param instance Lua instance to interact with
	* \param index Index type
	* \param handle Resulting entity
	*/

	inline unsigned int LuaImplQueryArg(const LuaInstance& instance, int index, Ndk::EntityHandle* handle, TypeTag<Ndk::EntityHandle>)
	{
		*handle = *static_cast<Ndk::EntityHandle*>(instance.CheckUserdata(index, "Entity"));

		return 1;
	}

	/*!
	* \brief Queries arguments for Lua
	* \return 1 in case of success
	*
	* \param instance Lua instance to interact with
	* \param index Index type
	* \param handle Resulting world
	*/

	inline unsigned int LuaImplQueryArg(const LuaInstance& instance, int index, Ndk::WorldHandle* handle, TypeTag<Ndk::WorldHandle>)
	{
		*handle = *static_cast<Ndk::WorldHandle*>(instance.CheckUserdata(index, "World"));

		return 1;
	}

#ifndef NDK_SERVER

	/*!
	* \brief Queries arguments for Lua
	* \return 1 in case of success
	*
	* \param instance Lua instance to interact with
	* \param index Index type
	* \param renderable Resulting reference to a instanced renderable
	*/

	inline unsigned int LuaImplQueryArg(const LuaInstance& instance, int index, InstancedRenderableRef* renderable, TypeTag<InstancedRenderableRef>)
	{
		if (instance.IsOfType(index, "InstancedRenderable") ||
		    instance.IsOfType(index, "Model") ||
		    instance.IsOfType(index, "Sprite"))
		{
			*renderable = *static_cast<InstancedRenderableRef*>(instance.ToUserdata(index));
		}
		else
			instance.ArgError(index, "is not a InstancedRenderable instance");

		return 1;
	}

	/*!
	* \brief Queries arguments for Lua
	* \return 1 in case of success
	*
	* \param instance Lua instance to interact with
	* \param index Index type
	* \param params Resulting parameters for a material
	*/

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

	/*!
	* \brief Queries arguments for Lua
	* \return 1 in case of success
	*
	* \param instance Lua instance to interact with
	* \param index Index type
	* \param params Resulting parameters for a model
	*/

	inline unsigned int LuaImplQueryArg(const LuaInstance& instance, int index, ModelParameters* params, TypeTag<ModelParameters>)
	{
		instance.CheckType(index, Nz::LuaType_Table);

		params->loadMaterials = instance.CheckField<bool>("LoadMaterials", params->loadMaterials);

		LuaImplQueryArg(instance, -1, &params->material, TypeTag<MaterialParams>());
		LuaImplQueryArg(instance, -1, &params->mesh, TypeTag<MeshParams>());

		return 1;
	}

	/*!
	* \brief Queries arguments for Lua
	* \return 1 in case of success
	*
	* \param instance Lua instance to interact with
	* \param index Index type
	* \param params Resulting parameters for a music
	*/

	inline unsigned int LuaImplQueryArg(const LuaInstance& instance, int index, MusicParams* params, TypeTag<MusicParams>)
	{
		instance.CheckType(index, Nz::LuaType_Table);

		params->forceMono = instance.CheckField<bool>("ForceMono", params->forceMono);

		return 1;
	}

	/*!
	* \brief Queries arguments for Lua
	* \return 1 in case of success
	*
	* \param instance Lua instance to interact with
	* \param index Index type
	* \param params Resulting parameters for a sound buffer
	*/

	inline unsigned int LuaImplQueryArg(const LuaInstance& instance, int index, SoundBufferParams* params, TypeTag<SoundBufferParams>)
	{
		instance.CheckType(index, Nz::LuaType_Table);

		params->forceMono = instance.CheckField<bool>("ForceMono", params->forceMono);

		return 1;
	}

	/*!
	* \brief Queries arguments for Lua
	* \return 1 in case of success
	*
	* \param instance Lua instance to interact with
	* \param index Index type
	* \param fontRef Resulting reference to a font
	*/

	inline unsigned int LuaImplQueryArg(const LuaInstance& instance, int index, TextureRef* textureRef, TypeTag<TextureRef>)
	{
		*textureRef = *static_cast<TextureRef*>(instance.CheckUserdata(index, "Texture"));

		return 1;
	}

#endif

	/*!
	* \brief Replies by value for Lua
	* \return 1 in case of success
	*
	* \param instance Lua instance to interact with
	* \param val Resulting color
	*/

	inline int LuaImplReplyVal(const LuaInstance& instance, Color&& val, TypeTag<Color>)
	{
		instance.PushTable();
		instance.PushField("r", val.r);
		instance.PushField("g", val.g);
		instance.PushField("b", val.b);
		instance.PushField("a", val.a);

		return 1;
	}

	/*!
	* \brief Replies by value for Lua
	* \return 1 in case of success
	*
	* \param instance Lua instance to interact with
	* \param val Resulting euler angles
	*/

	inline int LuaImplReplyVal(const LuaInstance& instance, EulerAnglesd&& val, TypeTag<EulerAnglesd>)
	{
		instance.PushInstance<EulerAnglesd>("EulerAngles", val);
		return 1;
	}

	/*!
	* \brief Replies by value for Lua
	* \return 1 in case of success
	*
	* \param instance Lua instance to interact with
	* \param val Resulting euler angles
	*/

	inline int LuaImplReplyVal(const LuaInstance& instance, EulerAnglesf&& val, TypeTag<EulerAnglesf>)
	{
		instance.PushInstance<EulerAnglesd>("EulerAngles", val);
		return 1;
	}

	/*!
	* \brief Replies by value for Lua
	* \return 1 in case of success
	*
	* \param instance Lua instance to interact with
	* \param val Resulting reference to a font
	*/

	inline int LuaImplReplyVal(const LuaInstance& instance, FontRef&& val, TypeTag<FontRef>)
	{
		instance.PushInstance<FontRef>("Font", val);
		return 1;
	}

	/*!
	* \brief Replies by value for Lua
	* \return 1 in case of success
	*
	* \param instance Lua instance to interact with
	* \param val Resulting size information for a font
	*/

	inline int LuaImplReplyVal(const LuaInstance& instance, Font::SizeInfo&& val, TypeTag<Font::SizeInfo>)
	{
		instance.PushTable();
		instance.PushField("LineHeight", val.lineHeight);
		instance.PushField("SpaceAdvance", val.spaceAdvance);
		instance.PushField("UnderlinePosition", val.underlinePosition);
		instance.PushField("UnderlineThickness", val.underlineThickness);

		return 1;
	}

	/*!
	* \brief Replies by value for Lua
	* \return 1 in case of success
	*
	* \param instance Lua instance to interact with
	* \param val Resulting IP address
	*/

	inline int LuaImplReplyVal(const LuaInstance& instance, IpAddress&& val, TypeTag<IpAddress>)
	{
		instance.PushInstance<IpAddress>("IpAddress", val);
		return 1;
	}

	/*!
	* \brief Replies by value for Lua
	* \return 1 in case of success
	*
	* \param instance Lua instance to interact with
	* \param val Resulting rectangle
	*/

	inline int LuaImplReplyVal(const LuaInstance& instance, Matrix4d&& val, TypeTag<Matrix4d>)
	{
		instance.PushInstance<Matrix4d>("Matrix4", val);
		return 1;
	}

	/*!
	* \brief Replies by value for Lua
	* \return 1 in case of success
	*
	* \param instance Lua instance to interact with
	* \param val Resulting rectangle
	*/

	inline int LuaImplReplyVal(const LuaInstance& instance, Matrix4f&& val, TypeTag<Matrix4f>)
	{
		instance.PushInstance<Matrix4d>("Matrix4", val);
		return 1;
	}

	/*!
	* \brief Replies by value for Lua
	* \return 1 in case of success
	*
	* \param instance Lua instance to interact with
	* \param val Resulting quaternion
	*/

	inline int LuaImplReplyVal(const LuaInstance& instance, Quaterniond&& val, TypeTag<Quaterniond>)
	{
		instance.PushInstance<Quaterniond>("Quaternion", val);
		return 1;
	}

	/*!
	* \brief Replies by value for Lua
	* \return 1 in case of success
	*
	* \param instance Lua instance to interact with
	* \param val Resulting quaternion
	*/

	inline int LuaImplReplyVal(const LuaInstance& instance, Quaternionf&& val, TypeTag<Quaternionf>)
	{
		instance.PushInstance<Quaterniond>("Quaternion", val);
		return 1;
	}

	/*!
	* \brief Replies by value for Lua
	* \return 1 in case of success
	*
	* \param instance Lua instance to interact with
	* \param val Resulting rectangle
	*/

	inline int LuaImplReplyVal(const LuaInstance& instance, Rectd&& val, TypeTag<Rectd>)
	{
		instance.PushInstance<Rectd>("Rect", val);
		return 1;
	}

	/*!
	* \brief Replies by value for Lua
	* \return 1 in case of success
	*
	* \param instance Lua instance to interact with
	* \param val Resulting rectangle
	*/

	inline int LuaImplReplyVal(const LuaInstance& instance, Rectf&& val, TypeTag<Rectf>)
	{
		instance.PushInstance<Rectd>("Rect", val);
		return 1;
	}

	/*!
	* \brief Replies by value for Lua
	* \return 1 in case of success
	*
	* \param instance Lua instance to interact with
	* \param val Resulting rectangle
	*/

	inline int LuaImplReplyVal(const LuaInstance& instance, Rectui&& val, TypeTag<Rectui>)
	{
		instance.PushInstance<Rectd>("Rect", val);
		return 1;
	}

	/*!
	* \brief Replies by value for Lua
	* \return 1 in case of success
	*
	* \param instance Lua instance to interact with
	* \param handle Resulting texture
	*/

	inline int LuaImplReplyVal(const LuaInstance& instance, TextureRef&& handle, TypeTag<TextureRef>)
	{
		instance.PushInstance<TextureRef>("Texture", handle);
		return 1;
	}

	/*!
	* \brief Replies by value for Lua
	* \return 1 in case of success
	*
	* \param instance Lua instance to interact with
	* \param val Resulting vector2D
	*/

	inline int LuaImplReplyVal(const LuaInstance& instance, Vector2d&& val, TypeTag<Vector2d>)
	{
		instance.PushInstance<Vector2d>("Vector2", val);
		return 1;
	}

	/*!
	* \brief Replies by value for Lua
	* \return 1 in case of success
	*
	* \param instance Lua instance to interact with
	* \param val Resulting vector2D
	*/

	inline int LuaImplReplyVal(const LuaInstance& instance, Vector2f&& val, TypeTag<Vector2f>)
	{
		instance.PushInstance<Vector2d>("Vector2", val);
		return 1;
	}

	/*!
	* \brief Replies by value for Lua
	* \return 1 in case of success
	*
	* \param instance Lua instance to interact with
	* \param val Resulting vector2D
	*/

	inline int LuaImplReplyVal(const LuaInstance& instance, Vector2ui&& val, TypeTag<Vector2ui>)
	{
		instance.PushInstance<Vector2d>("Vector2", val);
		return 1;
	}

	/*!
	* \brief Replies by value for Lua
	* \return 1 in case of success
	*
	* \param instance Lua instance to interact with
	* \param val Resulting vector3D
	*/

	inline int LuaImplReplyVal(const LuaInstance& instance, Vector3d&& val, TypeTag<Vector3d>)
	{
		instance.PushInstance<Vector3d>("Vector3", val);
		return 1;
	}

	/*!
	* \brief Replies by value for Lua
	* \return 1 in case of success
	*
	* \param instance Lua instance to interact with
	* \param val Resulting vector3D
	*/

	inline int LuaImplReplyVal(const LuaInstance& instance, Vector3f&& val, TypeTag<Vector3f>)
	{
		instance.PushInstance<Vector3d>("Vector3", val);
		return 1;
	}

	/*!
	* \brief Replies by value for Lua
	* \return 1 in case of success
	*
	* \param instance Lua instance to interact with
	* \param val Resulting vector3D
	*/

	inline int LuaImplReplyVal(const LuaInstance& instance, Vector3ui&& val, TypeTag<Vector3ui>)
	{
		instance.PushInstance<Vector3d>("Vector3", val);
		return 1;
	}

	/*!
	* \brief Replies by value for Lua
	* \return 1 in case of success
	*
	* \param instance Lua instance to interact with
	* \param ptr Resulting entity
	*/

	inline int LuaImplReplyVal(const LuaInstance& instance, Ndk::Entity* ptr, TypeTag<Ndk::Entity*>)
	{
		instance.PushInstance<Ndk::EntityHandle>("Entity", ptr);
		return 1;
	}

	/*!
	* \brief Replies by value for Lua
	* \return 1 in case of success
	*
	* \param instance Lua instance to interact with
	* \param ptr Resulting application
	*/

	inline int LuaImplReplyVal(const LuaInstance& instance, Ndk::Application* ptr, TypeTag<Ndk::Application*>)
	{
		instance.PushInstance<Ndk::Application*>("Application", ptr);
		return 1;
	}

	/*!
	* \brief Replies by value for Lua
	* \return 1 in case of success
	*
	* \param instance Lua instance to interact with
	* \param handle Resulting entity
	*/

	inline int LuaImplReplyVal(const LuaInstance& instance, Ndk::EntityHandle&& handle, TypeTag<Ndk::EntityHandle>)
	{
		instance.PushInstance<Ndk::EntityHandle>("Entity", handle);
		return 1;
	}

	/*!
	* \brief Replies by value for Lua
	* \return 1 in case of success
	*
	* \param instance Lua instance to interact with
	* \param handle Resulting node component
	*/

	inline int LuaImplReplyVal(const LuaInstance& instance, Ndk::NodeComponentHandle&& handle, TypeTag<Ndk::NodeComponentHandle>)
	{
		instance.PushInstance<Ndk::NodeComponentHandle>("NodeComponent", handle);
		return 1;
	}

	/*!
	* \brief Replies by value for Lua
	* \return 1 in case of success
	*
	* \param instance Lua instance to interact with
	* \param handle Resulting velocity component
	*/

	inline int LuaImplReplyVal(const LuaInstance& instance, Ndk::VelocityComponentHandle&& handle, TypeTag<Ndk::VelocityComponentHandle>)
	{
		instance.PushInstance<Ndk::VelocityComponentHandle>("VelocityComponent", handle);
		return 1;
	}

	/*!
	* \brief Replies by value for Lua
	* \return 1 in case of success
	*
	* \param instance Lua instance to interact with
	* \param ptr Resulting world
	*/

	inline int LuaImplReplyVal(const LuaInstance& instance, Ndk::World* ptr, TypeTag<Ndk::World*>)
	{
		instance.PushInstance<Ndk::WorldHandle>("World", ptr);
		return 1;
	}

	/*!
	* \brief Replies by value for Lua
	* \return 1 in case of success
	*
	* \param instance Lua instance to interact with
	* \param ptr Resulting world
	*/

	inline int LuaImplReplyVal(const LuaInstance& instance, Ndk::WorldHandle&& handle, TypeTag<Ndk::WorldHandle>)
	{
		instance.PushInstance<Ndk::WorldHandle>("World", handle);
		return 1;
	}

#ifndef NDK_SERVER

	/*!
	* \brief Replies by value for Lua
	* \return 1 in case of success
	*
	* \param instance Lua instance to interact with
	* \param handle Resulting console
	*/

	inline int LuaImplReplyVal(const LuaInstance& instance, Ndk::ConsoleHandle&& handle, TypeTag<Ndk::ConsoleHandle>)
	{
		instance.PushInstance<Ndk::ConsoleHandle>("Console", handle);
		return 1;
	}

	/*!
	* \brief Replies by value for Lua
	* \return 1 in case of success
	*
	* \param instance Lua instance to interact with
	* \param handle Resulting graphics component
	*/

	inline int LuaImplReplyVal(const LuaInstance& instance, Ndk::GraphicsComponentHandle&& handle, TypeTag<Ndk::GraphicsComponentHandle>)
	{
		instance.PushInstance<Ndk::GraphicsComponentHandle>("GraphicsComponent", handle);
		return 1;
	}

	/*!
	* \brief Replies by value for Lua
	* \return 1 in case of success
	*
	* \param instance Lua instance to interact with
	* \param val Resulting sound buffer
	*/

	inline int LuaImplReplyVal(const LuaInstance& instance, const SoundBuffer* val, TypeTag<const SoundBuffer*>)
	{
		instance.PushInstance<SoundBufferConstRef>("SoundBuffer", val);
		return 1;
	}

#endif

}
