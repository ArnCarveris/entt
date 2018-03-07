#include <unordered_map>
#include <unordered_set>
#include <functional>
#include <type_traits>
#include <gtest/gtest.h>
#include <entt/entity/entt_traits.hpp>
#include <entt/entity/space.hpp>

using SpaceAccessor = entt::SpaceAccessor<uint32_t, (std::uint64_t)entt::HashedString{ "foo" }>;
using Space = entt::Space<uint32_t, SpaceAccessor::hash>;


TEST(Space, Accessors) {
	using accessor_a_t = entt::SpaceAccessor < uint32_t, (std::uint64_t)entt::HashedString{ "a" } >;
	using accessor_b_t = entt::SpaceAccessor < uint32_t, (std::uint64_t)entt::HashedString{ "b" } >;

	using space_a_t = entt::Space<uint32_t, accessor_a_t::hash>;
	using space_b_t = entt::Space<uint32_t, accessor_b_t::hash>;

	space_a_t space_a(accessor_a_t("bar"));
	space_b_t space_b(accessor_b_t("bar"));

	auto e1 = space_a.create<int, char, float>();
	auto e2 = space_b.create<float>();

	space_a.assign<accessor_b_t>(e1, space_b.accessor());
	space_b.assign<accessor_a_t>(e2, space_a.accessor());

	ASSERT_EQ(space_a.get<accessor_b_t>(e1), space_b.accessor());
	ASSERT_EQ(space_b.get<accessor_a_t>(e2), space_a.accessor());

	ASSERT_EQ(space_a.size(), space_a_t::size_type{ 1 });
	ASSERT_EQ(space_b.size(), space_b_t::size_type{ 1 });
}

TEST(Space, Functionalities) {
	Space space(SpaceAccessor("bar"));

	ASSERT_EQ(space.size(), Space::size_type{ 0 });
	ASSERT_NO_THROW(space.reserve(42));
	ASSERT_NO_THROW(space.reserve<int>(8));
	ASSERT_NO_THROW(space.reserve<char>(8));
	ASSERT_TRUE(space.empty());

	ASSERT_EQ(space.capacity(), Space::size_type{ 0 });
	ASSERT_EQ(space.size<int>(), Space::size_type{ 0 });
	ASSERT_EQ(space.size<char>(), Space::size_type{ 0 });
	ASSERT_TRUE(space.empty<int>());
	ASSERT_TRUE(space.empty<char>());

	auto e0 = space.create();
	auto e1 = space.create<int, char>();

	ASSERT_TRUE(space.has<>(e0));
	ASSERT_TRUE(space.has<>(e1));

	ASSERT_EQ(space.capacity(), Space::size_type{ 2 });
	ASSERT_EQ(space.size<int>(), Space::size_type{ 1 });
	ASSERT_EQ(space.size<char>(), Space::size_type{ 1 });
	ASSERT_FALSE(space.empty<int>());
	ASSERT_FALSE(space.empty<char>());

	ASSERT_NE(e0, e1);

	ASSERT_FALSE(space.has<int>(e0));
	ASSERT_TRUE(space.has<int>(e1));
	ASSERT_FALSE(space.has<char>(e0));
	ASSERT_TRUE(space.has<char>(e1));
	ASSERT_FALSE((space.has<int, char>(e0)));
	ASSERT_TRUE((space.has<int, char>(e1)));

	ASSERT_EQ(space.assign<int>(e0, 42), 42);
	ASSERT_EQ(space.assign<char>(e0, 'c'), 'c');
	ASSERT_NO_THROW(space.remove<int>(e1));
	ASSERT_NO_THROW(space.remove<char>(e1));

	ASSERT_TRUE(space.has<int>(e0));
	ASSERT_FALSE(space.has<int>(e1));
	ASSERT_TRUE(space.has<char>(e0));
	ASSERT_FALSE(space.has<char>(e1));
	ASSERT_TRUE((space.has<int, char>(e0)));
	ASSERT_FALSE((space.has<int, char>(e1)));

	auto e2 = space.create();

	space.accomodate<int>(e2, space.get<int>(e0));
	space.accomodate<char>(e2, space.get<char>(e0));

	ASSERT_TRUE(space.has<int>(e2));
	ASSERT_TRUE(space.has<char>(e2));
	ASSERT_EQ(space.get<int>(e0), 42);
	ASSERT_EQ(space.get<char>(e0), 'c');

	ASSERT_EQ(std::get<0>(space.get<int, char>(e0)), 42);
	ASSERT_EQ(std::get<1>(static_cast<const Space &>(space).get<int, char>(e0)), 'c');

	ASSERT_EQ(space.get<int>(e0), space.get<int>(e2));
	ASSERT_EQ(space.get<char>(e0), space.get<char>(e2));
	ASSERT_NE(&space.get<int>(e0), &space.get<int>(e2));
	ASSERT_NE(&space.get<char>(e0), &space.get<char>(e2));

	ASSERT_NO_THROW(space.replace<int>(e0, 0));
	ASSERT_EQ(space.get<int>(e0), 0);

	ASSERT_NO_THROW(space.accomodate<int>(e0, 1));
	ASSERT_NO_THROW(space.accomodate<int>(e1, 1));
	ASSERT_EQ(static_cast<const Space &>(space).get<int>(e0), 1);
	ASSERT_EQ(static_cast<const Space &>(space).get<int>(e1), 1);

	ASSERT_EQ(space.size(), Space::size_type{ 3 });
	ASSERT_FALSE(space.empty());

	ASSERT_EQ(space.version(e2), Space::version_type{ 0 });
	ASSERT_EQ(space.current(e2), Space::version_type{ 0 });
	ASSERT_EQ(space.capacity(), Space::size_type{ 3 });
	ASSERT_NO_THROW(space.destroy(e2));
	ASSERT_EQ(space.capacity(), Space::size_type{ 3 });
	ASSERT_EQ(space.version(e2), Space::version_type{ 0 });
	ASSERT_EQ(space.current(e2), Space::version_type{ 1 });

	ASSERT_TRUE(space.valid(e0));
	ASSERT_TRUE(space.valid(e1));
	ASSERT_FALSE(space.valid(e2));

	ASSERT_EQ(space.size(), Space::size_type{ 2 });
	ASSERT_FALSE(space.empty());

	ASSERT_NO_THROW(space.reset());

	ASSERT_EQ(space.size(), Space::size_type{ 0 });
	ASSERT_TRUE(space.empty());

	space.create<int, char>();

	ASSERT_EQ(space.size<int>(), Space::size_type{ 1 });
	ASSERT_EQ(space.size<char>(), Space::size_type{ 1 });
	ASSERT_FALSE(space.empty<int>());
	ASSERT_FALSE(space.empty<char>());

	ASSERT_NO_THROW(space.reset<int>());

	ASSERT_EQ(space.size<int>(), Space::size_type{ 0 });
	ASSERT_EQ(space.size<char>(), Space::size_type{ 1 });
	ASSERT_TRUE(space.empty<int>());
	ASSERT_FALSE(space.empty<char>());

	ASSERT_NO_THROW(space.reset());

	ASSERT_EQ(space.size<int>(), Space::size_type{ 0 });
	ASSERT_EQ(space.size<char>(), Space::size_type{ 0 });
	ASSERT_TRUE(space.empty<int>());
	ASSERT_TRUE(space.empty<char>());

	e0 = space.create<int>();
	e1 = space.create();

	ASSERT_NO_THROW(space.reset<int>(e0));
	ASSERT_NO_THROW(space.reset<int>(e1));

	ASSERT_EQ(space.size<int>(), Space::size_type{ 0 });
	ASSERT_EQ(space.size<char>(), Space::size_type{ 0 });
	ASSERT_TRUE(space.empty<int>());
}

TEST(Space, CreateDestroyCornerCase) {
	Space space(SpaceAccessor("bar"));

	auto e0 = space.create();
	auto e1 = space.create();

	space.destroy(e0);
	space.destroy(e1);

	space.each([](auto) { FAIL(); });

	ASSERT_EQ(space.current(e0), Space::version_type{ 1 });
	ASSERT_EQ(space.current(e1), Space::version_type{ 1 });
}

TEST(Space, VersionOverflow) {
	Space space(SpaceAccessor("bar"));

	auto entity = space.create();
	space.destroy(entity);

	ASSERT_EQ(space.version(entity), Space::version_type{});

	for (auto i = entt::entt_traits<Space::entity_type>::version_mask; i; --i) {
		ASSERT_NE(space.current(entity), space.version(entity));
		space.destroy(space.create());
	}

	ASSERT_EQ(space.current(entity), space.version(entity));
}

TEST(Space, Each) {
	Space space(SpaceAccessor("bar"));
	Space::size_type tot;
	Space::size_type match;

	space.create();
	space.create<int>();
	space.create();
	space.create<int>();
	space.create();

	tot = 0u;
	match = 0u;

	space.each([&](auto entity) {
		if (space.has<int>(entity)) { ++match; }
		space.create();
		++tot;
	});

	ASSERT_EQ(tot, 5u);
	ASSERT_EQ(match, 2u);

	tot = 0u;
	match = 0u;

	space.each([&](auto entity) {
		if (space.has<int>(entity)) {
			space.destroy(entity);
			++match;
		}

		++tot;
	});

	ASSERT_EQ(tot, 10u);
	ASSERT_EQ(match, 2u);

	tot = 0u;
	match = 0u;

	space.each([&](auto entity) {
		if (space.has<int>(entity)) { ++match; }
		space.destroy(entity);
		++tot;
	});

	ASSERT_EQ(tot, 8u);
	ASSERT_EQ(match, 0u);

	space.each([&](auto) { FAIL(); });
}

TEST(Space, Orphans) {
	Space space(SpaceAccessor("bar"));
	Space::size_type tot{};

	space.create<int>();
	space.create();
	space.create<int>();
	space.create();
	space.attach<double>(space.create());

	space.orphans([&](auto) { ++tot; });
	ASSERT_EQ(tot, 2u);
	tot = 0u;

	space.each([&](auto entity) { space.reset<int>(entity); });
	space.orphans([&](auto) { ++tot; });
	ASSERT_EQ(tot, 4u);
	space.reset();
	tot = 0u;

	space.orphans([&](auto) { ++tot; });
	ASSERT_EQ(tot, 0u);
}

TEST(Space, Types) {
	Space space(SpaceAccessor("bar"));

	ASSERT_EQ(space.tag<int>(), space.tag<int>());
	ASSERT_EQ(space.component<int>(), space.component<int>());

	ASSERT_NE(space.tag<int>(), space.tag<double>());
	ASSERT_NE(space.component<int>(), space.component<double>());
}

TEST(Space, CreateDestroyEntities) {
	Space space(SpaceAccessor("bar"));
	Space::entity_type pre{}, post{};

	for (int i = 0; i < 10; ++i) {
		space.create<double>();
	}

	space.reset();

	for (int i = 0; i < 7; ++i) {
		auto entity = space.create<int>();
		if (i == 3) { pre = entity; }
	}

	space.reset();

	for (int i = 0; i < 5; ++i) {
		auto entity = space.create();
		if (i == 3) { post = entity; }
	}

	ASSERT_FALSE(space.valid(pre));
	ASSERT_TRUE(space.valid(post));
	ASSERT_NE(space.version(pre), space.version(post));
	ASSERT_EQ(space.version(pre) + 1, space.version(post));
	ASSERT_EQ(space.current(pre), space.current(post));
}

TEST(Space, AttachRemoveTags) {
	Space space(SpaceAccessor("bar"));
	const auto& cspace = space;

	ASSERT_FALSE(space.has<int>());

	auto entity = space.create();
	space.attach<int>(entity, 42);

	ASSERT_TRUE(space.has<int>());
	ASSERT_EQ(space.get<int>(), 42);
	ASSERT_EQ(cspace.get<int>(), 42);
	ASSERT_EQ(space.attachee<int>(), entity);

	space.remove<int>();

	ASSERT_FALSE(space.has<int>());

	space.attach<int>(entity, 42);
	space.destroy(entity);

	ASSERT_FALSE(space.has<int>());
}

TEST(Space, StandardViews) {
	Space space(SpaceAccessor("bar"));
	auto mview = space.view<int, char>();
	auto iview = space.view<int>();
	auto cview = space.view<char>();

	space.create(0, 'c');
	space.create(0);
	space.create(0, 'c');

	ASSERT_EQ(iview.size(), decltype(iview)::size_type{ 3 });
	ASSERT_EQ(cview.size(), decltype(cview)::size_type{ 2 });

	decltype(mview)::size_type cnt{ 0 };
	mview.each([&cnt](auto...) { ++cnt; });

	ASSERT_EQ(cnt, decltype(mview)::size_type{ 2 });
}

TEST(Space, PersistentViews) {
	Space space(SpaceAccessor("bar"));
	auto view = space.persistent<int, char>();

	ASSERT_TRUE((space.contains<int, char>()));
	ASSERT_FALSE((space.contains<int, double>()));

	space.prepare<int, double>();

	ASSERT_TRUE((space.contains<int, double>()));

	space.discard<int, double>();

	ASSERT_FALSE((space.contains<int, double>()));

	space.create(0, 'c');
	space.create(0);
	space.create(0, 'c');

	decltype(view)::size_type cnt{ 0 };
	view.each([&cnt](auto...) { ++cnt; });

	ASSERT_EQ(cnt, decltype(view)::size_type{ 2 });
}

TEST(Space, CleanStandardViewsAfterReset) {
	Space space(SpaceAccessor("bar"));
	auto view = space.view<int>();
	space.create(0);

	ASSERT_EQ(view.size(), Space::size_type{ 1 });

	space.reset();

	ASSERT_EQ(view.size(), Space::size_type{ 0 });
}

TEST(Space, CleanPersistentViewsAfterReset) {
	Space space(SpaceAccessor("bar"));
	auto view = space.persistent<int, char>();
	space.create(0, 'c');

	ASSERT_EQ(view.size(), Space::size_type{ 1 });

	space.reset();

	ASSERT_EQ(view.size(), Space::size_type{ 0 });
}

TEST(Space, CleanTagsAfterReset) {
	Space space(SpaceAccessor("bar"));
	auto entity = space.create();
	space.attach<int>(entity);

	ASSERT_TRUE(space.has<int>());

	space.reset();

	ASSERT_FALSE(space.has<int>());
}

TEST(Space, SortSingle) {
	Space space(SpaceAccessor("bar"));

	int val = 0;

	space.create(val++);
	space.create(val++);
	space.create(val++);

	for (auto entity : space.view<int>()) {
		ASSERT_EQ(space.get<int>(entity), --val);
	}

	space.sort<int>(std::less<int>{});

	for (auto entity : space.view<int>()) {
		ASSERT_EQ(space.get<int>(entity), val++);
	}
}

TEST(Space, SortMulti) {
	Space space(SpaceAccessor("bar"));

	unsigned int uval = 0u;
	int ival = 0;

	space.create(uval++, ival++);
	space.create(uval++, ival++);
	space.create(uval++, ival++);

	for (auto entity : space.view<unsigned int>()) {
		ASSERT_EQ(space.get<unsigned int>(entity), --uval);
	}

	for (auto entity : space.view<int>()) {
		ASSERT_EQ(space.get<int>(entity), --ival);
	}

	space.sort<unsigned int>(std::less<unsigned int>{});
	space.sort<int, unsigned int>();

	for (auto entity : space.view<unsigned int>()) {
		ASSERT_EQ(space.get<unsigned int>(entity), uval++);
	}

	for (auto entity : space.view<int>()) {
		ASSERT_EQ(space.get<int>(entity), ival++);
	}
}

TEST(Space, ComponentsWithTypesFromStandardTemplateLibrary) {
	// see #37 - the test shouldn't crash, that's all
	Space space(SpaceAccessor("bar"));
	auto entity = space.create();
	space.assign<std::unordered_set<int>>(entity).insert(42);
	space.destroy(entity);
}

TEST(Space, ConstructWithComponents) {
	// it should compile, that's all
	Space space(SpaceAccessor("bar"));
	const auto value = 0;
	space.create(value);
}