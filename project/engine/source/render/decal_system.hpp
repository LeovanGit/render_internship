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

    void addDecal(uint32_t model_id,
                  uint32_t transform_id,
                  const glm::vec3 & posMS,
                  const glm::vec3 & forward,
                  const glm::vec3 & right,
                  const glm::vec3 & up);

    void updateInstanceBuffer();
    void render(DxResPtr<ID3D11ShaderResourceView> depth_srv,
                DxResPtr<ID3D11ShaderResourceView> normals_srv,
                DxResPtr<ID3D11ShaderResourceView> model_id_srv);

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
                    const glm::vec3 & albedo,
                    const glm::mat4x4 & model,
                    const glm::mat4x4 & model_inv,
                    uint32_t model_id) :
                    position(position),
                    size(size),
                    albedo(albedo),
                    model(model),
                    model_inv(model_inv),
                    model_id(model_id)
        {}
        
        glm::vec3 position;
        glm::vec2 size;
        glm::vec3 albedo;

        glm::mat4x4 model;
        glm::mat4x4 model_inv;

        uint32_t model_id;
    };

    VertexBuffer<GPUInstance> instance_buffer;

    std::vector<Decal> decals;
};
} // namespace engine

#endif
