#include "globals.hlsl"

// [0] - begin offset
// [1] - particles count
// [2] - death particles count
RWBuffer<uint> particles_range : register(u2);

struct DrawInstancedIndirectArgs
{
    uint vertex_count_per_instance;
    uint instance_count;
    uint start_vertex_location;
    uint start_instance_location;
};

RWStructuredBuffer<DrawInstancedIndirectArgs> sparks_indirect_args : register(u3);

[numthreads(1, 1, 1)]
void computeShader()
{
    particles_range[0] = (particles_range[0] + particles_range[2]) % g_sparks_data_buffer_size;
    particles_range[1] -= particles_range[2];
    particles_range[2] = 0;
    
    sparks_indirect_args[0].vertex_count_per_instance = 6;
    sparks_indirect_args[0].instance_count = particles_range[1];
    sparks_indirect_args[0].start_vertex_location = 0;
    sparks_indirect_args[0].start_instance_location = 0;
}
