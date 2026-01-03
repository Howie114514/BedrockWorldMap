#pragma once

#include <filesystem>
#include <leveldb/db.h>
#include <mutex>
#include <nlohmann/json.hpp>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>


namespace bedrock_world_map {
struct Biome {
    int         id;
    std::string type;
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(Biome, id, type)
};
struct Color {
    int r;
    int g;
    int b;
};
struct Point {
    int   x;
    int   y;
    int   z;
    Color color;
    int   biome;
};
struct Chunk {
    int   x;
    int   z;
    Point blocks[256];
    bool  isValid;
};

std::string getChunkId(Chunk chunk);
Chunk       getBlockChunk(Point point);
std::string getBlockIndexInChunk(Point point);
} // namespace bedrock_world_map

namespace bedrock_world_map::storage {
class Storage {
public:
    Storage(std::string levelName, std::filesystem::path path);
    ~Storage();

private:
    std::unordered_map<std::string, Chunk&> cachedChunks;
    std::unordered_map<int, Biome>          biomes;
    std::mutex                              biomesMtx;
    std::string                             levelName;
    std::filesystem::path                   path;
    leveldb::DB*                            db;
    leveldb::Options                        ldbOptions;

public:
    void  writeBiomes();
    void  readBiomes();
    void  putBiome(std::string biomeName);
    void  saveChunk(Chunk chunk);
    void  putBlock(Point point);
    Point getBlock();
    Chunk readChunk(std::string id);
    void  updateChunk(Chunk& chunk);
    void  close();
    void  putChunkToCache(Chunk& chunk);
    void  removeChunkFromCache(std::string id);

    bool isValid = true;
};


} // namespace bedrock_world_map::storage

#define RETURN_IF_INVALID                                                                                              \
    if (!this->isValid) return

#define THROW_IF_INVALID                                                                                               \
    if (!this->isValid) throw std::exception("无效操作")