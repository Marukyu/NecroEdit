/**
 * NecroEdit v1.0
 */

#include <Client/System/NEApplication.hpp>
#include <string>
#include <vector>

int main(int argc, char *argv[])
{
	// put arguments into vector.
	std::vector<std::string> args(argv, argv+argc);
	
	// run client.
	NEApplication client;
	return client.run(args);
}
