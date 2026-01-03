#include "map-storage.h"
#include "leveldb/options.h"
#include "logger.h"
#include "nlohmann/json_fwd.hpp"

#include <exception>
#include <filesystem>
#include <fstream>
#include <ios>
#include <nlohmann/json.hpp>
#include <optional>
#include <sstream>
#include <string>
#include <utility>

using namespace bedrock_world_map;
using namespace bedrock_world_map::storage;

std::string bedrock_world_map::getChunkId(Chunk chunk) {
    return std::to_string(chunk.x) + "_" + std::to_string(chunk.z);
}


#pragma region Storage
Storage::Storage(std::string levelName, std::filesystem::path path) {
    this->levelName              = std::move(levelName);
    this->path                   = std::move(path);
    ldbOptions.create_if_missing = true;
    leveldb::Status status       = leveldb::DB::Open(ldbOptions, (path / "db").string(), &db);
    if (status.ok()) {
        DEBUG("成功打开{}的数据库(位于{})", levelName, path.string());
        this->readBiomes();
    } else {
        throw std::exception("打开数据库失败");
    }
}

void Storage::writeBiomes() {
    RETURN_IF_INVALID;
    biomesMtx.lock();
    std::ofstream ofs(path / "biomes.json");
    if (ofs.is_open()) {
        nlohmann::json j = this->biomes;
        ofs << j.dump();
        ofs.close();
    }
    biomesMtx.unlock();
}

void Storage::readBiomes() {
    RETURN_IF_INVALID;
    biomesMtx.lock();
    if (std::filesystem::exists(path / "biomes.json")) {
        std::ifstream ifs(path / "biomes.json");
        if (ifs.is_open()) {
            try {
                auto json = nlohmann::json::parse(ifs);
                json.get_to(this->biomes);
            } catch (nlohmann::json::exception e) {
                DEBUG("群系信息JSON解析失败:{}", e.what());
                ifs.close();
                writeBiomes();
            }
        }
    }
    biomesMtx.unlock();
}

void Storage::putBiome(std::string biomeName) {
    RETURN_IF_INVALID;
    biomesMtx.lock();
    Biome biome;
    biome.id         = (int)biomes.size();
    biome.type       = std::move(biomeName);
    biomes[biome.id] = biome;
    biomesMtx.unlock();
}

void Storage::close() {
    RETURN_IF_INVALID;
    isValid = false;
    delete db;
}

void Storage::saveChunk(Chunk chunk) {
    RETURN_IF_INVALID;
    if (!chunk.isValid) return;
    auto               id = getChunkId(chunk);
    std::ostringstream ss;
    ss.write(reinterpret_cast<char*>(&chunk), sizeof(chunk));
    db->Put(leveldb::WriteOptions(), id, ss.str());
}

bedrock_world_map::Chunk Storage::readChunk(std::string id) {
    THROW_IF_INVALID;
    if (cachedChunks.find(id) != cachedChunks.end()) {
        return cachedChunks[id];
    } else {
        std::string val;
        if (!db->Get(leveldb::ReadOptions(), id, &val).IsNotFound()) {
            std::istringstream ss(val);
            Chunk              c{};
            ss.read(reinterpret_cast<char*>(&c), sizeof(Chunk));
            return c;
        } else {
            Chunk c{};
            c.isValid = false;
            return c;
        }
    }
}

void Storage::updateChunk(Chunk chunk) {
    RETURN_IF_INVALID;
    if (chunk.isValid) {
        auto id = getChunkId(chunk);
        saveChunk(chunk);
        if (cachedChunks.find(id) != cachedChunks.end()) cachedChunks[id] = chunk;
    }
}
void Storage::putChunkToCache(Chunk& chunk) {
    RETURN_IF_INVALID;
    cachedChunks[getChunkId(chunk)] = chunk;
}

void Storage::removeChunkFromCache(std::string id) {
    RETURN_IF_INVALID;
    if (cachedChunks.find(id) != cachedChunks.end()) {
        cachedChunks.erase(id);
    }
}

Storage::~Storage() { close(); }

#pragma endregion Storage