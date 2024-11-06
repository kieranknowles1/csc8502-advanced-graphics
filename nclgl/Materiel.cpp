#include "Materiel.h"

#include "OGLRenderer.h"
#include "MeshMaterial.h"
#include "Texture.h"
#include "ResourceManager.h"

std::vector<Materiel> Materiel::fromFile(ResourceManager* rm, const std::string& filename) {
    std::vector<Materiel> result;
    MeshMaterial file(filename);
    for (int i = 0; i < file.getLayerCount(); i++) {
        auto layer = file.GetMaterialForLayer(i);

        std::string out;
        std::shared_ptr<Texture> diffuse = nullptr;
        std::shared_ptr<Texture> normal = nullptr;
        GLenum cullMode = Materiel::DefaultCullMode;
        //std::shared_ptr<Texture> noise = nullptr;

        if (layer->GetEntry("Diffuse", out)) {
            diffuse = rm->getTextures().get({ out, SOIL_FLAG_MIPMAPS, true });
        }
        if (layer->GetEntry("Bump", out)) {
			normal = rm->getTextures().get({ out, SOIL_FLAG_MIPMAPS, true });
		}
        if (layer->GetEntry("Cull", out)) {
            if (out == "GL_FRONT") {
                cullMode = GL_FRONT;
            }
            else if (out == "GL_BACK") {
                cullMode = GL_BACK;
            }
			else if (out == "GL_NONE") {
				cullMode = GL_NONE;
			}
            else throw std::runtime_error("Unknown cull mode: " + out);
        }
        Materiel m;
        m.diffuse = diffuse;
        m.normal = normal;
        m.cullMode = cullMode;
        result.push_back(m);
    }
    return result;
}

void Materiel::bind(OGLRenderer& r, const Materiel& defaults) const {
    auto shader = this->shader ? this->shader : defaults.shader;
    r.BindShader(shader.get());
    r.UpdateShaderMatrices();

    auto& diffuse = this->diffuse ? this->diffuse : defaults.diffuse;
    if (diffuse != nullptr)
        TextureUnit::Diffuse.bind(*shader, *diffuse);

    auto& normal = this->normal ? this->normal : defaults.normal;
    if (normal != nullptr)
        TextureUnit::Normal.bind(*shader, *normal);

    auto& noise = this->noise ? this->noise : defaults.noise;
    if (noise != nullptr)
		TextureUnit::Noise.bind(*shader, *noise);

    if (cullMode == GL_NONE) {
		glDisable(GL_CULL_FACE);
	}
	else {
		glEnable(GL_CULL_FACE);
		glCullFace(cullMode);
	}
}
