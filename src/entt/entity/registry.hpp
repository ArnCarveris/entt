#ifndef ENTT_ENTITY_REGISTRY_HPP
#define ENTT_ENTITY_REGISTRY_HPP


#include "container.hpp"


namespace entt {

template<typename Entity>
class Registry : public Container
	<
		Entity,
		struct InternalRegistryTagFamily,
		struct InternalRegistryComponentFamily,
		struct InternalRegistryViewFamily
	>
{
};


/**
 * @brief Default registry class.
 *
 * The default registry is the best choice for almost all the applications.<br/>
 * Users should have a really good reason to choose something different.
 */
using DefaultRegistry = Registry<std::uint32_t>;


}


#endif // ENTT_ENTITY_REGISTRY_HPP
