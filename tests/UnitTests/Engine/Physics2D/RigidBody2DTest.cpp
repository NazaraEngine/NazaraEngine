#include <Nazara/Physics2D/RigidBody2D.hpp>
#include <Nazara/Physics2D/PhysWorld2D.hpp>
#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <iostream>
#include <limits>

Nz::RigidBody2D CreateBody(Nz::PhysWorld2D& world);
void EQUALITY(const Nz::RigidBody2D& left, const Nz::RigidBody2D& right);
void EQUALITY(const Nz::Rectf& left, const Nz::Rectf& right);

SCENARIO("RigidBody2D", "[PHYSICS2D][RIGIDBODY2D]")
{
	GIVEN("A physic world and a rigid body")
	{
		Nz::PhysWorld2D world;
		world.SetMaxStepCount(std::numeric_limits<std::size_t>::max());
		world.SetStepSize(Nz::Time::TickDuration(200)); //< FIXME: Tests fail on Linux with default step size

		Nz::Vector2f positionAABB(3.f, 4.f);
		Nz::Rectf aabb(positionAABB.x, positionAABB.y, 1.f, 2.f);

		Nz::RigidBody2D::DynamicSettings dynamicSettings;
		dynamicSettings.collider = std::make_shared<Nz::BoxCollider2D>(aabb);
		dynamicSettings.mass = 1.f;

		Nz::RigidBody2D body(world, dynamicSettings);
		float angularVelocity = 0.2f;
		body.SetAngularVelocity(angularVelocity);
		Nz::Vector2f massCenter(5.f, 7.f);
		body.SetMassCenter(massCenter);
		Nz::Vector2f position(9.f, 1.f);
		body.SetPosition(position);
		float rotation = 0.1f;
		body.SetRotation(rotation);
		Nz::Vector2f velocity(-4.f, -2.f);
		body.SetVelocity(velocity);

		world.Step(Nz::Time::Second());

		Nz::RigidBody2D::StaticSettings staticSettings;

		WHEN("We copy construct the body")
		{
			body.AddForce(Nz::Vector2f(3.f, 5.f));
			Nz::RigidBody2D copiedBody(body);
			EQUALITY(copiedBody, body);
			world.Step(Nz::Time::Second());
			EQUALITY(copiedBody, body);
		}

		WHEN("We move construct the body")
		{
			Nz::RigidBody2D copiedBody(body);
			Nz::RigidBody2D movedBody(std::move(body));
			EQUALITY(movedBody, copiedBody);
		}

		WHEN("We copy assign the body")
		{
			Nz::RigidBody2D copiedBody(world, staticSettings);
			copiedBody = body;
			EQUALITY(copiedBody, body);
		}

		WHEN("We move assign the body")
		{
			Nz::RigidBody2D copiedBody(body);
			Nz::RigidBody2D movedBody(world, staticSettings);
			movedBody = std::move(body);
			EQUALITY(movedBody, copiedBody);
		}

		WHEN("We set a new geometry")
		{
			float radius = 5.f;
			body.SetCollider(std::make_shared<Nz::CircleCollider2D>(radius));

			world.Step(Nz::Time::Second());

			THEN("The aabb should be updated")
			{
				position = body.GetPosition();
				Nz::Rectf circleAABB(position.x - radius, position.y - radius, 2.f * radius, 2.f* radius);
				EQUALITY(body.GetAABB(), circleAABB);
			}
		}
	}

	GIVEN("A physic world")
	{
		Nz::PhysWorld2D world;
		world.SetMaxStepCount(std::numeric_limits<std::size_t>::max());

		Nz::Rectf aabb(3.f, 4.f, 1.f, 2.f);

		WHEN("We get a rigid body from a function")
		{
			std::vector<Nz::RigidBody2D> tmp;
			tmp.push_back(CreateBody(world));
			tmp.push_back(CreateBody(world));

			world.Step(Nz::Time::Second());

			THEN("They should be valid")
			{
				CHECK(tmp[0].GetAABB() == aabb);
				CHECK(tmp[1].GetAABB() == aabb);
			}
		}
	}

	GIVEN("A physic world and a rigid body")
	{
		Nz::PhysWorld2D world;
		world.SetMaxStepCount(std::numeric_limits<std::size_t>::max());

		Nz::Vector2f positionAABB(3.f, 4.f);
		Nz::Rectf aabb(positionAABB.x, positionAABB.y, 1.f, 2.f);

		Nz::RigidBody2D::DynamicSettings dynamicSettings;
		dynamicSettings.mass = 1.f;

		auto box = std::make_shared<Nz::BoxCollider2D>(aabb);

		Nz::RigidBody2D body(world, dynamicSettings);
		body.SetCollider(box, true, false);

		Nz::Vector2f position = Nz::Vector2f::Zero();
		body.SetPosition(position);

		world.Step(Nz::Time::Second());

		WHEN("We retrieve standard information")
		{
			THEN("We expect those to be true")
			{
				CHECK(body.GetAABB() == aabb);
				CHECK(body.GetAngularVelocity() == 0.f);
				CHECK(body.GetMassCenter(Nz::CoordSys::Global) == position);
				CHECK(body.GetCollider() == box);
				CHECK(body.GetMass() == Catch::Approx(dynamicSettings.mass));
				CHECK(body.GetPosition() == position);
				CHECK(body.GetRotation().value == Catch::Approx(0.f));
				CHECK(body.GetVelocity() == Nz::Vector2f::Zero());

				CHECK(body.IsKinematic() == false);
				CHECK(body.IsSleeping() == false);
			}
		}

		WHEN("We set a velocity")
		{
			Nz::Vector2f velocity(Nz::Vector2f::Unit());
			body.SetVelocity(velocity);
			position += velocity;
			world.Step(Nz::Time::Second());

			THEN("We expect those to be true")
			{
				aabb.Translate(velocity);
				CHECK(body.GetAABB() == aabb);
				CHECK(body.GetMassCenter(Nz::CoordSys::Global) == position);
				CHECK(body.GetPosition() == position);
				CHECK(body.GetVelocity() == velocity);
			}

			AND_THEN("We apply an impulse in the opposite direction")
			{
				body.AddImpulse(-velocity);
				world.Step(Nz::Time::Second());

				REQUIRE(body.GetVelocity() == Nz::Vector2f::Zero());
			}
		}

		WHEN("We set an angular velocity")
		{
			Nz::RadianAnglef angularSpeed = Nz::RadianAnglef::FromDegrees(90.f);
			body.SetAngularVelocity(angularSpeed);

			world.Step(Nz::Time::Second());

			THEN("We expect those to be true")
			{
				CHECK(body.GetAngularVelocity().ApproxEqual(angularSpeed));
				CHECK(body.GetRotation().ApproxEqual(angularSpeed));
				CHECK(body.GetAABB().ApproxEqual(Nz::Rectf(-6.f, 3.f, 2.f, 1.f), 0.00001f));

				world.Step(Nz::Time::Second());
				CHECK(body.GetRotation().ApproxEqual(2.f * angularSpeed));
				CHECK(body.GetAABB().ApproxEqual(Nz::Rectf(-4.f, -6.f, 1.f, 2.f), 0.00001f));

				world.Step(Nz::Time::Second());
				CHECK(body.GetRotation().ApproxEqual(3.f * angularSpeed));
				CHECK(body.GetAABB().ApproxEqual(Nz::Rectf(4.f, -4.f, 2.f, 1.f), 0.00001f));

				world.Step(Nz::Time::Second());
				CHECK(body.GetRotation().ApproxEqual(4.f * angularSpeed));
			}
		}

		WHEN("We apply a torque")
		{
			body.AddTorque(Nz::DegreeAnglef(90.f));
			world.Step(Nz::Time::Second());

			THEN("It is also counter-clockwise")
			{
				CHECK(body.GetAngularVelocity().value >= 0.f);
				CHECK(body.GetRotation().value >= 0.f);
			}
		}
	}

	GIVEN("A physic world and a rigid body of circle")
	{
		Nz::PhysWorld2D world;
		world.SetMaxStepCount(std::numeric_limits<std::size_t>::max());

		Nz::Vector2f position(3.f, 4.f);
		float radius = 5.f;

		std::shared_ptr<Nz::Collider2D> circle = std::make_shared<Nz::CircleCollider2D>(radius, position);

		Nz::RigidBody2D body(world, Nz::RigidBody2D::DynamicSettings({}, 1.f));
		body.SetCollider(circle, true, false);

		world.Step(Nz::Time::Second());

		WHEN("We ask for the aabb of the circle")
		{
			THEN("We expect this to be true")
			{
				Nz::Rectf circleAABB(position.x - radius, position.y - radius, 2.f * radius, 2.f* radius);
				REQUIRE(body.GetAABB() == circleAABB);
			}
		}
	}

	GIVEN("A physic world and a rigid body of compound")
	{
		Nz::PhysWorld2D world;
		world.SetMaxStepCount(std::numeric_limits<std::size_t>::max());

		Nz::Rectf aabb(0.f, 0.f, 1.f, 1.f);
		std::shared_ptr<Nz::BoxCollider2D> box1 = std::make_shared<Nz::BoxCollider2D>(aabb);
		aabb.Translate(Nz::Vector2f::Unit());
		std::shared_ptr<Nz::BoxCollider2D> box2 = std::make_shared<Nz::BoxCollider2D>(aabb);

		std::vector<std::shared_ptr<Nz::Collider2D>> colliders;
		colliders.push_back(box1);
		colliders.push_back(box2);
		std::shared_ptr<Nz::CompoundCollider2D> compound = std::make_shared<Nz::CompoundCollider2D>(colliders);

		float mass = 1.f;
		Nz::RigidBody2D body(world, Nz::RigidBody2D::DynamicSettings({}, mass));
		body.SetCollider(compound, true, false);

		world.Step(Nz::Time::Second());

		WHEN("We ask for the aabb of the compound")
		{
			THEN("We expect this to be true")
			{
				Nz::Rectf compoundAABB(0.f, 0.f, 2.f, 2.f);
				REQUIRE(body.GetAABB() == compoundAABB);
			}
		}
	}

	GIVEN("A physic world and a rigid body of circle")
	{
		Nz::PhysWorld2D world;
		world.SetMaxStepCount(std::numeric_limits<std::size_t>::max());

		std::vector<Nz::Vector2f> vertices;
		vertices.emplace_back(0.f, 0.f);
		vertices.emplace_back(0.f, 1.f);
		vertices.emplace_back(1.f, 1.f);
		vertices.emplace_back(1.f, 0.f);

		Nz::SparsePtr<const Nz::Vector2f> sparsePtr(vertices.data());
		std::shared_ptr<Nz::ConvexCollider2D> convex = std::make_shared<Nz::ConvexCollider2D>(sparsePtr, vertices.size());
		float mass = 1.f;
		Nz::RigidBody2D body(world, Nz::RigidBody2D::DynamicSettings({}, mass));
		body.SetCollider(convex, true, false);

		world.Step(Nz::Time::Second());

		WHEN("We ask for the aabb of the convex")
		{
			THEN("We expect this to be true")
			{
				Nz::Rectf convexAABB(0.f, 0.f, 1.f, 1.f);
				REQUIRE(body.GetAABB() == convexAABB);
			}
		}
	}

	GIVEN("A physic world and a rigid body of segment")
	{
		Nz::PhysWorld2D world;
		world.SetMaxStepCount(std::numeric_limits<std::size_t>::max());

		Nz::Vector2f positionA(3.f, 4.f);
		Nz::Vector2f positionB(1.f, -4.f);
		std::shared_ptr<Nz::Collider2D> segment = std::make_shared<Nz::SegmentCollider2D>(positionA, positionB, 0.f);
		float mass = 1.f;
		Nz::RigidBody2D body(world, Nz::RigidBody2D::DynamicSettings({}, mass));
		body.SetCollider(segment, true, false);

		world.Step(Nz::Time::Second());

		WHEN("We ask for the aabb of the segment")
		{
			THEN("We expect this to be true")
			{
				Nz::Rectf segmentAABB = Nz::Rectf::FromExtends(positionA, positionB);
				REQUIRE(body.GetAABB() == segmentAABB);
			}
		}
	}
}

Nz::RigidBody2D CreateBody(Nz::PhysWorld2D& world)
{
	Nz::Vector2f positionAABB(3.f, 4.f);
	Nz::Rectf aabb(positionAABB.x, positionAABB.y, 1.f, 2.f);
	std::shared_ptr<Nz::Collider2D> box = std::make_shared<Nz::BoxCollider2D>(aabb);
	float mass = 1.f;

	Nz::RigidBody2D body(world, Nz::RigidBody2D::DynamicSettings(box, mass));
	body.SetPosition(Nz::Vector2f::Zero());

	return body;
}

void EQUALITY(const Nz::RigidBody2D& left, const Nz::RigidBody2D& right)
{
	CHECK(left.GetAABB() == right.GetAABB());
	CHECK(left.GetAngularVelocity() == right.GetAngularVelocity());
	CHECK(left.GetMassCenter() == right.GetMassCenter());
	CHECK(left.GetCollider() == right.GetCollider());
	CHECK(left.GetHandle() != right.GetHandle());
	CHECK(left.GetMass() == Catch::Approx(right.GetMass()));
	CHECK(left.GetPosition() == right.GetPosition());
	CHECK(left.GetRotation().value == Catch::Approx(right.GetRotation().value));
	CHECK(left.GetVelocity() == right.GetVelocity());
}

void EQUALITY(const Nz::Rectf& left, const Nz::Rectf& right)
{
	CHECK(left.x == Catch::Approx(right.x));
	CHECK(left.y == Catch::Approx(right.y));
	CHECK(left.width == Catch::Approx(right.width));
	CHECK(left.height == Catch::Approx(right.height));
}
