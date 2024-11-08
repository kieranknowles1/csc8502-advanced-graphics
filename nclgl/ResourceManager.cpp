#include "ResourceManager.h"

#include <filesystem>
#include <iostream>

ResourceManager::ResourceManager()
{
#ifdef _WIN32
	// FIXME: This is a hack to get the working directory to be the root of the project
	auto workdir = std::filesystem::current_path();
	std::cout << "Original working directory: " << workdir << std::endl;
	// coursework -> build -> project root
	std::filesystem::current_path(workdir.parent_path().parent_path());
#endif // _WIN32
	auto pwd = std::filesystem::current_path().string();
	std::cout << "Using working directory: " << pwd << std::endl;
}
