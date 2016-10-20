// This file was automatically generated on 26 May 2014 at 01:05:31

#include <NDK/LuaBinding.hpp>

namespace Ndk
{
	/*!
	* \ingroup NDK
	* \class Ndk::LuaBinding
	* \brief NDK class that represents the binding between the engine & the SDK with the Lua scripting
	*/

	/*!
	* \brief Binds modules to Lua
	*/

	LuaBinding::LuaBinding() :
	// Core
	clockClass("Clock"),
	directoryClass("Directory"),
	fileClass("File"),
	streamClass("Stream"),

	// Math
	eulerAnglesClass("EulerAngles"),
	matrix4dClass("Matrix4"),
	quaternionClass("Quaternion"),
	rectClass("Rect"),
	vector2dClass("Vector2"),
	vector3dClass("Vector3"),

	// Network
	abstractSocketClass("AbstractSocket"),
	ipAddressClass("IpAddress"),

	// Utility
	abstractImage("AbstractImage"),
	fontClass("Font"),
	nodeClass("Node"),

	// SDK
	application("Application"),
	entityClass("Entity"),
	nodeComponent("NodeComponent"),
	velocityComponent("VelocityComponent"),
	worldClass("World")

	#ifndef NDK_SERVER
	,

	// Audio
	musicClass("Music"),
	soundClass("Sound"),
	soundBuffer("SoundBuffer"),
	soundEmitter("SoundEmitter"),

	// Graphics
	instancedRenderable("InstancedRenderable"),
	modelClass("Model"),

	// SDK
	consoleClass("Console"),
	graphicsComponent("GraphicsComponent")
	#endif
	{
		BindCore();
		BindMath();
		BindNetwork();
		BindSDK();
		BindUtility();

		#ifndef NDK_SERVER
		BindAudio();
		BindGraphics();
		BindRenderer();
		#endif
	}

	/*!
	* \brief Registers the classes that will be used by the Lua instance
	*
	* \param instance Lua instance that will interact with the engine & SDK
	*/

	void LuaBinding::RegisterClasses(Nz::LuaInstance& instance)
	{
		RegisterCore(instance);
		RegisterMath(instance);
		RegisterNetwork(instance);
		RegisterSDK(instance);
		RegisterUtility(instance);

		#ifndef NDK_SERVER
		RegisterAudio(instance);
		RegisterGraphics(instance);
		RegisterRenderer(instance);
		#endif
	}
}
