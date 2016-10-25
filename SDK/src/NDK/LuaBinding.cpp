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
	clock("Clock"),
	directory("Directory"),
	file("File"),
	stream("Stream"),

	// Math
	eulerAngles("EulerAngles"),
	matrix4d("Matrix4"),
	quaternion("Quaternion"),
	rect("Rect"),
	vector2d("Vector2"),
	vector3d("Vector3"),

	// Network
	abstractSocket("AbstractSocket"),
	ipAddress("IpAddress"),

	// Utility
	abstractImage("AbstractImage"),
	font("Font"),
	node("Node"),

	// SDK
	application("Application"),
	entity("Entity"),
	nodeComponent("NodeComponent"),
	velocityComponent("VelocityComponent"),
	world("World")

	#ifndef NDK_SERVER
	,

	// Audio
	music("Music"),
	sound("Sound"),
	soundBuffer("SoundBuffer"),
	soundEmitter("SoundEmitter"),

	// Graphics
	instancedRenderable("InstancedRenderable"),
	material("Material"),
	model("Model"),
	sprite("Sprite"),
	spriteLibrary("SpriteLibrary"),

	// Renderer
	texture("Texture"),

	// SDK
	console("Console"),
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
