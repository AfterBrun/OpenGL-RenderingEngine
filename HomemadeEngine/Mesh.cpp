#include "Mesh.h"

std::unique_ptr<Mesh> Mesh::Create(std::vector<Vertex>& vertex, std::vector<uint32_t>& indices) {
	auto MeshInstance = std::unique_ptr<Mesh>(new Mesh());
	MeshInstance->Init(vertex, indices);
	return std::move(MeshInstance);
}

void Mesh::Init(std::vector<Vertex>& vertex, std::vector<uint32_t>& indices) {
    m_indicesCount = indices.size();
    
    if (m_primitiveType == GL_TRIANGLES) {
        CalcTangent(vertex, indices);
    }
    

	m_vao = vertexLayout::CreateVertexLayout();
	m_vbo = buffer::CreateBuffer(GL_ARRAY_BUFFER, vertex.data(), size_t(vertex.size() * sizeof(Vertex)), GL_STATIC_DRAW);
    m_ebo = buffer::CreateBuffer(GL_ELEMENT_ARRAY_BUFFER, indices.data(), size_t(indices.size() * sizeof(uint32_t)), GL_STATIC_DRAW);

	m_vao->SetAttributePointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
	m_vao->SetAttributePointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), offsetof(Vertex, Normal));
	m_vao->SetAttributePointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), offsetof(Vertex, TexCoord));
    m_vao->SetAttributePointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), offsetof(Vertex, Tangent));
}

void Mesh::Draw(const ShaderProgram* program) {
    //program->Use();
    m_vao->Bind();
    unsigned int diffuseNr = 1;
    unsigned int specularNr = 1;
    unsigned int normalNR = 1;
    for (int i = 0; i < m_materials.size(); i++) {
        glActiveTexture(GL_TEXTURE0 + i);
        std::string number;
        std::string type = m_materials[i]->GetTypeString();
        if (type == "texture_diffuse") {
            number = std::to_string(diffuseNr);
            diffuseNr++;
        }
        else if (type == "texture_specular") {
            number = std::to_string(specularNr);
            specularNr++;
        }
        else if (type == "texture_normal") {
            number = std::to_string(specularNr);
            normalNR++;
        }
        program->SetUniform(("material." + type + number).c_str(), i);
        m_materials[i]->Bind();
    }
    glActiveTexture(GL_TEXTURE0);
    glDrawElements(GL_TRIANGLES, m_indicesCount, GL_UNSIGNED_INT, 0);
}

std::unique_ptr<Mesh> Mesh::CreateBox() {
    std::vector<Vertex> vertices = {
      Vertex { glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec2(0.0f, 0.0f) },
      Vertex { glm::vec3(0.5f, -0.5f, -0.5f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec2(1.0f, 0.0f) },
      Vertex { glm::vec3(0.5f,  0.5f, -0.5f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec2(1.0f, 1.0f) },
      Vertex { glm::vec3(-0.5f,  0.5f, -0.5f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec2(0.0f, 1.0f) },

      Vertex { glm::vec3(-0.5f, -0.5f,  0.5f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec2(0.0f, 0.0f) },
      Vertex { glm::vec3(0.5f, -0.5f,  0.5f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec2(1.0f, 0.0f) },
      Vertex { glm::vec3(0.5f,  0.5f,  0.5f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec2(1.0f, 1.0f) },
      Vertex { glm::vec3(-0.5f,  0.5f,  0.5f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec2(0.0f, 1.0f) },

      Vertex { glm::vec3(-0.5f,  0.5f,  0.5f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec2(1.0f, 0.0f) },
      Vertex { glm::vec3(-0.5f,  0.5f, -0.5f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec2(1.0f, 1.0f) },
      Vertex { glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec2(0.0f, 1.0f) },
      Vertex { glm::vec3(-0.5f, -0.5f,  0.5f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec2(0.0f, 0.0f) },

      Vertex { glm::vec3(0.5f,  0.5f,  0.5f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec2(1.0f, 0.0f) },
      Vertex { glm::vec3(0.5f,  0.5f, -0.5f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec2(1.0f, 1.0f) },
      Vertex { glm::vec3(0.5f, -0.5f, -0.5f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec2(0.0f, 1.0f) },
      Vertex { glm::vec3(0.5f, -0.5f,  0.5f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec2(0.0f, 0.0f) },

      Vertex { glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec2(0.0f, 1.0f) },
      Vertex { glm::vec3(0.5f, -0.5f, -0.5f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec2(1.0f, 1.0f) },
      Vertex { glm::vec3(0.5f, -0.5f,  0.5f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec2(1.0f, 0.0f) },
      Vertex { glm::vec3(-0.5f, -0.5f,  0.5f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec2(0.0f, 0.0f) },

      Vertex { glm::vec3(-0.5f,  0.5f, -0.5f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec2(0.0f, 1.0f) },
      Vertex { glm::vec3(0.5f,  0.5f, -0.5f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec2(1.0f, 1.0f) },
      Vertex { glm::vec3(0.5f,  0.5f,  0.5f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec2(1.0f, 0.0f) },
      Vertex { glm::vec3(-0.5f,  0.5f,  0.5f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec2(0.0f, 0.0f) },
    };

    std::vector<uint32_t> indices = {
       0,  2,  1,  2,  0,  3,
       4,  5,  6,  6,  7,  4,
       8,  9, 10, 10, 11,  8,
      12, 14, 13, 14, 12, 15,
      16, 17, 18, 18, 19, 16,
      20, 22, 21, 22, 20, 23,
    };

    return Create(vertices, indices);
}

std::unique_ptr<Mesh> Mesh::CreatePlane() {
    std::vector<Vertex> vertices = {
      Vertex { glm::vec3(-0.5f, -0.5f, 0.0f), glm::vec3(0.0f,  0.0f, 1.0f), glm::vec2(0.0f, 0.0f) },
      Vertex { glm::vec3(0.5f, -0.5f, 0.0f), glm::vec3(0.0f,  0.0f, 1.0f), glm::vec2(1.0f, 0.0f) },
      Vertex { glm::vec3(0.5f,  0.5f, 0.0f), glm::vec3(0.0f,  0.0f, 1.0f), glm::vec2(1.0f, 1.0f) },
      Vertex { glm::vec3(-0.5f,  0.5f, 0.0f), glm::vec3(0.0f,  0.0f, 1.0f), glm::vec2(0.0f, 1.0f) },
    };

    std::vector<uint32_t> indices = {
      0,  1,  2,  2,  3,  0,
    };

    return Create(vertices, indices);
}

void Mesh::CalcTangent(std::vector<Vertex>& vertices, std::vector<uint32_t>& indices)
{
    auto compute = [](
        const glm::vec3& pos1, const glm::vec3& pos2, const glm::vec3& pos3,
        const glm::vec2& uv1, const glm::vec2& uv2, const glm::vec2& uv3)
        -> glm::vec3 {

        auto edge1 = pos2 - pos1;
        auto edge2 = pos3 - pos1;
        auto deltaUV1 = uv2 - uv1;
        auto deltaUV2 = uv3 - uv1;
        float det = (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);
        if (det != 0.0f) {
            auto invDet = 1.0f / det;
            return deltaUV2.y * edge1 - deltaUV1.y * edge2;
        }
        else {
            return glm::vec3(0.0f, 0.0f, 0.0f);
        }
    };

    // initialize
    std::vector<glm::vec3> tangents;
    tangents.resize(vertices.size());
    memset(tangents.data(), 0, tangents.size() * sizeof(glm::vec3));

    // accumulate triangle tangents to each vertex
    for (size_t i = 0; i < indices.size(); i += 3) {
        auto v1 = indices[i];
        auto v2 = indices[i + 1];
        auto v3 = indices[i + 2];

        tangents[v1] += compute(
            vertices[v1].Position, vertices[v2].Position, vertices[v3].Position,
            vertices[v1].TexCoord, vertices[v2].TexCoord, vertices[v3].TexCoord);

        tangents[v2] = compute(
            vertices[v2].Position, vertices[v3].Position, vertices[v1].Position,
            vertices[v2].TexCoord, vertices[v3].TexCoord, vertices[v1].TexCoord);

        tangents[v3] = compute(
            vertices[v3].Position, vertices[v1].Position, vertices[v2].Position,
            vertices[v3].TexCoord, vertices[v1].TexCoord, vertices[v2].TexCoord);
    }

    // normalize
    for (size_t i = 0; i < vertices.size(); i++) {
        vertices[i].Tangent = glm::normalize(tangents[i]);
    }
}

/*
void Mesh::DrawBox(ShaderProgram* program) {
    program->Use();
	m_vao->Bind();
    if (!m_materials.empty()) {
        glActiveTexture(GL_TEXTURE0);
        m_materials[0]->Bind();
        program->SetUniform("material.texture_diffuse1", 0);

        glActiveTexture(GL_TEXTURE1);
        m_materials[1]->Bind();
        program->SetUniform("material.texture_specular1", 1);
    }

	glDrawElements(GL_TRIANGLES, m_indicesCount, GL_UNSIGNED_INT, 0);
}
*/