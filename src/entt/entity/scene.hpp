#ifndef ENTT_ENTITY_SPACE_HPP
#define ENTT_ENTITY_SPACE_HPP

#include "registry.hpp"
#include "../core/hashed_string.hpp"


namespace entt {


	template<typename Entity, HashedString::hash_type Hash> class Scene;
	template<typename Entity, HashedString::hash_type Hash> class SceneAccessor
	{
		static_assert(Hash > 1, "!");
	private:
		friend class Scene<Entity, Hash>;
	public:
		static constexpr HashedString::hash_type hash = Hash;
	public:
		SceneAccessor() = default;
		SceneAccessor(const char* string):
			key(HashedString{ string })
		{ }

		SceneAccessor(const SceneAccessor&) = default;
		SceneAccessor(SceneAccessor&&) = default;

		SceneAccessor& operator = (const SceneAccessor&) = default;
		SceneAccessor& operator = (SceneAccessor&&) = default;

		bool operator == (const SceneAccessor& other) const noexcept
		{
			return key == other.key;
		}
		bool operator != (const SceneAccessor& other) const noexcept
		{
			return key != other.key;
		}
	public:
		HashedString::hash_type key = 0;
	};

	template<typename Entity, HashedString::hash_type Hash>
	class Scene : public Registry<Entity, Hash>
	{
	public:
		using accessor_type = SceneAccessor<Entity, Hash>;
	public:
		Scene() = delete;
		Scene(const accessor_type& accessor) :
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
