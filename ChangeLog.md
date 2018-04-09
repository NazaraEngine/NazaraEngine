# Upcoming version:

Miscellaneous:
- Add possibility to excludes with one commande all tests/examples/tools/etc.
- Units tests are now part of the "test" exclusion category
- Fix project exclusion not working (but correctly excluding projects relying upon it)
- Upgraded Catch to v2.0.1
- ⚠️ Merged NazaraExtlibs workspace to main workspace (allowing `make` command to work without -f parameter) and removed extern libraries precompiled
- Updated stb_image to version 2.16 and stb_image_write to version 1.07 (allowing support for JPEG writing)
- ⚠️ Renamed extlibs folder to thirdparty
- Partial fix for Premake regenerating projects for no reason
- FirstScene now uses the EventHandler (#151)
- ⚠️ Rename Prerequesites.hpp to Prerequisites.hpp (#153)
- Updated premake5-linux64 with a nightly to fix a build error when a previous version of Nazara was installed on the system.
- Fix compilation with some MinGW distributions
- Add Lua unit tests
- NDEBUG is now defined in Release
- Replaced typedefs keywords with modern using keywords

Nazara Engine:
- VertexMapper:GetComponentPtr no longer throw an error if component is disabled or incompatible with template type, instead a null pointer is returned.
- Bitset swap operation is now correctly marked as noexcept`
- Mesh loaders now takes MeshParams vertexDeclaration into account
- ⚠️ Replaced RenderTarget::Get[Height|Width] by RenderTarget::GetSize
- ⚠️ Removed Window::Get[Height|Width] methods
- Fix compilation error when including Nazara/Renderer/ShaderBuilder.hpp
- Fix reflection sometimes being enabled by default for Materials
- Fix built-in unserialization of std::string which was corruption memory
- Fix Buffer::Destroy() not really destroying buffer
- Fix Bitset::TestAll() returned wrong result on full of '1' bitset
- ByteStream now returns the number of bytes written as the other streams
- Total rewriting of the color conversions
- Fix NormalizeAngle to the correct range
- Fix BoundingVolume::Lerp() with Extend_Null
- Simplification of methods Matrix4::SetRotation() and Quaternion::MakeRotationBetween()
- Fix mouve moved event generated on X11 platform when doing Mouse::SetPosition()
- EnumAsFlags specialization no longer require a `value` field to enable flags operators
- EnumAsFlags specialization `max` field can be of the same type as the enum
- Flags class now use an UInt8 or UInt16 to store the value if possible.
- Flags class is now explicitly convertible to any integer type of the same size (or greater size) than the internal size.
- Fix String movement constructor, which was leaving a null shared string (which was not reusable)
- Add Flags<E>::Test method, in order to test one or multiple flags at once.
- ⚠️ Vector2, Vector3 and Vector4 array/pointer constructor is now explicit to prevent some mistakes as `Vector2 vec2; vec2 = 0;`
- Fix RigidBody2D::SetGeom attribute copy and possible crash with static objects
- Fix error when opening a non-existent file on Posix
- Fix Directory::Create not working on Posix systems when recursive option was enabled
- Fix default directory permission (now created with 777)
- Add linear and angular damping accessor to RigidBody3D
- Fix MemoryStream::WriteBlock "Invalid buffer" assertion triggering when writing a zero-sized block
- ⚠️ Rename RigidBody3D::[Get|Set]Velocity to [Get|Set]LinearVelocity
- Fix RigidBody3D copy constructor not copying all physics states (angular/linear damping/velocity, mass center, position and rotation)
- Add RigidBody3D simulation control (via EnableSimulation and IsSimulationEnabled), which allows to disable physics and collisions at will.
- Fix some uninitialized values (found by Valgrind) in Network module
- Fix possible infinite recursion when outputting a Thread::Id object 
- ⚠️ Replaced implicit conversion from a Nz::String to a std::string by an explicit method ToStdString()
- Fix LuaInstance movement constructor/assignment operator which was corrupting Lua memory
- Fix potential bug on SocketImpl::Connect (used by TcpClient::Connect) on POSIX platforms
- It is now possible to initialize a StackArray with a size of zero on every platforms (this was not possible on non-Windows platforms before)
- Calling PlacementDestroy on a null pointer is now a no-op (was triggering an undefined behavior)
- Fix OBJParser relative offsets handling
- Add JPEG image saver
- Update Constraint2Ds classes (Add : Ref, Library, ConstRef, New function and Update : ctors)
- Fix LuaClass not working correctly when Lua stack wasn't empty
- Add RigidBody2D simulation control (via EnableSimulation and IsSimulationEnabled), which allows to disable physics and collisions at will.
- ⚠️ LuaInstance no longer load all lua libraries on construction, this is done in the new LoadLibraries method which allows you to excludes some libraries
- Clock::Restart now returns the elapsed microseconds since construction or last Restart call
- Add PhysWorld2D::[Get|Set]IterationCount to control how many iterations chipmunk will perform per step.
- Add PhysWorld2D::UseSpatialHash to use spatial hashing instead of bounding box trees, which may speedup simulation in some cases.
- Add PhysWorld[2D|3D] max step count per Step call (default: 50), to avoid spirals of death when the physics engine simulation time is over step size.
- Fix Window triggering KeyPressed event after triggering a resize/movement event on Windows
- (WIP) Add support for materials and callbacks to Physics3D module.
- PhysWorld3D class is now movable
- ⚠️ Removed array/pointer constructor from Vector classes
- Fixed Platform module not being classified as client-only
- ⚠️ Renamed Bitset::Read to Bitset::Write
- Fixed ENetCompressor class destructor not being virtual
- ⚠️ Added a type tag parameter to Serialize and Unserialize functions, to prevent implicit conversions with overloads
- Added Collider3D::ForEachPolygon method, allowing construction of a debug mesh based on the physics collider
- Fixed ConvexCollider3D::GetType returning Compound instead of ConvexHull.
- Dual-stack sockets are now supported (by using NetProtocol_Any at creation/opening)
- Fixed IPv6 addresses not being correctly encoded/decoded from the socket API.
- Fix copy and move semantic on HandledObject and ObjectHandle
- Add support for emissive and normal maps in .mtl loader using custom keywords ([map_]emissive and [map_]normal)
- Music, Sound and SoundEmitter are now movable

Nazara Development Kit:
- Added ImageWidget (#139)
- ⚠️ Removed TextAreaWidget::GetLineCount
- Fix World movement which could cause crashes when updating systems
- Fix crash occuring sometimes on keyboard event
- Add support for EchoMode to TextAreaWidget (which allows to setup password text area)
- Add signal OnTextChanged to TextAreaWidget
- ⚠️ Removed TextAreaWidget::GetGlyphUnderCursor
- Fixed minor issues relative to TextAreaWidget cursor handling
- ⚠️ Renamed BaseWidget::GrabKeyboard method to SetFocus
- Added BaseWidget::ClearFocus method and OnFocus[Lost|Received] virtual methods
- TextAreaWidget will now show a cursor as long as it has focus
- Fix BaseWidget linking error on Linux
- ⚠️ Rewrite StateMachine to fix instantaneous state changing (state change is no longer effective until the next update call)
- Fix entities destruction when coming from World::Clear() (also called by destructor), which invalidated world entities handles before destroying entities (preventing destruction callback to get valid entities handles from world)
- Add Entity::Disable method, which is a shortcut to Enable(false)
- Add BaseWidget::HasFocus
- Fix TextAreaWidget cursor sometimes showing up in readonly mode
- ⚠️ BaseWidget::OnKeyPressed now returns a boolean to indicate if it should block default action (such as tab to switch to the previous/next widget)
- Pressing tab/shift-tab will now move to the next/previous widget able to be focused on
- Fix GraphicsComponent::Clear method now clearing reflective states
- Add linear and angular damping accessor to PhysicsComponent3D
- Fix GraphicsComponent cloning not copying renderable local matrices
- ⚠️ Rename PhysicsComponent3D::[Get|Set]Velocity to [Get|Set]LinearVelocity
- Add OnEntityDisabled and OnEntityEnabled callbacks to BaseComponent
- Disabling an entity with a CollisionComponent3D or PhysicsComponent3D will properly disable it from the physics simulation
- It is now possible to disable synchronization between a PhysicsComponent3D and the NodeComponent
- Fix PhysicsComponent3D copy which was not copying physics state (such as mass, mass center, damping values, gravity factor and auto-sleep mode)
- Fix TextAreaWidget::Clear crash
- Add ConstraintComponent2D class
- Fix CollisionComponent3D initialization (teleportation to their real coordinates) which could sometimes mess up the physics scene.
- ⚠️ Renamed World::Update() to World::Refresh() for more clarity and to differentiate it from World::Update(elapsedTime)
- World entity ids are now reused from lowest to highest (they were previously reused in reverse order of death)
- World now has an internal profiler, allowing to measure the refresh and system update time
- CollisionComponent[2D|3D] and PhysicsComponent[2D|3D] now configures their internal RigidBody userdata to the entity ID they belong to (useful for callbacks).
- Fixed EntityList copy/movement assignment operator which was not properly unregistering contained entities.
- ListenerSystem now handles velocity in a generic way (no longer require a VelocityComponent and is compatible with physics)
- World now has const getters for systems

# 0.4:

Build system:
- ⚠️ Update premake binaries version to premake5-alpha12
- It is now possible to generate a tool project running which will run the premake when built on Windows (PremakeProject=true).
- Added VS2017 Windows shortcut

Nazara Engine:
- Improved Particle Demo
- https://github.com/DigitalPulseSoftware/NazaraEngine/pull/126 File locking now works properly on Linux
- https://github.com/DigitalPulseSoftware/NazaraEngine/pull/136 Fixed std::getline with Nz::String not setting eofbit.
- Added support for CRC64
- Added MovablePtr utility class
- ⚠️ StackAllocation class has been replaced with much-more user-friendly StackArray class.
- ⚠️ ParameterList now takes double and long long parameter instead of float and int parameters.
- Threads can now be named (for debug purposes)
- Added ParameterList::ForEach method
- Flags<T> operators works in the global namespace as well
- **Added support for skybox reflections, realtime reflections are on the tracks!**
- ⚠️ InstancedRenderable now handle materials and skins in a generic way (which means you can use skins on every InstancedRenderable such as Billboard, Sprites, etc.).
- **Added support for coroutines (LuaCoroutine class)**
- ⚠️ Introduction of a new class, LuaState (inherited by LuaCoroutine and LuaInstance).
- **Added ENet protocol support for reliable UDP communication**
- ⚠️ SocketPoller is now able to wait on read/write status of socket
- ⚠️ SocketPoller::Wait() has now support for infinite waiting (-1)
- Added UdpSocket::ReceiveMultiple method
- **Added WIP shader build (based on AST, with support for GLSL)**
- https://github.com/DigitalPulseSoftware/NazaraEngine/pull/127 Fixed RigidBody2D::GetAABB() wrong AABB
- **Added basic support for constraints**
- Added support for collision callbacks
- Added support for raycast
- Added support for nearest body queries
- Added support for debug draw
- Added support for damping
- ⚠️ RigidBody2D created without mass are now kinematic by default instead of statics
- https://github.com/DigitalPulseSoftware/NazaraEngine/pull/128 ⚠️ **Platform-specific classes were moved to a new module: Platform** (this means Utility module no longer needs to be linked to X11/xcb or gdi32).
- https://github.com/DigitalPulseSoftware/NazaraEngine/pull/135 ⚠️ Improved vertex declarations (they now checks template type with real type)
- Mesh index buffer optimization is now disabled by default in debug mode
- It is now possible to set the vertex declaration wanted when building/loading a mesh
- It is now possible to set index/vertex buffer usage flags when building/loading a mesh
- Added VertexMapper::GetVertexBuffer()
- Added VertexMapper::GetVertexCount()
- Added VertexMapper::HasComponentOfType()
- Fixed SimpleTextDrawer bounds computation


Nazara Development Kit:
- ⚠️ Components no longer need to be copyable by assignation
- Added BaseComponent::GetEntity()
- ⚠️Systems are no longer copyable/clonable
- ⚠️World::[Get|Set]UpdateRate has been renamed World::[Get|Set]FixedUpdateRate.
- Added World::[Get|Set]MaximumUpdateRate.
- Added BaseWidget::CenterHorizontal()
- Added BaseWidget::CenterVertical()
- EntityHandle are now handled in such a way they will never move in memory until a world update is triggered, this means it is now safe to returns const reference to EntityHandle as long as you do not keep them from one world update to another.
- Fixed a crash when destroying a hovered widget
- Added CollisionComponent2D::GetAABB()
- Added Entity::OnEntityDestruction signal
- ⚠️EntityList were remade to take less memory and are easier to use but can only be iterated from front to back
- Entities are now automatically removed from EntityLists when destroyed.
- It is no longer required for a component to have a default constructor to be binded to Lua.
- https://github.com/DigitalPulseSoftware/NazaraEngine/pull/123 StateMachine can now handle multiple states at once
- https://github.com/DigitalPulseSoftware/NazaraEngine/pull/132 Added ProgressBarWidget
- https://github.com/DigitalPulseSoftware/NazaraEngine/pull/133 ButtonWidget color and texture can now be customized
- https://github.com/DigitalPulseSoftware/NazaraEngine/pull/130 Added CheckboxWidget
- Added OnEntityDestruction() event on components in order to let them do better cleanup.

# 0.3

Build system:
- Added `Configurations` parameter for build system, with support for ReleaseWithDebug. (67dcf166b80b4de1ecb5256a271a5ac80526435d)

Nazara Engine:
- **Added new particle demo (Space battle)**. (3c6a6cd3a9294e5ba577af9ec3bef695438c28b7)
- **Added back automatic Frustum Culling**. (a349b931e6a5b45586316763a458e1d89758542a)
- **Added Nz::Flags class to properly handle enum flags**. (d6b793f46178a05998182a4bbe595c425465eb07)
- **Refactored Buffer classes to prepare the new Renderer**. (9e0fd0a8e82567d03f063728e76f6dd5571760f2)
- **Added a way to override über-shaders used by the engine at runtime**. (d2d6bae47f326562a2c50631f169100e35ecdd09)
- ⚠️ **Reworked Nz::Cursor, it now includes default system cursors, allows to retrieve its image and is ref-counted** (6751d480b152a6203262b72e2cbe120007741621, 6751d480b152a6203262b72e2cbe120007741621, 0cab95e8aea4d2fb8a58c9dba2d609e33c748093).
- ⚠️ Nz::WindowCursor has been renamed to Nz::SystemCursor. (6751d480b152a6203262b72e2cbe120007741621)
- Added Nz::CursorController class, for indirect cursor control. (a2e5e4874469958b658c24d8a52bb9bc274e9398)
- Added Nz::UdpSocket::SendMultiple method, allowing to merge multiple buffers into the same datagram. (ea0d42f4234bcca184bb42aef16fb2a4e1346913).
- Added [Nz::TcpClient::SendMultiple](https://nazara.digitalpulsesoftware.net/doc/class_nz_1_1_tcp_client.html#a495c32beb46ed9192699a3b82d358035) method, allowing to send multiple buffers at once.
- Added [Nz::PlacementDestroy](https://nazara.digitalpulsesoftware.net/doc/namespace_nz.html#a27c8667def991fc896c5beff3e62668a). (ea985fa76586762f008e4054938db3234eeaf0cb)
- Added [Nz::String::Format](https://nazara.digitalpulsesoftware.net/doc/class_nz_1_1_string.html#a4b699982e7f9ea38f6d44b43ac1e2040) and [Nz::String::FormatVA](https://nazara.digitalpulsesoftware.net/doc/class_nz_1_1_string.html#abe0fcbce11224b157ac756b60e8dee92) static methods. (cc6e4127dc6c61799a64404770992cef0804ad34).
- Added [Nz::ParticleGroup::GetBuffer](https://nazara.digitalpulsesoftware.net/doc/class_nz_1_1_particle_mapper.html#aefe1b251efc8c9b8668842275561be0c) method. (4dc85789b59e50d964c83321dbd4b6485c04bef6)  
- Added Nz::ParticleMapper::GetPointer method. (1f4e6c2d1594b7bb9dd6f4ea5480fdd16cf5f208)  
- ⚠️ Structures provied by ParticleStruct header now have a float life. (472d964d587d906764ad1e05bfcc9ab1bf979483)
- Fixed scale property of Nz::TextSprite not affecting its bounding volume. (52b29bac775823294c4ad7de70f4dc3f4adfa743)
- ⚠️ Nz:MeshParams::flipUVs has been replaced by texCoordOffset and texCoordScale. (a1a7d908adc060fd7a43491c903dfe3b501d98e5)
- Fixed [Nz::Music::Stop](https://nazara.digitalpulsesoftware.net/doc/class_nz_1_1_music.html#a9f0eb20328d6b35ab290b19364e95ee8) which was not resetting the playing offset. (12d7bc9aa3b672fc855478904072ed18f06e37ca, 24be97447af5c55b444a96e8d5d000e590279171)
- Reworked LuaBinding classes to try to improve compile time and generated objects memory. (5555d24afca2ec766c7625bb8e959560677b69c2).
- ⚠️ Convert OpenMode to use the new Nz::Flags class. (49dfe31fa036cdac4f531a15972e2bd52fa9ab57)
- ⚠️ Convert StreamOption to use the new Nz::Flags class. (49dfe31fa036cdac4f531a15972e2bd52fa9ab57)
- ⚠️ Convert WindowStyleFlags to use the new Nz::Flags class. (6c0422350fea520f96253df1113ee7c49233bd06)
- ⚠️ Fix typo in OpenMode_MustExist (previously written as "MustExit"). (445ab13ef8a78c07697556ae50086a9276cbf7c2)
- Added [Nz::String::GetCharacterPosition](https://nazara.digitalpulsesoftware.net/doc/class_nz_1_1_string.html#acd89bc8d5afaa808c8baa480501d5f58) method. (2f4ca23cdaefc8350a95d2d6aa741602b5ee00f1)
- Exposed Nz::SoftwareBuffer class. (842786d2d482890ad3ed9ba88613f1d2a0f901ba)
- ⚠️ Removed Sphere::SquaredDistance method, use Vector3::SquaredDistance with its center instead. (5b017aecfdb86246fe8517453376801b00c23843)
- Fixed SceneAmbient shader uniform not being sent by DepthRenderTechnique. (83e345a2fc25073a9f10b0a3547a75692613f9b3)
- Fixed light selection bug (causing an object to not have any light on it although it is within its radius). (9c04d79b2906940067a32d84800edd1ffd38d9bd)
- Renderer::SetTexture and Renderer::SetTextureSampler now takes `unsigned int` instead of `UInt8` texture indexes. (09c2cbf1c5eeadac52c72459b0fb6df3064fc16a)
- When a shader fails to validate, its uniform are now dumped to the log. (0d37a3d4bf68bce9f533ad8c95225809cc312cdd)
- When a shader fails to compile, its code is now dumped to the log. (53ee8915fa0255b5c7492952919edd3a70e29b6c)
- ⚠️ Texture units used by Nazara are now static, fixes a bug with shadow. (b290a1233d725636d73c3bd8b37c394d93789524)
- Nz::Signal move constructor and assignement operators are now `noexcept`. (00144e944e6d8d894aef8b7bced9a6b364091626)
- Nz::EventHandler is now a handled object. (498b84fc690bae084a63ef379452cd45173c933a).
- Added a way to specify receive and send buffer size per socket. (c4459f5910d1f7e5833e2cbdca1dbd048a9a0416).
- Fixed ObjectHandle <= operator. (6f601101d23fe790dd83a1f69a137009116ad91b)
- Fixed Nz::UdpSocket::Receive failing when peers suddenly closes its socket. (12401e0e2f0cee0ab8fcd9124cce368e54f8037b)
- All noises classes now uses std::mt19937 as a random number generator, to ensure the same results on every machine. (1f5ea9839016964c173d919263827dee69ecb65d)

Nazara Development Kit:
- **Added basic widgets**. (c8a12083b3133e946bf60dd060331a4b4631f8d8)  
- VelocitySystem will no longer affect entities with PhysicsComponent2D. (a6853234412c744cdcb28344f02f7b0c92704d77)
- Fixed EulerAngles constructor in Lua. (d55149a0a70f6230b6f1c3fb50e37dc82a2feb9f)
- Fixed Component::OnDetached not being called on entity destruction. (5b777eb4853639d7aeb232ca46d17f0d432f47ca)

Nazara Engine:

# 0.2.1

Nazara Engine:
- Nazara binaries are now compiled with Run-Time Type-Information. (a70acdc8f44010627a65282fd3099202116d3e13)
- Nazara demos are now compiled with relative dependencies on Linux. 
  (d6fbb4c408d48c4a768fad7b43460c76a0df1777)
- Added [**Nz::BitCount**](https://nazara.digitalpulsesoftware.net/doc/group__core.html#ga6bfbcff78eb6cfbe3ddaedcfc8c04196) function. (82e31a3ec8449da6618f41690164c2e1d883edb4)
- Added [**Nz::Bitset::AppendBits**](https://nazara.digitalpulsesoftware.net/doc/class_nz_1_1_bitset.html#a5ca8f365006c86d6d699d02471904f7e) method. (b018a400499a2356c4455a40d9f6a6c12b3cb36b)
- Added [**Nz::Bitset::Read**](https://nazara.digitalpulsesoftware.net/doc/class_nz_1_1_bitset.html#aca204e1d71e36d6c8c2c544ffd9824ac) method. (f0c40ecb2f2f64f5af46f38e4b589d8c1dea824c)
- Added [**Nz::Bitset::Reverse**](https://nazara.digitalpulsesoftware.net/doc/class_nz_1_1_bitset.html#af372e64f33a2114837fb25daffcc1008) method. (0abd1bbfbf6b949e350a78170aae1b45698620eb)
- Added [**Nz::Bitset::FromPointer**](https://nazara.digitalpulsesoftware.net/doc/class_nz_1_1_bitset.html#a487836f91821e6d7901eb753baf47de3) static method. (f0c40ecb2f2f64f5af46f38e4b589d8c1dea824c)
- Added an operator<< for `ostream` and `Nz::Bitset`. (9d9efac2baf29842e58b3e73f255ca06cb24d1fb)
- Added `Nz::Image::HasAlpha` method. (1029b37d3654aea9fcc4fd2a43e6d32c9842268a)
- Added a [**Sprite::SetMaterial**](https://nazara.digitalpulsesoftware.net/doc/class_nz_1_1_sprite.html#a3074f991b72af8146c5b5ba2fdda42fa) overload taking a material name. (d50f5ed7c88e613e01c76c3765d86b2014773ba8)
- Added a [**Sprite::SetTexture**](https://nazara.digitalpulsesoftware.net/doc/class_nz_1_1_sprite.html#a6ddf41ffff1e155a99b06487e72ae47d) overload taking a texture name. (d50f5ed7c88e613e01c76c3765d86b2014773ba8)
- Added Nz::SegmentCollider2D class. (0ede1e893aab6ab159b57eea24af273e82d68d16)
- Nz::CountBits now returns a `std::size_t`. (63e9d77e77d35f9f0b14529dd8d86b2cd39b751e)
- Improved bitset unit tests by checking them with multiple blocks size (f6426a53d77ccee2297a0efa8b811e482f65a48b)
- `NDEBUG` is no longer automatically defined by Nazara headers. (fef5337279ea33f0675c55002f1df77234252168)
- The engine now asserts on CHAR_BIT equality to 8. (01be79f8524e5f68e713a6070d3b5aacfa595aa5)
- Fixed a crash occuring after a RigidBody2D got modified/removed. (874362a606f513be1888997f2f1b87cad4fbca53)
- Fixed Nz::Bitset::PerformandsAND (called by &= operator) giving wrong result. (ecfce94461d1c2b96bdab7f957e14856c7100108)
- PixelFormats with over 64 bits per components are now rejected. (119b7bcad4dd16f5499e2ec6a9da48c3985b036f)
- Fixed `PixelFormatInfo` masks bit order. (62197da39e6dccbe957794e5422454c49c4f039f)
- Fixed RigidBody2D collisions. (c99d7fd640a69f18a5f615d2ebc6d7f15d329f6e)
- Fixed RigidBody2D::AddForce application point. (7eb240e4a1fa1af16aa68197ec688f71ff3d32c4)
- Fixed Nz::Quaternion::Lerp compilation. (739291651eef4bc90ad14342415bf88d20142f0f)

Nazara Development Kit:
- Fixed missing [group **NDK**](https://nazara.digitalpulsesoftware.net/doc/group___n_d_k.html) in documentation. (51c6b0241c074c64319f2347eaea72992951322f)
- Fixed CollisionComponent2D position when used alone. (e24d433f7563fcd4156ac3be01570752bd7c734a)
- Including the following changes in the Lua API:
  - CameraComponent is now accessible from Lua (cfb40bf4dc4777012a11fea528f8203ef53c5686).
  - Methods from EulerAngles and Quaternions classes are now accessible from Lua. (0886292c00ea3826c6c23e1e9d1c76bd6c0cf28d)
  - Keyboard is now accessible from Lua (e50c9757e4f64aed553ebfa3859d2642c03ba58e)

# 0.2

Build system:
- Added a .editorconfig file to help consistency (8126c981946591c4e5a4bc0e56268aecbe6a76f1)
- Code::Blocks is now supported by Premake5 (40276139b52e0aae7daf20d3f0a84b007e84a993)
- Global headers no longer include generation date. (fd3161c8e9bb9e262ed2359d679f1c47564dbac2)
- Upgraded Premake5 to alpha10, removing Premake4 in the process. (110064c08f429664f2ecc2a86319afee2441b28e)

Nazara Engine:
- ⚠️**The Physics module has been renamed to Physics3D** (65bfd77623d1bf337849cc828022adf190c1d677)
  - The PhysObject class has been renamed to RigidBody3D (d2c8ca0f0cfc91869e1802ee759ea2b356fc0a30)
  - The PhysWorld class has been renamed to PhysWorld3D (d130719c5fd9521062f8c9e48729aba430061c77)
  - The *Geom classes have been renamed to *Collider3D (fc1ea178c7e1dba7739443afcec339f6622243ae)
  - The GeomType enum has been renamed to ColliderType3D and is now part of thz Nz namespace (1f75d449d61612909b1e879f2558c89dd41c2394)
- **A new Physics2D module has been added, using [chipmunk](https://chipmunk-physics.net)** (e9be18d1813197004a06d5eb13fe87232ccd6168)
- ⚠️ **`NAZARA_UTILITY_THREADED_WINDOW` got replaced by the runtime flag `WindowStyle_Threaded`**. (a7dd0e0a20c4c4401c663f4665ea9133dfbca1a5)
- LuaClass is now default-constructable and (re)initialized later with its `Reset` method (dc3e125bdac3e0c790743af78fead3a2e28c73b3)
- LuaClass is now able to bind classes with deleted destructor (3168e5ae07e29168223666dd23c29cb6cd7788ed)
- Added a `LuaInstance::Push` variadic overload able to push multiple arguments (a19edf1676181ec8765d9d61a02c8b0f2533c9df)
- The Vector classes now have a std::hash overload (74b446af2fc0898afa170c7fec8eaf6b5cf30614)
- The TextEntered event handling on Linux has been improved (7d1c5fa1af98abc2f253fd4a9c6b6d8604dbccaa)
- An interactive unit test has been added for testing the window events (34d92320b6252e1bf4e86e3544ecc06dc3126d8d)
- `AbstractLogger::IsStdReplicationEnabled()` is now const (5df095c0f178fcb5f772aaf9a9fb4bf79257b008)
- `FileLogger::IsTimeLoggingEnabled()` is now const (de0f93116aad07d5893e8d3ba3ec1313d7458bdf)
- Fixed `Apply` return type not taking references into account (1eb49d38691798d78c7e6559007d4f496b625db3)
- Fixed `LuaInstance::PushInstance` memory corruption (54b77c0f48a005e345109a85f49cbc4b28d93f07)
- Fixed `Matrix4::Get(Column|Row)` compilation (728d7b829e6604e314e9b8a6dfa5bd5d85602c7a)
- Fixed `ResourceManager::Purge` compilation (c7002830f58f092adb6c055a911397abc949fd4a)
- Fixed `Quaterniond::operator*` compilation (19dc95ae7c65b148cc5faeb897ded4880b9fba28)
- Fixed the <= operator with two ObjectRef (7597578bbf400e7610103b191e4b8e9e2af0b47f)
- ⚠️ LuaClass methods now include an `argumentCount` parameter, and the instance remains at the top of the Lua stack, shifting all parameters by one (9b7947812b4c4a4543a1447daea4472f4f674b7f)
- Fixed automatic lua binding of `T& T::Method()`, which will now return the original Lua instance instead of copying it. (d20fa0077a74dbe5e9e914f7adfd0c62faf5fa3a)
- `VideoMode` third argument (`bpp`) is now optional. (2ab311e0a68523b2113b25f24244b855cc4712bb)
- `ByteArray::ToHex` is now implemented in ByteArray.cpp, to reduce the amount of warnings generated by MSVC because of the `sprintf` call. (41e7e6af601fbd3b2ca96b188e4906c010903ef0)

Nazara Development Kit:
- ⚠️ The Collision and Physics components now have a 3D suffix (6e289fe789d64ac9730ddc0f4cc240e02754e8a3)
- Added Collision and Physics components for 2D physics. (b5048dfb3704675b9f7438d080a9347c99884972)
- `LuaAPI::GetBinding()` will now implicitly initialize LuaAPI if required (ec161141d8d99db250f4f5a1e739123ad4f91750)
- It is now possible to change the update order of a world's systems. (51b6979bb59b6d880d0bcacef1b3d8d7420a5fa6)
- By default, the Render and Listener systems update are now performed last. (dd22e5f1054da80b5b6aff5ae96e9832b4425ed7)
- PhysicsSystem3D no longer initialize an internal PhysWorld3D if no physical entity gets created (e282442407c028c5d1cff96847b5b7f857a646bb)
- Including the following changes in the Lua API:
  - `GraphicsComponent:Attach` overloads (taking local matrix and render order) are fixed. (d525669f3ae0d40266df9c34b6b1525a10a26d7e)
  - Material is now accessible from Lua (aed4e1ee91d9875592adb178334220339afb72a0)
  - Matrix4 is now accessible from Lua (fb518403659455ae79be848b99dd8f6083f1ab58)
  - Sprite is now accessible from Lua (e034dce76dc6a2fd1f403221e1945c1a2c3e28ee)
  - SpriteLibrary is now accessible from Lua (013a133f60d46114e1b02fed7eab9f9a9f506068)
  - Texture is now accessible from Lua (f8b55a5063c058965668ed4277a7e50280ea100d)
  - TextureLibrary is now accessible from Lua (3a64ef9e136c457a8207440e37361b5b3384e133)
  - TextureManager is now accessible from Lua (3a64ef9e136c457a8207440e37361b5b3384e133)

Issues fixed:
- #102: Some shaders used by the Graphics module may fail to compile with some drivers.
- #104: On Windows, when using threaded windows, the window position obtained by `Window::GetPosition()` never gets updated.
- #105: On Windows, when not using threaded windows, a lot of Moved events gets generated instead of just one for every movement.
- #111: On Linux, `IsKeyPressed(Keyboard::Q);` returns true if the `A` key is pressed.
- #114: Box/cubic sphere submesh generation is broken.

# 0.1.1

Additions/Changes:
- Nz::Bitset now supports shifting operations (along with <<, >> operators) (37089d9a5fba52ba83f46d603381584a13036ac0).

Issues fixed:
- #73, #81: Nazara fails to compile on Linux with x86 target.
- #75: Lua constructors may fail in some cases.
- #77: Packaging the engine under Linux fails to copy the executable binaries of the demo and unit tests.
- #78: Building with Clang fails.
- #79: `SocketPoller::Wait` does not update its `error` parameter on Linux.
- #80: When initializing the engine, some pixel format errors occurs, this currently has no side-effect.

# 0.1
- Initial release
