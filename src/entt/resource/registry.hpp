#ifndef ENTT_RESOURCE_REGISTRY_HPP
#define ENTT_RESOURCE_REGISTRY_HPP

#include <entt/resource/cache.hpp>
#include <entt/core/family.hpp>

#include <string>
#include <vector>

namespace entt
{

    /*! @brief TODO */
    template<template<typename> class LoaderImplementation, typename LoaderInterface>
    class ResourceRegistry
    {
        static_assert(!std::is_same<LoaderInterface, void>::value, "!");

        using type_family = Family<ResourceRegistry<LoaderImplementation, LoaderInterface>, struct InternalResourceType>;
    public:
        /*! @brief TODO */
        using id_t = HashedString;

        /*! @brief TODO */
        using type_id_t = typename type_family::family_type;

        /*! @brief TODO */
        template<typename Resource>
        using loader_type = LoaderImplementation<Resource>;

        /*! @brief TODO */
        template<typename Resource>
        using handle_type = ResourceHandle<Resource>;

        /*! @brief TODO */
        template<typename Resource>
        using cache_type = ResourceCache<Resource>;


        /*! @brief TODO */
        template<typename Resource, typename Loader = loader_type<Resource>>
        class LoaderBasis: 
            public LoaderInterface,
            public ResourceLoader<Loader, Resource> {

            friend class ResourceCache<Resource>;
            friend class ResourceRegistry<LoaderImplementation, LoaderInterface>;

            /*! @brief TODO */
            std::shared_ptr<Resource> get(const id_t& id) {
                return static_cast<Loader*>(this)->load(id);
            }
            /*! @brief TODO */
            bool reset(Resource& resource) {
                return static_cast<Loader*>(this)->unload(resource);
            }
            /*! @brief TODO */
            id_t id(Resource& resource) {
                return static_cast<Loader*>(this)->identify(resource);
            }
        protected:
            cache_type<Resource>* cache;
        };
    private:
        using cache_container_type = std::vector<std::shared_ptr<void>>;
        using loader_container_type = std::vector<std::unique_ptr<LoaderInterface>>;
    public:
        ResourceRegistry() = default;
        ResourceRegistry(const ResourceRegistry&) = delete;
        ResourceRegistry(ResourceRegistry&&) = default;


        /*! @brief TODO */
        template<typename Resource, typename... Args>
        auto deliver(Args... args) ENTT_NOEXCEPT {
            static_assert(std::is_base_of<LoaderBasis<Resource>, loader_type<Resource>>::value, "!");

            const auto index = type<Resource>();

            if (cache_container.size() <= index) {
                cache_container.resize(index + 1);
            }
            if (loader_container.size() <= index) {
                loader_container.resize(index + 1);
            }

            cache_container[index] = std::make_shared<cache_type<Resource>>();
            loader_container[index] = std::make_unique<loader_type<Resource>>(std::forward<Args>(args)...);

            loader<Resource>()->cache = cache<Resource>();
            return index;
        }

        /*! @brief TODO */
        template<typename Resource>
        handle_type<Resource> obtain(const id_t& id) ENTT_NOEXCEPT {
            auto* cache_ptr = cache<Resource>();
            auto* loader_ptr = loader<Resource>();

            cache_ptr->load(loader_ptr, id, id);

            return cache_ptr->handle(id);
        }

        /*! @brief TODO */
        template<typename Resource>
        bool release(handle_type<Resource>& handle) ENTT_NOEXCEPT {
            bool released = false;
            if (handle) {
                auto* loader_ptr = loader<Resource>();
                auto* cache_ptr = cache<Resource>();
                auto& resource_ref = const_cast<Resource&>(handle.get());

                const id_t id{ loader_ptr->id(resource_ref) };

                released = cache_ptr->count(id) <= 2 && loader_ptr->reset(resource_ref);

                if (released) {
                    cache_ptr->discard(id);
                }

                handle.reset();
            }
            return released;
        }

        /*! @brief TODO */
        template<typename Resource, typename Cache = cache_type<Resource>>
        Cache* cache() ENTT_NOEXCEPT {
            return (Cache*)cache_container[type<Resource>()].get();
        }

        /*! @brief TODO */
        template<typename Resource, typename Loader = loader_type<Resource> >
        Loader* loader() ENTT_NOEXCEPT {
            return (Loader*)loader_container[type<Resource>()].get();
        }

        /*! @brief TODO */
        const loader_container_type& loaders() const ENTT_NOEXCEPT {
            return loader_container;
        }

        /*! @brief TODO */
        template<typename Resource>
        constexpr type_id_t type() {
            return type_family::type<Resource>();
        }
    private:
        cache_container_type    cache_container;
        loader_container_type   loader_container;
    };
}
#endif