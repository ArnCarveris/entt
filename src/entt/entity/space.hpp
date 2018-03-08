#ifndef ENTT_ENTITY_SPACE_HPP
#define ENTT_ENTITY_SPACE_HPP

#include "registry.hpp"
#include "../core/hashed_string.hpp"


namespace entt {


	template<typename Entity, HashedString::hash_type Hash> class Space;
	template<typename Entity, HashedString::hash_type Hash> class SpaceAccessor
	{
		static_assert(Hash > 1, "!");
	private:
		friend class Space<Entity, Hash>;
	public:
		static constexpr HashedString::hash_type hash = Hash;
	public:
		SpaceAccessor() = default;
		SpaceAccessor(const char* string):
			key(HashedString{ string })
		{ }

		SpaceAccessor(const SpaceAccessor&) = default;
		SpaceAccessor(SpaceAccessor&&) = default;

		SpaceAccessor& operator = (const SpaceAccessor&) = default;
		SpaceAccessor& operator = (SpaceAccessor&&) = default;

		bool operator == (const SpaceAccessor& other) const noexcept
		{
			return key == other.key;
		}
		bool operator != (const SpaceAccessor& other) const noexcept
		{
			return key != other.key;
		}
	public:
		HashedString::hash_type key = 0;
	};

	template<typename Entity, HashedString::hash_type Hash>
	class Space : public Registry<Entity, Hash>
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
