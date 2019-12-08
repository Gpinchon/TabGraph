#include "parser/GLTF.hpp"

int main(int argc, char const *argv[])
{
	if (argc <= 1)
		return -42;
	auto scene(GLTF::Parse(argv[1]));
	return 0;
}