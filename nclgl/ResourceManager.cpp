#include "ResourceManager.h"

#include <filesystem>
#include <iostream>

ResourceManager::ResourceManager()
{
#ifdef _WIN32
	// FIXME: This is a hack to get the working directory to be the root of the project
	std::filesystem::current_path("H:/Documents/src/csc8502-advanced-graphics");
#endif // _WIN32
	auto pwd = std::filesystem::current_path().string();
	std::cout << "Using working directory: " << pwd << std::endl;
}
