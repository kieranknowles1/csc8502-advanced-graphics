#include "Materiel.h"

#include "OGLRenderer.h"
#include "MeshMaterial.h"
#include "Texture.h"

Materiel Materiel::fromFile(const std::string& filename) {
    MeshMaterial mat(filename);
    throw new std::runtime_error("Not implemented"); // TODO
}

void Materiel::bind(OGLRenderer& r, const Materiel& defaults) const {
    auto shader = this->shader ? this->shader : defaults.shader;
    r.BindShader(shader.get());
    r.UpdateShaderMatrices();

    auto diffuse = this->diffuse ? this->diffuse : defaults.diffuse;
    if (diffuse != nullptr)
        TextureUnit::Diffuse.bind(*shader, *diffuse);

    auto normal = this->normal ? this->normal : defaults.normal;
    if (normal != nullptr)
        TextureUnit::Normal.bind(*shader, *normal);

    auto noise = this->noise ? this->noise : defaults.noise;
    if (noise != nullptr)
		TextureUnit::Noise.bind(*shader, *noise);
}
