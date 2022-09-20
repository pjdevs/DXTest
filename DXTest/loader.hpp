#pragma once

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "MeshData.hpp"
#include "Texture.hpp"
#include "stb_image.h"

MeshData* loadMeshData(const std::string& path);
Texture* loadTexture(const GraphicsDevice& device, const std::string& path);
Texture* loadTextureHDR(const GraphicsDevice& device, const std::string& path);