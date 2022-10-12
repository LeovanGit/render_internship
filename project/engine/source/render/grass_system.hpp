#ifndef GRASS_SYSTEM_HPP
#define GRASS_SYSTEM_HPP

#include "glm.hpp"
#include <vector>

#include "shader.hpp"
#include "texture.hpp"
#include "grass_field.hpp"
#include "vertex_buffer.hpp"

namespace engine
{
class GrassSystem
{
public:
    // deleted methods should be public for better error messages
    GrassSystem(const GrassSystem & other) = delete;
    void operator=(const GrassSystem & other) = delete;
    
    static void init();

    static GrassSystem * getInstance();

    static void del();

    void addGrassField(const GrassField & grass_field);

    void updateInstanceBuffer();
    void render();

    // move them to GrassField class for different grass:
    std::shared_ptr<Shader> shader;
    std::shared_ptr<Texture> albedo;
    std::shared_ptr<Texture> opacity;
    
private:
    GrassSystem() = default;
    ~GrassSystem() = default;

    static GrassSystem * instance;
       
    struct GPUInstance
    {
        GPUInstance(const glm::vec3 & position,
                    glm::vec2 size) :
            position(position),
            size(size)
        {}

        glm::vec3 position;
        glm::vec2 size;
    };

    VertexBuffer<GPUInstance> instance_buffer;
    
    std::vector<GrassField> grass_fields;
};
} // namespace engine

#endif
