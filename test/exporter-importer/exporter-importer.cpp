#include <gtest/gtest.h>
#include <sstream>
#include <vector>
#include <cereal/archives/json.hpp>
#include <entt/entity/registry.hpp>
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



TEST(ExporterImporter, Full) {
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
         .component<Position, Timer, Relationship>(output, es, {"position", "timer", "relationship"});
    }
    {
        cereal::JSONInputArchive input{ storage };

        dst
         .importer<const char*, cereal::NameValuePair>()
         .component<Position, Timer, Relationship>(input, ed, {"position", "timer", "relationship");

        ASSERT_EQ(dst.has<Position>(ed), src.has<Position>(es));
        ASSERT_EQ(dst.has<Timer>(ed), src.has<Timer>(es));
        ASSERT_EQ(dst.has<Relationship>(ed), src.has<Relationship>(es));
    }
}
