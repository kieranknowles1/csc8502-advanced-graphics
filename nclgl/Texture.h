#pragma once

#include <memory>
#include <string>

#include <glad/glad.h>

class Shader;
class Texture;

namespace TextureUnit
{
    class Unit
    {
    public:
        constexpr Unit(GLenum unit, const char* uniform) : unit(unit), uniform(uniform) {};

        constexpr GLenum enumValue() const { return unit; }
        constexpr GLint intValue() const { return unit - GL_TEXTURE0; }

        void bind(const Shader& shader, const Texture& texture) const;
    private:
        GLenum unit;
        const char* uniform;
	};

    const Unit Diffuse = Unit(GL_TEXTURE0, "diffuseTex");
    const Unit Normal = Unit(GL_TEXTURE1, "bumpTex");
    const Unit Noise = Unit(GL_TEXTURE2, "noiseTex");
}

struct TextureKey
{
    std::string name;
    int soilFlags;
    bool repeat;

    bool operator<(const TextureKey& other) const
    {
        if (name != other.name) return name < other.name;
        if (soilFlags != other.soilFlags) return soilFlags < other.soilFlags;
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
    void bind() const;

private:
    std::string name;
    GLenum type;
    GLuint texture;
};
