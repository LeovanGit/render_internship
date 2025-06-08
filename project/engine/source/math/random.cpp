#include "random.hpp"

namespace math
{
// [min; max)
float randomFromRange(float min, float max)
{
    std::random_device random_device;
    std::mt19937 mersenne_random(random_device());
    
    std::uniform_real_distribution<float> distribution(min, max);
    return distribution(random_device);
}

std::vector<glm::vec2> poissonDiscSampling(uint32_t width,
                                           uint32_t height,
                                           float radius,
                                           uint32_t k)
{
    std::vector<glm::vec2> positions;
    
    float cell_size = radius / sqrtf(2.0f);

    std::vector<glm::vec2> grid(width * height, glm::vec2(-1.0f));

    glm::vec2 random_point(randomFromRange(0.0f, width),
                           randomFromRange(0.0f, height));

    grid[uint32_t(random_point.x / cell_size) +
         uint32_t(random_point.y / cell_size) * width] = random_point;

    std::vector<glm::vec2> active;
    active.push_back(random_point);

    positions.push_back(random_point);
    
    while (active.size() > 0)
    {        
        uint32_t rand_i = uint32_t(randomFromRange(0.0f, active.size()));
        bool found = false;
        
        for (uint32_t i = 0; i != k; ++i)
        {
            float length = randomFromRange(radius, 2 * radius);
            float angle = randomFromRange(0.0f, 2 * PI);

            glm::vec2 sample = active[rand_i] +
                               glm::vec2(cosf(angle), sinf(angle)) * length;

            // check if coordinates belong to the grid
            if (sample.x < 0.0f || sample.x >= int(width) ||
                sample.y < 0.0f || sample.y >= int(height)) continue;
            
            glm::vec<2, int> sample_coord(int(sample.x / cell_size),
                                          int(sample.y / cell_size));
            
            // check if this grid's cell is empty
            if (grid[sample_coord.x + sample_coord.y * width].x >= 0.0f) continue;

            found = true;
            
            for (int row = -1; row <= 1; ++row)
            {
                for (int col = -1; col <= 1; ++col)
                {
                    if (row == 0 && col == 0) continue;                    
                    
                    glm::vec<2, int> neighbor_coord(sample_coord.x + row,
                                                    sample_coord.y + col);

                    // check if coordinates belong to the grid
                    if (neighbor_coord.x < 0.0f || neighbor_coord.x >= int(width) ||
                        neighbor_coord.y < 0.0f || neighbor_coord.y >= int(height)) continue;
                    
                    glm::vec2 neighbor = grid[neighbor_coord.x +
                                              neighbor_coord.y * width];

                    // check if this grid's cell not empty
                    if (neighbor.x >= 0.0f)
                    {
                        float dist = glm::distance(sample, neighbor);
                        if (dist < radius)
                        {
                            found = false;
                            goto next_sample;
                        }
                    }
                }
            }

            if (found)
            {
                grid[sample_coord.x + sample_coord.y * width] = sample;
                active.push_back(sample);
                positions.push_back(sample);
                break;
            }
            
            next_sample: continue;
        }
        if (!found) active.erase(active.begin() + rand_i);
    }

    return positions;
}
} // namespace math
