#ifndef ENTT_ENTITY_IMPORTER_HPP
#define ENTT_ENTITY_IMPORTER_HPP


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
    class Importer final {

        friend class Registry<Entity>;

        Importer(Registry<Entity> &registry) ENTT_NOEXCEPT
            : registry{ registry }
        {}

        template<typename Type, typename... Args>
        void assign(Args... args, Entity& entity, const Type& value) const {
            registry.template assign<Type>(args..., entity, value);
        }

        template<typename Type, typename... Args>
        void assign(Args... args, Prototype<Entity>& prototype, const Type& value) const {
            prototype.set<Type>(args..., value);
        }

        template<typename Type, typename Archive, typename Destination, typename... Args>
        bool try_load(Archive &archive, Destination& destination, KeyType key, Args... args) const {
            try
            {
                Type tmp{};
                
                KeyValuePair<Type> instance{key,  std::move(tmp) };

                archive(instance);

                assign<Type>(args..., destination, static_cast<const Type &>(instance.value));

                return true;
            }
            catch (const std::exception& e)
            {
                (void)e;

                return false;
            }
        }

        template<typename... Component, typename Archive, typename Destination, std::size_t... Indexes>
        void components(Archive &archive, Destination& destination, const std::array<KeyType, sizeof...(Component)>& keys, std::index_sequence<Indexes...>) const {
            using accumulator_type = int[];
            accumulator_type accumulator = { (try_load<Component>(archive, destination, keys[Indexes]), 0)... };
            (void)accumulator;
        }

        template<typename... Tag, typename Archive, std::size_t... Indexes>
        void tags(Archive &archive, Entity& entity, const std::array<KeyType, sizeof...(Tag)>& keys, std::index_sequence<Indexes...>) const {
            using accumulator_type = int[];
            accumulator_type accumulator = { (try_load<Tag>(archive, entity, keys[Indexes], tag_t{}), 0)... };
            (void)accumulator;
        }
    public:

        /*! @brief Copying a importer isn't allowed. */
        Importer(const Importer &) = delete;
        /*! @brief Default move constructor. */
        Importer(Importer &&) = default;

        /*! @brief Copying a importer isn't allowed. @return This importer. */
        Importer & operator=(const Importer &) = delete;
        /*! @brief Default move assignment operator. @return This importer. */
        Importer & operator=(Importer &&) = default;

        template<typename... Component, typename Archive, typename Destination>
        const Importer & component(Archive &archive, Destination& destination, const std::array<KeyType, sizeof...(Component)>& keys) const {
            components<Component...>(archive, destination, keys, std::make_index_sequence<sizeof...(Component)>{});
            return *this;
        }

        template<typename... Tag, typename Archive>
        const Importer & tag(Archive &archive, Entity& entity, const std::array<KeyType, sizeof...(Tag)>& keys) const {
            tags<Tag...>(archive, entity, keys, std::make_index_sequence<sizeof...(Tag)>{});
            return *this;
        }
    private:
        Registry<Entity> &registry;
    };
}


#endif // ENTT_ENTITY_IMPORTER_HPP