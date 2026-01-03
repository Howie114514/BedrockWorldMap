#include "nlohmann/detail/macro_scope.hpp"
#include <filesystem>
#include <nlohmann/json.hpp>
#include <string>

namespace bedrock_world_map {
enum class MapType { DEFAULT, TERRAIN, BIOME };

struct Configure {
    // 基础信息
    int         format_version;
    std::string language;
    std::string storagePath;

    // 地图设置
    MapType m_mapType;
    bool    m_showPlayers;

    // 路标点
    bool wp_showWaypointsInMap;
    bool wp_showWaypointsInGame;
    bool wp_saveDeadPosition;


    NLOHMANN_DEFINE_TYPE_INTRUSIVE(
        Configure,
        format_version,
        language,
        m_mapType,
        m_showPlayers,
        wp_showWaypointsInGame,
        wp_showWaypointsInMap,
        wp_saveDeadPosition
    );
};

namespace configure {
Configure             configure;
std::filesystem::path path;
void                  read();
} // namespace configure

} // namespace bedrock_world_map