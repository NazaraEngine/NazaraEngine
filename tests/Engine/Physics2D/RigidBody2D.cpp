#include <Nazara/Physics2D/RigidBody2D.hpp>
#include <Nazara/Physics2D/PhysWorld2D.hpp>
#include <Catch/catch.hpp>

Nz::RigidBody2D CreateBody(Nz::PhysWorld2D& world);
void EQUALITY(const Nz::RigidBody2D& left, const Nz::RigidBody2D& right);

SCENARIO("RigidBody2D", "[PHYSICS2D][RIGIDBODY2D]")
{
	GIVEN("A physic world and a rigid body")
	{
		Nz::PhysWorld2D world;

		Nz::Vector2f positionAABB(3.f, 4.f);
		Nz::Rectf aabb(positionAABB.x, positionAABB.y, 1.f, 2.f);
		Nz::Collider2DRef box = Nz::BoxCollider2D::New(aabb);
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
			Nz::Vector2f positionCircle(0.f, 0.f);
			Nz::Collider2DRef circle = Nz::CircleCollider2D::New(radius, position);
			body.SetGeom(circle);

			world.Step(1.f);

			THEN("The aabb should be updated")
			{
				Nz::Rectf circleAABB(position.x - radius, position.y - radius, 2.f * radius, 2.f* radius);
				REQUIRE(body.GetAABB() == circleAABB);
			}
		}
	}

	GIVEN("A physic world")
	{
		Nz::PhysWorld2D world;
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
		Nz::Vector2f positionAABB(3.f, 4.f);
		Nz::Rectf aabb(positionAABB.x, positionAABB.y, 1.f, 2.f);
		Nz::Collider2DRef box = Nz::BoxCollider2D::New(aabb);
		float mass = 1.f;
		Nz::RigidBody2D body(&world, mass, box);
		bool userData = false;
		body.SetUserdata(&userData);

		Nz::Vector2f position = Nz::Vector2f::Zero();

		world.Step(1.f);

		WHEN("We retrieve standard information")
		{
			THEN("We expect those to be true")
			{
				CHECK(body.GetAABB() == aabb);
				CHECK(body.GetAngularVelocity() == Approx(0.f));
				CHECK(body.GetCenterOfGravity() == Nz::Vector2f::Zero());
				CHECK(body.GetGeom() == box);
				CHECK(body.GetMass() == Approx(mass));
				CHECK(body.GetPosition() == position);
				CHECK(body.GetRotation() == Approx(0.f));
				CHECK(body.GetUserdata() == &userData);
				CHECK(body.GetVelocity() == Nz::Vector2f::Zero());

				CHECK(body.IsMoveable() == true);
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
				CHECK(body.GetCenterOfGravity() == Nz::Vector2f::Zero());
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
			float angularSpeed = Nz::FromDegrees(90.f);
			body.SetAngularVelocity(angularSpeed);
			world.Step(1.f);

			THEN("We expect those to be true")
			{
				CHECK(body.GetAngularVelocity() == Approx(angularSpeed));
				CHECK(body.GetRotation() == Approx(angularSpeed));
				CHECK(body.GetAABB() == Nz::Rectf(-6.f, 3.f, 2.f, 1.f));

				world.Step(1.f);
				CHECK(body.GetRotation() == Approx(2.f * angularSpeed));
				CHECK(body.GetAABB() == Nz::Rectf(-4.f, -6.f, 1.f, 2.f));

				world.Step(1.f);
				CHECK(body.GetRotation() == Approx(3.f * angularSpeed));
				CHECK(body.GetAABB() == Nz::Rectf(4.f, -4.f, 2.f, 1.f));

				world.Step(1.f);
				CHECK(body.GetRotation() == Approx(4.f * angularSpeed));
			}
		}

		WHEN("We apply a torque")
		{
			float angularSpeed = Nz::DegreeToRadian(90.f);
			body.AddTorque(angularSpeed);
			world.Step(1.f);

			THEN("It is also counter-clockwise")
			{
				CHECK(body.GetAngularVelocity() >= 0.f);
				CHECK(body.GetRotation() >= 0.f);
			}
		}
	}

	GIVEN("A physic world and a rigid body of circle")
	{
		Nz::PhysWorld2D world;

		Nz::Vector2f position(3.f, 4.f);
		float radius = 5.f;
		Nz::Collider2DRef circle = Nz::CircleCollider2D::New(radius, position);
		float mass = 1.f;
		Nz::RigidBody2D body(&world, mass, circle);
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

	GIVEN("A physic world and a rigid body of segment")
	{
		Nz::PhysWorld2D world;

		Nz::Vector2f positionA(3.f, 4.f);
		Nz::Vector2f positionB(1.f, -4.f);
		Nz::Collider2DRef segment = Nz::SegmentCollider2D::New(positionA, positionB, 0.f);
		float mass = 1.f;
		Nz::RigidBody2D body(&world, mass, segment);
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
	Nz::Collider2DRef box = Nz::BoxCollider2D::New(aabb);
	float mass = 1.f;
	return Nz::RigidBody2D(&world, mass, box);
}

void EQUALITY(const Nz::RigidBody2D& left, const Nz::RigidBody2D& right)
{
	CHECK(left.GetAABB() == right.GetAABB());
	CHECK(left.GetAngularVelocity() == right.GetAngularVelocity());
	CHECK(left.GetCenterOfGravity() == right.GetCenterOfGravity());
	CHECK(left.GetGeom() == right.GetGeom());
	CHECK(left.GetHandle() != right.GetHandle());
	CHECK(left.GetMass() == right.GetMass());
	CHECK(left.GetPosition() == right.GetPosition());
	CHECK(left.GetRotation() == right.GetRotation());
	CHECK(left.GetUserdata() == right.GetUserdata());
	CHECK(left.GetVelocity() == right.GetVelocity());
}