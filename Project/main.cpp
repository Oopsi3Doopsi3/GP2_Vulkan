#include "GP2Base.h"

//std
#include <cstdlib>
#include <iostream>
#include <stdexcept>

int main() 
{
	GP2::GP2Base base;

	try{
		base.Run();
	}
	catch (const std::exception &e)
	{
		std::cerr << e.what() << "\n";
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}