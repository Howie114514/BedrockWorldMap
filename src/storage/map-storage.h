#pragma once

#include <filesystem>
#include <leveldb/db.h>
#include <mutex>
#include <nlohmann/json.hpp>
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
};
} // namespace bedrock_world_map

namespace bedrock_world_map::storage {
class Storage {
public:
    Storage(std::string levelName, std::filesystem::path path);

private:
    std::vector<Chunk>             chunks;
    std::unordered_map<int, Biome> biomes;
    std::mutex                     biomesMtx;
    std::string                    levelName;
    std::filesystem::path          path;
    leveldb::DB*                   db;
    leveldb::Options               ldbOptions;

public:
    void   writeBiomes();
    void   readBiomes();
    void   putBiome(Biome b);
    void   saveDB();
    void   closeDB();
    void   readChunks();
    void   putChunk(Chunk chunk);
    void   putBlock(Point point);
    Chunk& getChunk();
    Point& getBlock();
};
} // namespace bedrock_world_map::storage