#include <unordered_map>
#include <unordered_set>
#include <functional>
#include <type_traits>
#include <gtest/gtest.h>
#include <entt/entity/entt_traits.hpp>
#include <entt/entity/container.hpp>

using DefaultContainer = entt::Container<std::uint32_t, struct TagFamily, struct ComponentFamily, struct ViewFamily>;

TEST(DefaultContainer, Functionalities) {
	DefaultContainer container;

	ASSERT_EQ(container.size(), DefaultContainer::size_type{ 0 });
	ASSERT_NO_THROW(container.reserve(42));
	ASSERT_NO_THROW(container.reserve<int>(8));
	ASSERT_NO_THROW(container.reserve<char>(8));
	ASSERT_TRUE(container.empty());

	ASSERT_EQ(container.capacity(), DefaultContainer::size_type{ 0 });
	ASSERT_EQ(container.size<int>(), DefaultContainer::size_type{ 0 });
	ASSERT_EQ(container.size<char>(), DefaultContainer::size_type{ 0 });
	ASSERT_TRUE(container.empty<int>());
	ASSERT_TRUE(container.empty<char>());

	auto e0 = container.create();
	auto e1 = container.create<int, char>();

	ASSERT_TRUE(container.has<>(e0));
	ASSERT_TRUE(container.has<>(e1));

	ASSERT_EQ(container.capacity(), DefaultContainer::size_type{ 2 });
	ASSERT_EQ(container.size<int>(), DefaultContainer::size_type{ 1 });
	ASSERT_EQ(container.size<char>(), DefaultContainer::size_type{ 1 });
	ASSERT_FALSE(container.empty<int>());
	ASSERT_FALSE(container.empty<char>());

	ASSERT_NE(e0, e1);

	ASSERT_FALSE(container.has<int>(e0));
	ASSERT_TRUE(container.has<int>(e1));
	ASSERT_FALSE(container.has<char>(e0));
	ASSERT_TRUE(container.has<char>(e1));
	ASSERT_FALSE((container.has<int, char>(e0)));
	ASSERT_TRUE((container.has<int, char>(e1)));

	ASSERT_EQ(container.assign<int>(e0, 42), 42);
	ASSERT_EQ(container.assign<char>(e0, 'c'), 'c');
	ASSERT_NO_THROW(container.remove<int>(e1));
	ASSERT_NO_THROW(container.remove<char>(e1));

	ASSERT_TRUE(container.has<int>(e0));
	ASSERT_FALSE(container.has<int>(e1));
	ASSERT_TRUE(container.has<char>(e0));
	ASSERT_FALSE(container.has<char>(e1));
	ASSERT_TRUE((container.has<int, char>(e0)));
	ASSERT_FALSE((container.has<int, char>(e1)));

	auto e2 = container.create();

	container.accomodate<int>(e2, container.get<int>(e0));
	container.accomodate<char>(e2, container.get<char>(e0));

	ASSERT_TRUE(container.has<int>(e2));
	ASSERT_TRUE(container.has<char>(e2));
	ASSERT_EQ(container.get<int>(e0), 42);
	ASSERT_EQ(container.get<char>(e0), 'c');

	ASSERT_EQ(std::get<0>(container.get<int, char>(e0)), 42);
	ASSERT_EQ(std::get<1>(static_cast<const DefaultContainer &>(container).get<int, char>(e0)), 'c');

	ASSERT_EQ(container.get<int>(e0), container.get<int>(e2));
	ASSERT_EQ(container.get<char>(e0), container.get<char>(e2));
	ASSERT_NE(&container.get<int>(e0), &container.get<int>(e2));
	ASSERT_NE(&container.get<char>(e0), &container.get<char>(e2));

	ASSERT_NO_THROW(container.replace<int>(e0, 0));
	ASSERT_EQ(container.get<int>(e0), 0);

	ASSERT_NO_THROW(container.accomodate<int>(e0, 1));
	ASSERT_NO_THROW(container.accomodate<int>(e1, 1));
	ASSERT_EQ(static_cast<const DefaultContainer &>(container).get<int>(e0), 1);
	ASSERT_EQ(static_cast<const DefaultContainer &>(container).get<int>(e1), 1);

	ASSERT_EQ(container.size(), DefaultContainer::size_type{ 3 });
	ASSERT_FALSE(container.empty());

	ASSERT_EQ(container.version(e2), DefaultContainer::version_type{ 0 });
	ASSERT_EQ(container.current(e2), DefaultContainer::version_type{ 0 });
	ASSERT_EQ(container.capacity(), DefaultContainer::size_type{ 3 });
	ASSERT_NO_THROW(container.destroy(e2));
	ASSERT_EQ(container.capacity(), DefaultContainer::size_type{ 3 });
	ASSERT_EQ(container.version(e2), DefaultContainer::version_type{ 0 });
	ASSERT_EQ(container.current(e2), DefaultContainer::version_type{ 1 });

	ASSERT_TRUE(container.valid(e0));
	ASSERT_TRUE(container.valid(e1));
	ASSERT_FALSE(container.valid(e2));

	ASSERT_EQ(container.size(), DefaultContainer::size_type{ 2 });
	ASSERT_FALSE(container.empty());

	ASSERT_NO_THROW(container.reset());

	ASSERT_EQ(container.size(), DefaultContainer::size_type{ 0 });
	ASSERT_TRUE(container.empty());

	container.create<int, char>();

	ASSERT_EQ(container.size<int>(), DefaultContainer::size_type{ 1 });
	ASSERT_EQ(container.size<char>(), DefaultContainer::size_type{ 1 });
	ASSERT_FALSE(container.empty<int>());
	ASSERT_FALSE(container.empty<char>());

	ASSERT_NO_THROW(container.reset<int>());

	ASSERT_EQ(container.size<int>(), DefaultContainer::size_type{ 0 });
	ASSERT_EQ(container.size<char>(), DefaultContainer::size_type{ 1 });
	ASSERT_TRUE(container.empty<int>());
	ASSERT_FALSE(container.empty<char>());

	ASSERT_NO_THROW(container.reset());

	ASSERT_EQ(container.size<int>(), DefaultContainer::size_type{ 0 });
	ASSERT_EQ(container.size<char>(), DefaultContainer::size_type{ 0 });
	ASSERT_TRUE(container.empty<int>());
	ASSERT_TRUE(container.empty<char>());

	e0 = container.create<int>();
	e1 = container.create();

	ASSERT_NO_THROW(container.reset<int>(e0));
	ASSERT_NO_THROW(container.reset<int>(e1));

	ASSERT_EQ(container.size<int>(), DefaultContainer::size_type{ 0 });
	ASSERT_EQ(container.size<char>(), DefaultContainer::size_type{ 0 });
	ASSERT_TRUE(container.empty<int>());
}

TEST(DefaultContainer, CreateDestroyCornerCase) {
	DefaultContainer container;

	auto e0 = container.create();
	auto e1 = container.create();

	container.destroy(e0);
	container.destroy(e1);

	container.each([](auto) { FAIL(); });

	ASSERT_EQ(container.current(e0), DefaultContainer::version_type{ 1 });
	ASSERT_EQ(container.current(e1), DefaultContainer::version_type{ 1 });
}

TEST(DefaultContainer, VersionOverflow) {
	DefaultContainer container;

	auto entity = container.create();
	container.destroy(entity);

	ASSERT_EQ(container.version(entity), DefaultContainer::version_type{});

	for (auto i = entt::entt_traits<DefaultContainer::entity_type>::version_mask; i; --i) {
		ASSERT_NE(container.current(entity), container.version(entity));
		container.destroy(container.create());
	}

	ASSERT_EQ(container.current(entity), container.version(entity));
}

TEST(DefaultContainer, Each) {
	DefaultContainer container;
	DefaultContainer::size_type tot;
	DefaultContainer::size_type match;

	container.create();
	container.create<int>();
	container.create();
	container.create<int>();
	container.create();

	tot = 0u;
	match = 0u;

	container.each([&](auto entity) {
		if (container.has<int>(entity)) { ++match; }
		container.create();
		++tot;
	});

	ASSERT_EQ(tot, 5u);
	ASSERT_EQ(match, 2u);

	tot = 0u;
	match = 0u;

	container.each([&](auto entity) {
		if (container.has<int>(entity)) {
			container.destroy(entity);
			++match;
		}

		++tot;
	});

	ASSERT_EQ(tot, 10u);
	ASSERT_EQ(match, 2u);

	tot = 0u;
	match = 0u;

	container.each([&](auto entity) {
		if (container.has<int>(entity)) { ++match; }
		container.destroy(entity);
		++tot;
	});

	ASSERT_EQ(tot, 8u);
	ASSERT_EQ(match, 0u);

	container.each([&](auto) { FAIL(); });
}

TEST(DefaultContainer, Orphans) {
	DefaultContainer container;
	DefaultContainer::size_type tot{};

	container.create<int>();
	container.create();
	container.create<int>();
	container.create();
	container.attach<double>(container.create());

	container.orphans([&](auto) { ++tot; });
	ASSERT_EQ(tot, 2u);
	tot = 0u;

	container.each([&](auto entity) { container.reset<int>(entity); });
	container.orphans([&](auto) { ++tot; });
	ASSERT_EQ(tot, 4u);
	container.reset();
	tot = 0u;

	container.orphans([&](auto) { ++tot; });
	ASSERT_EQ(tot, 0u);
}

TEST(DefaultContainer, Types) {
	DefaultContainer container;

	ASSERT_EQ(container.tag<int>(), container.tag<int>());
	ASSERT_EQ(container.component<int>(), container.component<int>());

	ASSERT_NE(container.tag<int>(), container.tag<double>());
	ASSERT_NE(container.component<int>(), container.component<double>());
}

TEST(DefaultContainer, CreateDestroyEntities) {
	DefaultContainer container;
	DefaultContainer::entity_type pre{}, post{};

	for (int i = 0; i < 10; ++i) {
		container.create<double>();
	}

	container.reset();

	for (int i = 0; i < 7; ++i) {
		auto entity = container.create<int>();
		if (i == 3) { pre = entity; }
	}

	container.reset();

	for (int i = 0; i < 5; ++i) {
		auto entity = container.create();
		if (i == 3) { post = entity; }
	}

	ASSERT_FALSE(container.valid(pre));
	ASSERT_TRUE(container.valid(post));
	ASSERT_NE(container.version(pre), container.version(post));
	ASSERT_EQ(container.version(pre) + 1, container.version(post));
	ASSERT_EQ(container.current(pre), container.current(post));
}

TEST(DefaultContainer, AttachRemoveTags) {
	DefaultContainer container;
	const auto &ccontainer = container;

	ASSERT_FALSE(container.has<int>());

	auto entity = container.create();
	container.attach<int>(entity, 42);

	ASSERT_TRUE(container.has<int>());
	ASSERT_EQ(container.get<int>(), 42);
	ASSERT_EQ(ccontainer.get<int>(), 42);
	ASSERT_EQ(container.attachee<int>(), entity);

	container.remove<int>();

	ASSERT_FALSE(container.has<int>());

	container.attach<int>(entity, 42);
	container.destroy(entity);

	ASSERT_FALSE(container.has<int>());
}

TEST(DefaultContainer, StandardViews) {
	DefaultContainer container;
	auto mview = container.view<int, char>();
	auto iview = container.view<int>();
	auto cview = container.view<char>();

	container.create(0, 'c');
	container.create(0);
	container.create(0, 'c');

	ASSERT_EQ(iview.size(), decltype(iview)::size_type{ 3 });
	ASSERT_EQ(cview.size(), decltype(cview)::size_type{ 2 });

	decltype(mview)::size_type cnt{ 0 };
	mview.each([&cnt](auto...) { ++cnt; });

	ASSERT_EQ(cnt, decltype(mview)::size_type{ 2 });
}

TEST(DefaultContainer, PersistentViews) {
	DefaultContainer container;
	auto view = container.persistent<int, char>();

	ASSERT_TRUE((container.contains<int, char>()));
	ASSERT_FALSE((container.contains<int, double>()));

	container.prepare<int, double>();

	ASSERT_TRUE((container.contains<int, double>()));

	container.discard<int, double>();

	ASSERT_FALSE((container.contains<int, double>()));

	container.create(0, 'c');
	container.create(0);
	container.create(0, 'c');

	decltype(view)::size_type cnt{ 0 };
	view.each([&cnt](auto...) { ++cnt; });

	ASSERT_EQ(cnt, decltype(view)::size_type{ 2 });
}

TEST(DefaultContainer, CleanStandardViewsAfterReset) {
	DefaultContainer container;
	auto view = container.view<int>();
	container.create(0);

	ASSERT_EQ(view.size(), DefaultContainer::size_type{ 1 });

	container.reset();

	ASSERT_EQ(view.size(), DefaultContainer::size_type{ 0 });
}

TEST(DefaultContainer, CleanPersistentViewsAfterReset) {
	DefaultContainer container;
	auto view = container.persistent<int, char>();
	container.create(0, 'c');

	ASSERT_EQ(view.size(), DefaultContainer::size_type{ 1 });

	container.reset();

	ASSERT_EQ(view.size(), DefaultContainer::size_type{ 0 });
}

TEST(DefaultContainer, CleanTagsAfterReset) {
	DefaultContainer container;
	auto entity = container.create();
	container.attach<int>(entity);

	ASSERT_TRUE(container.has<int>());

	container.reset();

	ASSERT_FALSE(container.has<int>());
}

TEST(DefaultContainer, SortSingle) {
	DefaultContainer container;

	int val = 0;

	container.create(val++);
	container.create(val++);
	container.create(val++);

	for (auto entity : container.view<int>()) {
		ASSERT_EQ(container.get<int>(entity), --val);
	}

	container.sort<int>(std::less<int>{});

	for (auto entity : container.view<int>()) {
		ASSERT_EQ(container.get<int>(entity), val++);
	}
}

TEST(DefaultContainer, SortMulti) {
	DefaultContainer container;

	unsigned int uval = 0u;
	int ival = 0;

	container.create(uval++, ival++);
	container.create(uval++, ival++);
	container.create(uval++, ival++);

	for (auto entity : container.view<unsigned int>()) {
		ASSERT_EQ(container.get<unsigned int>(entity), --uval);
	}

	for (auto entity : container.view<int>()) {
		ASSERT_EQ(container.get<int>(entity), --ival);
	}

	container.sort<unsigned int>(std::less<unsigned int>{});
	container.sort<int, unsigned int>();

	for (auto entity : container.view<unsigned int>()) {
		ASSERT_EQ(container.get<unsigned int>(entity), uval++);
	}

	for (auto entity : container.view<int>()) {
		ASSERT_EQ(container.get<int>(entity), ival++);
	}
}

TEST(DefaultContainer, ComponentsWithTypesFromStandardTemplateLibrary) {
	// see #37 - the test shouldn't crash, that's all
	DefaultContainer container;
	auto entity = container.create();
	container.assign<std::unordered_set<int>>(entity).insert(42);
	container.destroy(entity);
}

TEST(DefaultContainer, ConstructWithComponents) {
	// it should compile, that's all
	DefaultContainer container;
	const auto value = 0;
	container.create(value);
}

TEST(DefaultContainer, MergeTwoRegistries) {
	using entity_type = typename DefaultContainer::entity_type;

	DefaultContainer src;
	DefaultContainer dst;

	std::unordered_map<entity_type, entity_type> ref;

	auto merge = [&ref](const auto &view, auto &dst) {
		view.each([&](auto entity, const auto &component) {
			if (ref.find(entity) == ref.cend()) {
				ref.emplace(entity, dst.create(component));
			}
			else {
				using component_type = std::decay_t<decltype(component)>;
				dst.template assign<component_type>(ref[entity], component);
			}
		});
	};

	src.create<int, float, double>();
	src.create<char, float, int>();

	dst.create<int, char, double>();
	dst.create<float, int>();

	auto eq = [](auto begin, auto end) { ASSERT_EQ(begin, end); };
	auto ne = [](auto begin, auto end) { ASSERT_NE(begin, end); };

	eq(dst.view<int, float, double>().begin(), dst.view<int, float, double>().end());
	eq(dst.view<char, float, int>().begin(), dst.view<char, float, int>().end());

	merge(src.view<int>(), dst);
	merge(src.view<char>(), dst);
	merge(src.view<double>(), dst);
	merge(src.view<float>(), dst);

	ne(dst.view<int, float, double>().begin(), dst.view<int, float, double>().end());
	ne(dst.view<char, float, int>().begin(), dst.view<char, float, int>().end());
}
