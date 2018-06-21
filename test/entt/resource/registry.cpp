#include <gtest/gtest.h>
#include <entt/resource/registry.hpp>
#include <string>

template<typename Resource>
class Loader;
struct ILoader
{
    virtual const char* name() = 0;
};

using ResourceRegistry = entt::ResourceRegistry<Loader, ILoader>;


struct Resource
{
    std::string id;

    Resource(const ResourceRegistry::id_type& id) :
        id((const char*)id)
    { }
};
struct ResourceA : Resource
{
    int value;

    ResourceA(const ResourceRegistry::id_type& id, int value) :
        Resource(id),
        value(value)
    { }
};

struct ResourceB : Resource
{
    float value;

    ResourceB(const ResourceRegistry::id_type& id, float value) :
        Resource(id),
        value(value)
    { }
};


template<> class Loader<ResourceA> : public ResourceRegistry::LoaderBasis<ResourceA>{
public:
    std::shared_ptr<ResourceA> load(const ResourceRegistry::id_type& id)
    {
        return std::make_shared<ResourceA>(id, 123);
    };
    bool unload(const ResourceA& entry)
    {
        return true;
    }
    ResourceRegistry::id_type identify(const ResourceA& entry)
    {
        return ResourceRegistry::id_type{ entry.id.c_str() };
    }
   
    const char* name() final
    {
        return "ResourceA";
    }
};

template<> class Loader<ResourceB> : public ResourceRegistry::LoaderBasis<ResourceB> {
public:
    std::shared_ptr<ResourceB> load(const ResourceRegistry::id_type& id)
    {
        return std::make_shared<ResourceB>(id, 3.14f);
    };
    bool unload(const ResourceB& entry)
    {
        return true;
    }
    ResourceRegistry::id_type identify(const ResourceB& entry)
    {
        return ResourceRegistry::id_type{ entry.id.c_str()};
    }

    const char* name() final
    {
        return "ResourceB";
    }
};


TEST(ResourceRegistry, Functionalities) {
    ResourceRegistry registry;

    const ResourceRegistry::id_type id_a{ "a" };
    const ResourceRegistry::id_type id_b{ "b" };


    ASSERT_EQ(registry.deliver<ResourceA>(), registry.type<ResourceA>());
    ASSERT_EQ(registry.deliver<ResourceB>(), registry.type<ResourceB>());

    ASSERT_TRUE(registry.cache<ResourceA>()->empty());
    ASSERT_TRUE(registry.cache<ResourceB>()->empty());

    auto handle_a = registry.obtain<ResourceA>(id_a);
    auto handle_b = registry.obtain<ResourceB>(id_b);

    ASSERT_TRUE(handle_a);
    ASSERT_TRUE(handle_b);

    ASSERT_FALSE(registry.cache<ResourceA>()->empty());
    ASSERT_FALSE(registry.cache<ResourceB>()->empty());

    ASSERT_EQ(registry.cache<ResourceA>()->count(id_a), 2);
    ASSERT_EQ(registry.cache<ResourceB>()->count(id_b), 2);

    ASSERT_EQ(registry.loader<ResourceA>()->identify(handle_a.get()), id_a);
    ASSERT_EQ(registry.loader<ResourceB>()->identify(handle_b.get()), id_b);

    auto handle_a1 = registry.obtain<ResourceA>(id_a);
    
    ASSERT_EQ(registry.cache<ResourceA>()->count(id_a), 3);
    
    ASSERT_FALSE(registry.release(handle_a1));
    ASSERT_FALSE(handle_a1);

    ASSERT_EQ(registry.cache<ResourceA>()->count(id_a), 2);

    ASSERT_TRUE(registry.release(handle_a));
    ASSERT_TRUE(registry.release(handle_b));
    
    ASSERT_FALSE(handle_a);
    ASSERT_FALSE(handle_b);

    ASSERT_FALSE(registry.release(handle_a));
    ASSERT_FALSE(registry.release(handle_b));

    ASSERT_EQ(registry.cache<ResourceA>()->count(id_a), 0);
    ASSERT_EQ(registry.cache<ResourceB>()->count(id_b), 0);

    ASSERT_EQ(strcmp(registry.loader<ResourceA>()->name(), "ResourceA"), 0);
    ASSERT_EQ(strcmp(registry.loader<ResourceB>()->name(), "ResourceB"), 0);
    
    for (auto& loader : registry.loaders())
    {
        auto* name = loader->name();

        ASSERT_TRUE(name != nullptr);
    }
}
