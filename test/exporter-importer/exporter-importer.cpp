#include <gtest/gtest.h>
#include <sstream>
#include <vector>
#include <cereal/archives/json.hpp>
#include <entt/entity/registry.hpp>
#include <entt/entity/prototype.hpp>
#include <entt/core/hashed_string.hpp>

struct Position {
    float x;
    float y;
};

struct Timer {
    int duration;
    int elapsed{ 0 };
};


struct Relationship {
    entt::DefaultRegistry::entity_type parent;
};

template<typename Archive>
void serialize(Archive &archive, Position &position) {
    archive(cereal::make_nvp("x", position.x));
    archive(cereal::make_nvp("y", position.y));
}

template<typename Archive>
void serialize(Archive &archive, Timer &timer) {
    archive(cereal::make_nvp("duration", timer.duration));
}

template<typename Archive>
void serialize(Archive &archive, Relationship &relationship) {
    archive(cereal::make_nvp("parent", relationship.parent));
}

template<typename Archive>
bool has_component_key(Archive& archive, const char* key)
{
    const char* current_key = archive.getNodeName();

    return current_key != nullptr && !strcmp(current_key, key);
}

const std::array<const char*, 3> component_keys = { "position", "timer", "relationship" };

TEST(ExporterImporter, Basic) {
    std::stringstream storage;


    entt::DefaultRegistry src;
    entt::DefaultRegistry dst;

    auto e0 = src.create();
    src.assign<Position>(e0, 16.f, 16.f);

    auto es = src.create();
    src.assign<Position>(es, .8f, .0f);
    src.assign<Relationship>(es, e0);


    auto ed = dst.create();
    {
        cereal::JSONOutputArchive output{ storage };

        src
            .exporter<const char*, cereal::NameValuePair>()
            .component<Position, Timer, Relationship>(output, es, component_keys);
    }
    {
        cereal::JSONInputArchive input{ storage };

        dst
            .importer<const char*, cereal::NameValuePair>()
            .component<Position, Timer, Relationship>(input, has_component_key<cereal::JSONInputArchive>, ed, component_keys);

        ASSERT_EQ(dst.has<Position>(ed), src.has<Position>(es));
        ASSERT_EQ(dst.has<Timer>(ed), src.has<Timer>(es));
        ASSERT_EQ(dst.has<Relationship>(ed), src.has<Relationship>(es));
    }
}


TEST(ExporterImporter, Prototype) {
    std::stringstream storage;

    entt::DefaultRegistry registry;

    entt::DefaultPrototype src{registry};
    entt::DefaultPrototype dst{registry};

    src.set<Position>(.8f, .0f);
    src.set<Timer>(1000, 100);

    {
        cereal::JSONOutputArchive output{ storage };

        registry
            .exporter<const char*, cereal::NameValuePair>()
            .component<Position, Timer, Relationship>(output, src, component_keys);
    }
    {

        cereal::JSONInputArchive input{ storage };

        registry
            .importer<const char*, cereal::NameValuePair>()
            .component<Position, Timer, Relationship>(input, has_component_key<cereal::JSONInputArchive>, dst, component_keys);

        ASSERT_EQ(dst.has<Position>(), src.has<Position>());
        ASSERT_EQ(dst.has<Timer>(), src.has<Timer>());
        ASSERT_EQ(dst.has<Relationship>(), src.has<Relationship>());
    }
    storage.clear();

    auto e1 = src();
    auto e2 = dst();

    ASSERT_EQ(registry.has<Position>(e1), registry.has<Position>(e2));
    ASSERT_EQ(registry.has<Timer>(e1), registry.has<Timer>(e2));
    ASSERT_EQ(registry.has<Relationship>(e1), registry.has<Relationship>(e2));
}
