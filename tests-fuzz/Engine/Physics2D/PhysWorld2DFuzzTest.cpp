#include <Nazara/Physics2D/PhysWorld2D.hpp>
#include <catch2/catch.hpp>

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size) {
	float arg = *(const_cast<uint8_t *>(data));
	Nz::Rectf aabb(arg, arg, arg, arg);
	return 0;
}
