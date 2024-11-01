#include "Materiel.h"

#include "OGLRenderer.h"
#include "Texture.h"

void Materiel::bind(OGLRenderer& r, const Materiel& defaults) const {
    auto shader = this->shader ? this->shader : defaults.shader;
    r.BindShader(shader.get());
    r.UpdateShaderMatrices();

    auto diffuse = this->diffuse ? this->diffuse : defaults.diffuse;
    TextureUnit::Diffuse.bind(*shader, *diffuse);

    auto normal = this->normal ? this->normal : defaults.normal;
    TextureUnit::Normal.bind(*shader, *normal);
}
