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

        int flags = SOIL_FLAG_MIPMAPS;
        if (layer->GetEntry("InvertY", out)) {
            if (out == "true") {
                flags |= SOIL_FLAG_INVERT_Y;
            }
        }

        if (layer->GetEntry("Diffuse", out)) {
            diffuse = rm->getTextures().get({ out, flags, true });
        }
        if (layer->GetEntry("Bump", out)) {
			normal = rm->getTextures().get({ out, flags, true });
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

void Materiel::bind(OGLRenderer& r, const Materiel& defaults, bool shadowPass) const {
    // If we're doing a shadow pass then we don't anything other than the shader
    if (shadowPass) {
        auto& shader = this->shadowShader ? this->shadowShader : defaults.shadowShader;
        r.BindShader(shader.get());
        r.UpdateShaderMatrices();
        return;
    }

    auto& shader = this->shader ? this->shader : defaults.shader;
    r.BindShader(shader.get());
    r.UpdateShaderMatrices();

    if (diffuse != nullptr)
		TextureUnit::Diffuse.bind(*shader, *diffuse);
    if (normal != nullptr)
        TextureUnit::Normal.bind(*shader, *normal);
    if (noise != nullptr)
        TextureUnit::Noise.bind(*shader, *noise);

    if (cullMode == GL_NONE) {
		glDisable(GL_CULL_FACE);
	}
	else {
		glEnable(GL_CULL_FACE);
		glCullFace(cullMode);
	}

    glUniform1f(glGetUniformLocation(shader->GetProgram(), "reflectivity"), reflectivity);
}
