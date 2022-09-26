#pragma once

#include <string>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "MeshData.hpp"
#include "Texture.hpp"
#include "stb_image.h"

MeshData* loadMeshData(const std::string& path, bool flip = false);
Texture* loadTexture(const GraphicsDevice& device, const std::string& path, bool alpha = false);
Texture* loadTextureHDR(const GraphicsDevice& device, const std::string& path);