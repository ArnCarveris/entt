#include <unordered_map>
#include <unordered_set>
#include <functional>
#include <type_traits>
#include <gtest/gtest.h>
#include <entt/entity/entt_traits.hpp>
#include <entt/entity/scene.hpp>

using SceneAccessor = entt::SceneAccessor<uint32_t, (std::uint64_t)entt::HashedString{ "foo" }>;
using Scene = entt::Scene<uint32_t, SceneAccessor::hash>;


TEST(Scene, Accessors) {
	using accessor_a_t = entt::SceneAccessor < uint32_t, (std::uint64_t)entt::HashedString{ "A" } >;
	using accessor_b_t = entt::SceneAccessor < uint32_t, (std::uint64_t)entt::HashedString{ "B" } >;

	using scene_a_t = entt::Scene<uint32_t, accessor_a_t::hash>;
	using scene_b_t = entt::Scene<uint32_t, accessor_b_t::hash>;

	scene_a_t scene_a(accessor_a_t("a"));
	scene_b_t scene_b(accessor_b_t("b"));

	auto e1 = scene_a.create<int, char, float>();
	auto e2 = scene_b.create<float>();

	scene_a.assign<accessor_b_t>(e1, scene_b.accessor());
	scene_b.assign<accessor_a_t>(e2, scene_a.accessor());

	ASSERT_EQ(scene_a.get<accessor_b_t>(e1), scene_b.accessor());
	ASSERT_EQ(scene_b.get<accessor_a_t>(e2), scene_a.accessor());

	ASSERT_EQ(scene_a.size(), scene_a_t::size_type{ 1 });
	ASSERT_EQ(scene_b.size(), scene_b_t::size_type{ 1 });
}

TEST(Scene, Functionalities) {
	Scene scene(SceneAccessor("bar"));

	ASSERT_EQ(scene.size(), Scene::size_type{ 0 });
	ASSERT_NO_THROW(scene.reserve(42));
	ASSERT_NO_THROW(scene.reserve<int>(8));
	ASSERT_NO_THROW(scene.reserve<char>(8));
	ASSERT_TRUE(scene.empty());

	ASSERT_EQ(scene.capacity(), Scene::size_type{ 0 });
	ASSERT_EQ(scene.size<int>(), Scene::size_type{ 0 });
	ASSERT_EQ(scene.size<char>(), Scene::size_type{ 0 });
	ASSERT_TRUE(scene.empty<int>());
	ASSERT_TRUE(scene.empty<char>());

	auto e0 = scene.create();
	auto e1 = scene.create<int, char>();

	ASSERT_TRUE(scene.has<>(e0));
	ASSERT_TRUE(scene.has<>(e1));

	ASSERT_EQ(scene.capacity(), Scene::size_type{ 2 });
	ASSERT_EQ(scene.size<int>(), Scene::size_type{ 1 });
	ASSERT_EQ(scene.size<char>(), Scene::size_type{ 1 });
	ASSERT_FALSE(scene.empty<int>());
	ASSERT_FALSE(scene.empty<char>());

	ASSERT_NE(e0, e1);

	ASSERT_FALSE(scene.has<int>(e0));
	ASSERT_TRUE(scene.has<int>(e1));
	ASSERT_FALSE(scene.has<char>(e0));
	ASSERT_TRUE(scene.has<char>(e1));
	ASSERT_FALSE((scene.has<int, char>(e0)));
	ASSERT_TRUE((scene.has<int, char>(e1)));

	ASSERT_EQ(scene.assign<int>(e0, 42), 42);
	ASSERT_EQ(scene.assign<char>(e0, 'c'), 'c');
	ASSERT_NO_THROW(scene.remove<int>(e1));
	ASSERT_NO_THROW(scene.remove<char>(e1));

	ASSERT_TRUE(scene.has<int>(e0));
	ASSERT_FALSE(scene.has<int>(e1));
	ASSERT_TRUE(scene.has<char>(e0));
	ASSERT_FALSE(scene.has<char>(e1));
	ASSERT_TRUE((scene.has<int, char>(e0)));
	ASSERT_FALSE((scene.has<int, char>(e1)));

	auto e2 = scene.create();

	scene.accomodate<int>(e2, scene.get<int>(e0));
	scene.accomodate<char>(e2, scene.get<char>(e0));

	ASSERT_TRUE(scene.has<int>(e2));
	ASSERT_TRUE(scene.has<char>(e2));
	ASSERT_EQ(scene.get<int>(e0), 42);
	ASSERT_EQ(scene.get<char>(e0), 'c');

	ASSERT_EQ(std::get<0>(scene.get<int, char>(e0)), 42);
	ASSERT_EQ(std::get<1>(static_cast<const Scene &>(scene).get<int, char>(e0)), 'c');

	ASSERT_EQ(scene.get<int>(e0), scene.get<int>(e2));
	ASSERT_EQ(scene.get<char>(e0), scene.get<char>(e2));
	ASSERT_NE(&scene.get<int>(e0), &scene.get<int>(e2));
	ASSERT_NE(&scene.get<char>(e0), &scene.get<char>(e2));

	ASSERT_NO_THROW(scene.replace<int>(e0, 0));
	ASSERT_EQ(scene.get<int>(e0), 0);

	ASSERT_NO_THROW(scene.accomodate<int>(e0, 1));
	ASSERT_NO_THROW(scene.accomodate<int>(e1, 1));
	ASSERT_EQ(static_cast<const Scene &>(scene).get<int>(e0), 1);
	ASSERT_EQ(static_cast<const Scene &>(scene).get<int>(e1), 1);

	ASSERT_EQ(scene.size(), Scene::size_type{ 3 });
	ASSERT_FALSE(scene.empty());

	ASSERT_EQ(scene.version(e2), Scene::version_type{ 0 });
	ASSERT_EQ(scene.current(e2), Scene::version_type{ 0 });
	ASSERT_EQ(scene.capacity(), Scene::size_type{ 3 });
	ASSERT_NO_THROW(scene.destroy(e2));
	ASSERT_EQ(scene.capacity(), Scene::size_type{ 3 });
	ASSERT_EQ(scene.version(e2), Scene::version_type{ 0 });
	ASSERT_EQ(scene.current(e2), Scene::version_type{ 1 });

	ASSERT_TRUE(scene.valid(e0));
	ASSERT_TRUE(scene.valid(e1));
	ASSERT_FALSE(scene.valid(e2));

	ASSERT_EQ(scene.size(), Scene::size_type{ 2 });
	ASSERT_FALSE(scene.empty());

	ASSERT_NO_THROW(scene.reset());

	ASSERT_EQ(scene.size(), Scene::size_type{ 0 });
	ASSERT_TRUE(scene.empty());

	scene.create<int, char>();

	ASSERT_EQ(scene.size<int>(), Scene::size_type{ 1 });
	ASSERT_EQ(scene.size<char>(), Scene::size_type{ 1 });
	ASSERT_FALSE(scene.empty<int>());
	ASSERT_FALSE(scene.empty<char>());

	ASSERT_NO_THROW(scene.reset<int>());

	ASSERT_EQ(scene.size<int>(), Scene::size_type{ 0 });
	ASSERT_EQ(scene.size<char>(), Scene::size_type{ 1 });
	ASSERT_TRUE(scene.empty<int>());
	ASSERT_FALSE(scene.empty<char>());

	ASSERT_NO_THROW(scene.reset());

	ASSERT_EQ(scene.size<int>(), Scene::size_type{ 0 });
	ASSERT_EQ(scene.size<char>(), Scene::size_type{ 0 });
	ASSERT_TRUE(scene.empty<int>());
	ASSERT_TRUE(scene.empty<char>());

	e0 = scene.create<int>();
	e1 = scene.create();

	ASSERT_NO_THROW(scene.reset<int>(e0));
	ASSERT_NO_THROW(scene.reset<int>(e1));

	ASSERT_EQ(scene.size<int>(), Scene::size_type{ 0 });
	ASSERT_EQ(scene.size<char>(), Scene::size_type{ 0 });
	ASSERT_TRUE(scene.empty<int>());
}

TEST(Scene, CreateDestroyCornerCase) {
	Scene scene(SceneAccessor("bar"));

	auto e0 = scene.create();
	auto e1 = scene.create();

	scene.destroy(e0);
	scene.destroy(e1);

	scene.each([](auto) { FAIL(); });

	ASSERT_EQ(scene.current(e0), Scene::version_type{ 1 });
	ASSERT_EQ(scene.current(e1), Scene::version_type{ 1 });
}

TEST(Scene, VersionOverflow) {
	Scene scene(SceneAccessor("bar"));

	auto entity = scene.create();
	scene.destroy(entity);

	ASSERT_EQ(scene.version(entity), Scene::version_type{});

	for (auto i = entt::entt_traits<Scene::entity_type>::version_mask; i; --i) {
		ASSERT_NE(scene.current(entity), scene.version(entity));
		scene.destroy(scene.create());
	}

	ASSERT_EQ(scene.current(entity), scene.version(entity));
}

TEST(Scene, Each) {
	Scene scene(SceneAccessor("bar"));
	Scene::size_type tot;
	Scene::size_type match;

	scene.create();
	scene.create<int>();
	scene.create();
	scene.create<int>();
	scene.create();

	tot = 0u;
	match = 0u;

	scene.each([&](auto entity) {
		if (scene.has<int>(entity)) { ++match; }
		scene.create();
		++tot;
	});

	ASSERT_EQ(tot, 5u);
	ASSERT_EQ(match, 2u);

	tot = 0u;
	match = 0u;

	scene.each([&](auto entity) {
		if (scene.has<int>(entity)) {
			scene.destroy(entity);
			++match;
		}

		++tot;
	});

	ASSERT_EQ(tot, 10u);
	ASSERT_EQ(match, 2u);

	tot = 0u;
	match = 0u;

	scene.each([&](auto entity) {
		if (scene.has<int>(entity)) { ++match; }
		scene.destroy(entity);
		++tot;
	});

	ASSERT_EQ(tot, 8u);
	ASSERT_EQ(match, 0u);

	scene.each([&](auto) { FAIL(); });
}

TEST(Scene, Orphans) {
	Scene scene(SceneAccessor("bar"));
	Scene::size_type tot{};

	scene.create<int>();
	scene.create();
	scene.create<int>();
	scene.create();
	scene.attach<double>(scene.create());

	scene.orphans([&](auto) { ++tot; });
	ASSERT_EQ(tot, 2u);
	tot = 0u;

	scene.each([&](auto entity) { scene.reset<int>(entity); });
	scene.orphans([&](auto) { ++tot; });
	ASSERT_EQ(tot, 4u);
	scene.reset();
	tot = 0u;

	scene.orphans([&](auto) { ++tot; });
	ASSERT_EQ(tot, 0u);
}

TEST(Scene, Types) {
	Scene scene(SceneAccessor("bar"));

	ASSERT_EQ(scene.tag<int>(), scene.tag<int>());
	ASSERT_EQ(scene.component<int>(), scene.component<int>());

	ASSERT_NE(scene.tag<int>(), scene.tag<double>());
	ASSERT_NE(scene.component<int>(), scene.component<double>());
}

TEST(Scene, CreateDestroyEntities) {
	Scene scene(SceneAccessor("bar"));
	Scene::entity_type pre{}, post{};

	for (int i = 0; i < 10; ++i) {
		scene.create<double>();
	}

	scene.reset();

	for (int i = 0; i < 7; ++i) {
		auto entity = scene.create<int>();
		if (i == 3) { pre = entity; }
	}

	scene.reset();

	for (int i = 0; i < 5; ++i) {
		auto entity = scene.create();
		if (i == 3) { post = entity; }
	}

	ASSERT_FALSE(scene.valid(pre));
	ASSERT_TRUE(scene.valid(post));
	ASSERT_NE(scene.version(pre), scene.version(post));
	ASSERT_EQ(scene.version(pre) + 1, scene.version(post));
	ASSERT_EQ(scene.current(pre), scene.current(post));
}

TEST(Scene, AttachRemoveTags) {
	Scene scene(SceneAccessor("bar"));
	const auto& cscene = scene;

	ASSERT_FALSE(scene.has<int>());

	auto entity = scene.create();
	scene.attach<int>(entity, 42);

	ASSERT_TRUE(scene.has<int>());
	ASSERT_EQ(scene.get<int>(), 42);
	ASSERT_EQ(cscene.get<int>(), 42);
	ASSERT_EQ(scene.attachee<int>(), entity);

	scene.remove<int>();

	ASSERT_FALSE(scene.has<int>());

	scene.attach<int>(entity, 42);
	scene.destroy(entity);

	ASSERT_FALSE(scene.has<int>());
}

TEST(Scene, StandardViews) {
	Scene scene(SceneAccessor("bar"));
	auto mview = scene.view<int, char>();
	auto iview = scene.view<int>();
	auto cview = scene.view<char>();

	scene.create(0, 'c');
	scene.create(0);
	scene.create(0, 'c');

	ASSERT_EQ(iview.size(), decltype(iview)::size_type{ 3 });
	ASSERT_EQ(cview.size(), decltype(cview)::size_type{ 2 });

	decltype(mview)::size_type cnt{ 0 };
	mview.each([&cnt](auto...) { ++cnt; });

	ASSERT_EQ(cnt, decltype(mview)::size_type{ 2 });
}

TEST(Scene, PersistentViews) {
	Scene scene(SceneAccessor("bar"));
	auto view = scene.persistent<int, char>();

	ASSERT_TRUE((scene.contains<int, char>()));
	ASSERT_FALSE((scene.contains<int, double>()));

	scene.prepare<int, double>();

	ASSERT_TRUE((scene.contains<int, double>()));

	scene.discard<int, double>();

	ASSERT_FALSE((scene.contains<int, double>()));

	scene.create(0, 'c');
	scene.create(0);
	scene.create(0, 'c');

	decltype(view)::size_type cnt{ 0 };
	view.each([&cnt](auto...) { ++cnt; });

	ASSERT_EQ(cnt, decltype(view)::size_type{ 2 });
}

TEST(Scene, CleanStandardViewsAfterReset) {
	Scene scene(SceneAccessor("bar"));
	auto view = scene.view<int>();
	scene.create(0);

	ASSERT_EQ(view.size(), Scene::size_type{ 1 });

	scene.reset();

	ASSERT_EQ(view.size(), Scene::size_type{ 0 });
}

TEST(Scene, CleanPersistentViewsAfterReset) {
	Scene scene(SceneAccessor("bar"));
	auto view = scene.persistent<int, char>();
	scene.create(0, 'c');

	ASSERT_EQ(view.size(), Scene::size_type{ 1 });

	scene.reset();

	ASSERT_EQ(view.size(), Scene::size_type{ 0 });
}

TEST(Scene, CleanTagsAfterReset) {
	Scene scene(SceneAccessor("bar"));
	auto entity = scene.create();
	scene.attach<int>(entity);

	ASSERT_TRUE(scene.has<int>());

	scene.reset();

	ASSERT_FALSE(scene.has<int>());
}

TEST(Scene, SortSingle) {
	Scene scene(SceneAccessor("bar"));

	int val = 0;

	scene.create(val++);
	scene.create(val++);
	scene.create(val++);

	for (auto entity : scene.view<int>()) {
		ASSERT_EQ(scene.get<int>(entity), --val);
	}

	scene.sort<int>(std::less<int>{});

	for (auto entity : scene.view<int>()) {
		ASSERT_EQ(scene.get<int>(entity), val++);
	}
}

TEST(Scene, SortMulti) {
	Scene scene(SceneAccessor("bar"));

	unsigned int uval = 0u;
	int ival = 0;

	scene.create(uval++, ival++);
	scene.create(uval++, ival++);
	scene.create(uval++, ival++);

	for (auto entity : scene.view<unsigned int>()) {
		ASSERT_EQ(scene.get<unsigned int>(entity), --uval);
	}

	for (auto entity : scene.view<int>()) {
		ASSERT_EQ(scene.get<int>(entity), --ival);
	}

	scene.sort<unsigned int>(std::less<unsigned int>{});
	scene.sort<int, unsigned int>();

	for (auto entity : scene.view<unsigned int>()) {
		ASSERT_EQ(scene.get<unsigned int>(entity), uval++);
	}

	for (auto entity : scene.view<int>()) {
		ASSERT_EQ(scene.get<int>(entity), ival++);
	}
}

TEST(Scene, ComponentsWithTypesFromStandardTemplateLibrary) {
	// see #37 - the test shouldn't crash, that's all
	Scene scene(SceneAccessor("bar"));
	auto entity = scene.create();
	scene.assign<std::unordered_set<int>>(entity).insert(42);
	scene.destroy(entity);
}

TEST(Scene, ConstructWithComponents) {
	// it should compile, that's all
	Scene scene(SceneAccessor("bar"));
	const auto value = 0;
	scene.create(value);
}