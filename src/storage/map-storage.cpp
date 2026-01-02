#include "map-storage.h"
#include "logger.h"
#include "nlohmann/json_fwd.hpp"

#include <exception>
#include <filesystem>
#include <fstream>
#include <ios>
#include <nlohmann/json.hpp>
#include <utility>


using namespace bedrock_world_map::storage;

Storage::Storage(std::string levelName, std::filesystem::path path) {
    this->levelName              = std::move(levelName);
    this->path                   = std::move(path);
    ldbOptions.create_if_missing = true;
    leveldb::Status status       = leveldb::DB::Open(ldbOptions, (path / "db").string(), &db);
    if (status.ok()) {
        DEBUG("成功打开{}的数据库(位于{})", levelName, path.string());
        this->readBiomes();
        this->readChunks();
    } else {
        throw std::exception("打开数据库失败");
    }
}

void Storage::writeBiomes() {
    std::ofstream ofs(path / "biomes.json");
    if (ofs.is_open()) {
        nlohmann::json j = this->biomes;
        ofs << j.dump();
        ofs.close();
    }
}
void Storage::readBiomes() {
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
}