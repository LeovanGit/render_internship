#ifndef SHADER_INCLUDER_HPP
#define SHADER_INCLUDER_HPP

#include <iostream>
#include <filesystem>
#include <d3d11_4.h>
#include <vector>
#include <fstream>

namespace fs = std::filesystem;
	
class ShaderIncluder : public ID3DInclude
{
public:
    ShaderIncluder(const std::string & currentDir);

    HRESULT Open(D3D_INCLUDE_TYPE includeType,
                 const char* relativePath,
                 LPCVOID pParentData,
                 const void** outData,
                 uint32_t* outBytes) override;

    HRESULT Close(const void* pData) override;

private:
    fs::path m_currentDir;
    std::vector<uint8_t> m_includeLevels;
		
    using FileData = std::vector<char>;
    std::vector<FileData> m_openedFiles;

    void readFile(FileData & filedata);
};

#endif
