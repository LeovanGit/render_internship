#include "d3d.hpp"

namespace engine
{
ID3D11Device5 * s_device = nullptr;
ID3D11DeviceContext4 * s_device_context = nullptr;
IDXGIFactory5 * s_factory = nullptr;

ID3D11Buffer * s_vbo = nullptr; // Vertex Buffer Object
ID3D11InputLayout * s_input_layout = nullptr;
} // namespace engine
