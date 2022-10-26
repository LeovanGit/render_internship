#ifndef DECAL_SYSTEM
#define DECAL_SYSTEM

#include "glm.hpp"
#include <vector>

#include "vertex_buffer.hpp"
#include "decal.hpp"
#include "shader.hpp"
#include "texture.hpp"
#include "random.hpp"

namespace engine
{
class DecalSystem
{
public:
    // deleted methods should be public for better error messages
    DecalSystem(const DecalSystem & other) = delete;
    void operator=(const DecalSystem & other) = delete;

    static void init();

    static DecalSystem * getInstance();

    static void del();

    void addDecal(const glm::vec3 & position);

    void updateInstanceBuffer();
    void render();

    std::shared_ptr<Shader> shader;
    std::shared_ptr<Texture> normals;
    
private:
    DecalSystem() = default;
    ~DecalSystem() = default;

    static DecalSystem * instance;

    struct GPUInstance
    {
        GPUInstance(const glm::vec3 & position,
                    const glm::vec2 & size,
                    const glm::vec3 & albedo) :
                    position(position),
                    size(size),
                    albedo(albedo)
        {}

        glm::vec3 position;
        glm::vec2 size;
        glm::vec3 albedo;
    };

    VertexBuffer<GPUInstance> instance_buffer;

    std::vector<Decal> decals;
};
} // namespace engine

#endif
