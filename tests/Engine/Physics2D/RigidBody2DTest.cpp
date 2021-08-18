#include <Nazara/Physics2D/RigidBody2D.hpp>
#include <Nazara/Physics2D/PhysWorld2D.hpp>
#include <catch2/catch.hpp>
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

		Nz::Vector2f positionAABB(3.f, 4.f);
		Nz::Rectf aabb(positionAABB.x, positionAABB.y, 1.f, 2.f);
		std::shared_ptr<Nz::Collider2D> box = std::make_shared<Nz::BoxCollider2D>(aabb);
		float mass = 1.f;
		Nz::RigidBody2D body(&world, mass, box);
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
		bool userdata = false;
		body.SetUserdata(&userdata);

		world.Step(1.f);

		WHEN("We copy construct the body")
		{
			body.AddForce(Nz::Vector2f(3.f, 5.f));
			Nz::RigidBody2D copiedBody(body);
			EQUALITY(copiedBody, body);
			world.Step(1.f);
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
			Nz::RigidBody2D copiedBody(&world, 0.f);
			copiedBody = body;
			EQUALITY(copiedBody, body);
		}

		WHEN("We move assign the body")
		{
			Nz::RigidBody2D copiedBody(body);
			Nz::RigidBody2D movedBody(&world, 0.f);
			movedBody = std::move(body);
			EQUALITY(movedBody, copiedBody);
		}

		WHEN("We set a new geometry")
		{
			float radius = 5.f;
			body.SetGeom(std::make_shared<Nz::CircleCollider2D>(radius));

			world.Step(1.f);

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

			world.Step(1.f);

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
		std::shared_ptr<Nz::Collider2D> box = std::make_shared<Nz::BoxCollider2D>(aabb);
		float mass = 1.f;
		Nz::RigidBody2D body(&world, mass);
		body.SetGeom(box, true, false);

		bool userData = false;
		body.SetUserdata(&userData);

		Nz::Vector2f position = Nz::Vector2f::Zero();
		body.SetPosition(position);

		world.Step(1.f);

		WHEN("We retrieve standard information")
		{
			THEN("We expect those to be true")
			{
				CHECK(body.GetAABB() == aabb);
				CHECK(body.GetAngularVelocity() == 0.f);
				CHECK(body.GetMassCenter(Nz::CoordSys::Global) == position);
				CHECK(body.GetGeom() == box);
				CHECK(body.GetMass() == Approx(mass));
				CHECK(body.GetPosition() == position);
				CHECK(body.GetRotation().value == Approx(0.f));
				CHECK(body.GetUserdata() == &userData);
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
			world.Step(1.f);

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
				world.Step(1.f);

				REQUIRE(body.GetVelocity() == Nz::Vector2f::Zero());
			}
		}

		WHEN("We set an angular velocity")
		{
			Nz::RadianAnglef angularSpeed = Nz::RadianAnglef::FromDegrees(90.f);
			body.SetAngularVelocity(angularSpeed);
			world.Step(1.f);

			THEN("We expect those to be true")
			{
				CHECK(body.GetAngularVelocity() == angularSpeed);
				CHECK(body.GetRotation() == angularSpeed);
				CHECK(body.GetAABB() == Nz::Rectf(-6.f, 3.f, 2.f, 1.f));

				world.Step(1.f);
				CHECK(body.GetRotation() == 2.f * angularSpeed);
				CHECK(body.GetAABB() == Nz::Rectf(-4.f, -6.f, 1.f, 2.f));

				world.Step(1.f);
				CHECK(body.GetRotation() == 3.f * angularSpeed);
				CHECK(body.GetAABB() == Nz::Rectf(4.f, -4.f, 2.f, 1.f));

				world.Step(1.f);
				CHECK(body.GetRotation() == 4.f * angularSpeed);
			}
		}

		WHEN("We apply a torque")
		{
			body.AddTorque(Nz::DegreeAnglef(90.f));
			world.Step(1.f);

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
		float mass = 1.f;
		Nz::RigidBody2D body(&world, mass);
		body.SetGeom(circle, true, false);

		world.Step(1.f);

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
		Nz::RigidBody2D body(&world, mass);
		body.SetGeom(compound, true, false);

		world.Step(1.f);

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
		Nz::RigidBody2D body(&world, mass);
		body.SetGeom(convex, true, false);

		world.Step(1.f);

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
		Nz::RigidBody2D body(&world, mass);
		body.SetGeom(segment, true, false);

		world.Step(1.f);

		WHEN("We ask for the aabb of the segment")
		{
			THEN("We expect this to be true")
			{
				Nz::Rectf segmentAABB(positionA, positionB);
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

	Nz::RigidBody2D body(&world, mass, box);
	body.SetPosition(Nz::Vector2f::Zero());

	return body;
}

void EQUALITY(const Nz::RigidBody2D& left, const Nz::RigidBody2D& right)
{
	CHECK(left.GetAABB() == right.GetAABB());
	CHECK(left.GetAngularVelocity() == right.GetAngularVelocity());
	CHECK(left.GetMassCenter() == right.GetMassCenter());
	CHECK(left.GetGeom() == right.GetGeom());
	CHECK(left.GetHandle() != right.GetHandle());
	CHECK(left.GetMass() == Approx(right.GetMass()));
	CHECK(left.GetPosition() == right.GetPosition());
	CHECK(left.GetRotation().value == Approx(right.GetRotation().value));
	CHECK(left.GetUserdata() == right.GetUserdata());
	CHECK(left.GetVelocity() == right.GetVelocity());
}

void EQUALITY(const Nz::Rectf& left, const Nz::Rectf& right)
{
	CHECK(left.x == Approx(right.x));
	CHECK(left.y == Approx(right.y));
	CHECK(left.width == Approx(right.width));
	CHECK(left.height == Approx(right.height));
}
