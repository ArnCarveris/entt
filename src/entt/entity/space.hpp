#ifndef ENTT_ENTITY_SPACE_HPP
#define ENTT_ENTITY_SPACE_HPP

#include "container.hpp"
#include "../core/hashed_string.hpp"


namespace entt {

	template<typename Entity, HashedString::hash_type Hash> class Space;
	template<typename Entity, HashedString::hash_type Hash> class SpaceAccessor
	{
	private:
		friend class Space<Entity, Hash>;
	private:
		struct InternalTagFamily;
		struct InternalComponentFamily;
		struct InternalViewFamily;

		using container_type = Container
		<
			Entity,
			InternalTagFamily,
			InternalComponentFamily,
			InternalViewFamily
		>;
	public:
		static constexpr HashedString::hash_type hash = Hash;
	public:
		SpaceAccessor() = default;
		SpaceAccessor(const char* string):
			key(string)
		{ }

		SpaceAccessor(const SpaceAccessor&) = default;
		SpaceAccessor(SpaceAccessor&&) = default;

		SpaceAccessor& operator = (const SpaceAccessor&) = default;
		SpaceAccessor& operator = (SpaceAccessor&&) = default;
	public:
		HashedString key{""};
	};

	template<typename Entity, HashedString::hash_type Hash>
	class Space : public SpaceAccessor<Entity, Hash>::container_type
	{
	public:
		using accessor_type = SpaceAccessor<Entity, Hash>;
	public:
		Space() = delete;
		Space(const accessor_type& accessor) :
			m_accessor(accessor)
		{ }

		const accessor_type& accessor() const noexcept
		{
			return m_accessor;
		}
	private:
		const accessor_type m_accessor;
	};
}


#endif // ENTT_ENTITY_SPACE_HPP
