//Made with the help of "Brendan Galea"'s "Vulkan (c++) Game Engine Tutorial" series
//https://www.youtube.com/playlist?list=PL8327DO66nu9qYVKLDmdLW_84-yE4auCR

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