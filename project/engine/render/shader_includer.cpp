#include "shader_includer.hpp"

ShaderIncluder::ShaderIncluder(WCHAR* path)
{
    fs::path currentDir(path);
    m_currentDir = currentDir;
}

HRESULT ShaderIncluder::Open(D3D_INCLUDE_TYPE includeType,
                             const char* includePathStr,
                             const void* parentData,
                             const void** outData,
                             uint32_t* outBytes)
{
    fs::path includePath = includePathStr;

    uint8_t includeLevel = 0;
    fs::path tmpPath = includePath;
    while (tmpPath.has_parent_path())
    {
        tmpPath = tmpPath.parent_path();
        includeLevel += 1;
    }
    m_includeLevels.push_back(includeLevel);

    m_currentDir = m_currentDir / includePath;
		
    FileData& fileData = m_openedFiles.emplace_back();
    // FileSystem::readFile(fileData, m_currentDir);
    readFile(fileData);
    *outData = fileData.data();
    *outBytes = uint32_t(fileData.size());
		
    m_currentDir = m_currentDir.parent_path();

    return *outBytes > 0 ? S_OK : E_FAIL;
}

HRESULT ShaderIncluder::Close(const void* data)
{
    for (uint8_t i = 0; i < m_includeLevels.back(); ++i)
        m_currentDir = m_currentDir.parent_path();

    m_includeLevels.pop_back();
    m_openedFiles.pop_back();

    return S_OK;
}

void ShaderIncluder::readFile(FileData & filedata)
{
    std::ifstream file(m_currentDir);

    if (file.is_open())
    {
		char symbol = 0;

		while (file.get(symbol))
		{
            filedata.push_back(symbol);
		}

		file.close();
    }
}
