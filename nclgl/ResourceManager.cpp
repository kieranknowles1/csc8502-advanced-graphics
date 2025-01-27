#include "ResourceManager.h"

#include <filesystem>
#include <iostream>

ResourceManager::ResourceManager()
{
	auto pwd = std::filesystem::current_path().string();
	std::cout << "Using working directory: " << pwd << std::endl;
}
