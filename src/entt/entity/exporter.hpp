#ifndef ENTT_ENTITY_EXPORTER_HPP
#define ENTT_ENTITY_EXPORTER_HPP


#include <array>
#include <cstddef>
#include <utility>
#include <cassert>
#include <iterator>
#include <type_traits>
#include <unordered_map>
#include "../config/config.h"
#include "entt_traits.hpp"
#include "utility.hpp"


namespace entt {

    /**
    * @brief Forward declaration of the registry class.
    */
    template<typename>
    class Registry;

    template<typename Entity>
    class Prototype;

    template<typename Entity, typename KeyType, template<class> class KeyValuePair>
    class Exporter final {

        friend class Registry<Entity>;
        Exporter(const Registry<Entity> &registry) ENTT_NOEXCEPT
            : registry{ registry }
        {}

        template<typename Component>
        inline KeyValuePair<Component> make_kvp(KeyType key, Component&& component) const {
            return KeyValuePair<Component>{key, component};
        }

        template<typename Component, typename Archive>
        void component(Archive &archive, const Entity& entity, KeyType key) const {
            if (registry.template has<Component>(entity)) {
                archive(make_kvp(key, registry.template get<Component>(entity)));
            }
        }

        template<typename Component, typename Archive>
        void component(Archive &archive, const Prototype<Entity>& prototype, KeyType key) const {
            if (prototype.has<Component>()) {
                archive(make_kvp(key, prototype.get<Component>()));
            }
        }


        template<typename... Component, typename Archive, typename Source, std::size_t... Indexes>
        void components(Archive &archive, const Source& source, const std::array<KeyType, sizeof...(Component)>& keys, std::index_sequence<Indexes...>) const {
            using accumulator_type = int[];
            accumulator_type accumulator = { (component<Component>(archive, source, keys[Indexes]), 0)... };
            (void)accumulator;
        }
        
        template<typename Tag, typename Archive>
        void tag(Archive &archive, const Entity& entity, KeyType key) const {

            if (registry.template has<Tag>() && registry.template attachee<Tag>() == entity) {
                archive(make_kvp(key, registry.template get<Tag>()));
            }
        }
        template<typename... Tag, typename Archive, std::size_t... Indexes>
        void tags(Archive &archive, const Entity& entity, const std::array<KeyType, sizeof...(Tag)>& keys, std::index_sequence<Indexes...>) const {
            using accumulator_type = int[];
            accumulator_type accumulator = { (tag<Tag>(archive, entity, keys[Indexes]), 0)... };
            (void)accumulator;
        }
    public:


        /*! @brief Copying a exporter isn't allowed. */
        Exporter(const Exporter &) = delete;
        /*! @brief Default move constructor. */
        Exporter(Exporter &&) = default;

        /*! @brief Copying a exporter isn't allowed. @return This exporter. */
        Exporter & operator=(const Exporter &) = delete;
        /*! @brief Default move assignment operator. @return This exporter. */
        Exporter & operator=(Exporter &&) = default;

        template<typename... Component, typename Archive, typename Source>
        const Exporter & component(Archive &archive, const Source& source, const std::array<KeyType, sizeof...(Component)>& keys) const {
            components<Component...>(archive, source, keys, std::make_index_sequence<sizeof...(Component)>{});
            return *this;
        }

        template<typename... Tag, typename Archive>
        const Exporter & tag(Archive &archive, const Entity& entity, const std::array<KeyType, sizeof...(Tag)>& keys) const {
            tags<Tag...>(archive, entity, keys, std::make_index_sequence<sizeof...(Tag)>{});
            return *this;
        }
    private:
        const Registry<Entity> &registry;
    };
}


#endif // ENTT_ENTITY_EXPORTER_HPP