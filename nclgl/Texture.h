#pragma once

#include <memory>
#include <string>

#include <glad/glad.h>

struct TextureKey
{
    std::string name;
    int soilFlags;
    bool repeat;

    bool operator<(const TextureKey& other) const
    {
        if (name < other.name) return true;
        if (name > other.name) return false;

        if (soilFlags < other.soilFlags) return true;
        if (soilFlags > other.soilFlags) return false;

        return repeat < other.repeat;
    }
};

// Texture class that is managed using RAII principles
// That is, the texture is loaded and sent to the GPU in the constructor
// and freed in the destructor
class Texture
{
public:
    const static std::string CubeWestExt;
    const static std::string CubeEastExt;
    const static std::string CubeUpExt;
    const static std::string CubeDownExt;
    const static std::string CubeNorthExt;
    const static std::string CubeSouthExt;

    Texture(const std::string& name, GLenum type, GLuint id) : name(name), type(type), texture(id) {};

    // 2D texture + flags
    Texture(const TextureKey& key);
    // Cube map
    Texture(const std::string& name);

    ~Texture();

    std::string describe() const;

    // Bind the texture to the active texture unit
    void bind();

private:
    std::string name;
    GLenum type;
    GLuint texture;
};
