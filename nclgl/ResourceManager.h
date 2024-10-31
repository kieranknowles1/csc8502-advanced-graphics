#pragma once

#include <map>
#include <string>
#include <memory>
#include <iostream>

// Forward declarations won't work here, as we need the full declaration to generate templates
#include "Texture.h"

template <typename K, typename V>
class ResourceMap
{
public:
    std::shared_ptr<V> get(const K& key)
    {
        auto it = resources.find(key);
        if (it == resources.end() || it->second.expired())
        {
            auto resource = std::make_shared<V>(key);
            resources.emplace(key, resource);
            return resource;
        }

        return it->second.lock();
    }

    ~ResourceMap() {
        for (auto& ptr : resources) {
            if (!ptr.second.expired()) {
                // A resource not being deleted means that either we have a memory leak, or the resource manager wasn't destroyed last
                // The CPP standard specifies that members are destroyed in reverse order of declaration, and the resource manager is needed
                // for anything that uses it, so should be declared first
                std::cerr << "Resource " << ptr.second.lock()->describe() << " not deleted before ResourceManager destruction" << std::endl;
            }
        }
    }
private:
    std::map<K, std::weak_ptr<V>> resources;
};

// Class to keep a cache of loaded resources, and to load them if not already
// Methods may affect the OpenGL state, so should be called from the main thread
// and not during rendering
class ResourceManager
{
public:
    ResourceManager();

    ResourceMap<TextureKey, Texture>& getTextures() { return textures; }
    ResourceMap<std::string, Texture>& getCubeMaps() { return cubeMaps; }
protected:
    // TODO: Shaders need to be pairs of vertex and fragment shaders
    //std::map<std::string, Shader*> shaders;
    // TODO: Manage meshes and anything else we load frequently
    ResourceMap<TextureKey, Texture> textures;
    ResourceMap<std::string, Texture> cubeMaps;
};
