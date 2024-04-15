#include <gtest/gtest.h>
#include <chrono>
#include "Silva"

#define MAKE_DUMMY(n) struct dummy##n { int x; }

MAKE_DUMMY(0);
MAKE_DUMMY(1);
MAKE_DUMMY(2);
MAKE_DUMMY(3);
MAKE_DUMMY(4);
MAKE_DUMMY(5);
MAKE_DUMMY(6);
MAKE_DUMMY(7);
MAKE_DUMMY(8);
MAKE_DUMMY(9);
MAKE_DUMMY(10);

TEST(Entities, create_registry)
{
    hl::silva::Registry r;
}

TEST(Entities, create_entity)
{
    hl::silva::Registry r;

    hl::silva::Entity e = r.spawn_entity();
}

TEST(Entities, test_entity_id)
{
    hl::silva::Registry r;
    hl::silva::Entity e = r.spawn_entity();
    EXPECT_EQ(e.get_id(), 0);
}

TEST(Entities, test_entity_id_after_creation)
{
    hl::silva::Registry r;
    hl::silva::Entity e = r.spawn_entity();
    EXPECT_EQ(e.get_id(), 0);
    e = r.spawn_entity();
    EXPECT_EQ(e.get_id(), 1);
}

TEST(Entities, test_kill_entity)
{
    hl::silva::Registry r;
    hl::silva::Entity e = r.spawn_entity();
    EXPECT_EQ(e.get_id(), 0);
    r.kill_entity(e);
    e = r.spawn_entity();
    EXPECT_EQ(e.get_id(), 1);
    r.update(); // remove killed entities
    e = r.spawn_entity();
    EXPECT_EQ(e.get_id(), 0);
}

TEST(Components, test_emplace_component_to_entity)
{
    hl::silva::Registry r;
    hl::silva::Entity e = r.spawn_entity();

    r.register_components<dummy1>();
    r.emplace<dummy1>(e);
}

TEST(Components, test_emplace_component_to_entity_2)
{
    hl::silva::Registry r;
    hl::silva::Entity e = r.spawn_entity();

    r.register_components<dummy1>();
    r.emplace<dummy1>(e, 1);
}

TEST(Components, test_get_component_from_entity_eq)
{
    hl::silva::Registry r;
    hl::silva::Entity e = r.spawn_entity();

    r.register_components<dummy1>();
    r.emplace<dummy1>(e, 1);

    auto& c = r.get<dummy1>(e);
    EXPECT_EQ(c.x, 1);
}

TEST(Components, test_get_component_from_entity_after_removal)
{
    hl::silva::Registry r;
    hl::silva::Entity e = r.spawn_entity();

    r.register_components<dummy1>();
    r.emplace<dummy1>(e, 1);

    r.remove<dummy1>(e);
    try {
        auto& c = r.get<dummy1>(e);
    } catch (const hl::silva::Error& e) {
        return;
    }
    GTEST_FAIL();
}

TEST(Components, test_get_component_from_entity_after_removal_2)
{
    hl::silva::Registry r;
    hl::silva::Entity e = r.spawn_entity();

    r.register_components<dummy1>();
    r.emplace<dummy1>(e, 1);
    r.kill_entity(e);

    try {
        auto& c = r.get<dummy1>(e);
    } catch (const hl::silva::Error& e) {
        GTEST_FAIL();
    }
    r.update();
    try {
        auto& c = r.get<dummy1>(e);
    } catch (const hl::silva::Error& e) {
        return;
    }
    GTEST_FAIL();
}

TEST(Components, test_get_component_from_entity_after_removal_3)
{
    hl::silva::Registry r;
    hl::silva::Entity e = r.spawn_entity();

    r.register_components<dummy1>();
    r.emplace<dummy1>(e, 1);
    r.kill_entity(e);

    try {
        auto& c = r.get<dummy1>(e);
    } catch (const hl::silva::Error& e) {
        GTEST_FAIL();
    }
    r.spawn_entity();
    try {
        auto& c = r.get<dummy1>(e);
    } catch (const hl::silva::Error& e) {
        GTEST_FAIL();
    }

    r.update();

    try {
        auto& c = r.get<dummy1>(e);
    } catch (const hl::silva::Error& e) {
        return;
    }
    GTEST_FAIL();
}

TEST(Components, test_emplace_same_component)
{
    hl::silva::Registry r;
    hl::silva::Entity e = r.spawn_entity();

    r.register_components<dummy1>();
    r.emplace<dummy1>(e, 1);
    r.emplace<dummy1>(e, 2);

    auto& c = r.get<dummy1>(e);
    EXPECT_EQ(c.x, 2);
}

TEST(Components, test_emplace_multiple_components_to_entity)
{
    hl::silva::Registry r;
    hl::silva::Entity e = r.spawn_entity();

    r.register_components<dummy1, dummy2>();

    r.emplace<dummy1>(e, 1);
    r.emplace<dummy2>(e, 2);

    auto& c1 = r.get<dummy1>(e);
    auto& c2 = r.get<dummy2>(e);
    EXPECT_EQ(c1.x, 1);
    EXPECT_EQ(c2.x, 2);
}

TEST(Systems, test_system_creation)
{
    hl::silva::Registry r;
    r.add_system([](hl::silva::Registry& r) {});
}

TEST(Systems, test_system_creation_2)
{
    hl::silva::Registry r;
    r.add_system([](hl::silva::Registry& r) {});
    r.add_system([](hl::silva::Registry& r) {});
}

TEST(Systems, test_system_test_update)
{
    hl::silva::Registry r;
    hl::silva::Entity e = r.spawn_entity();

    r.register_components<dummy0>();

    r.emplace<dummy0>(e, 1);
    r.add_system([&e](hl::silva::Registry& r) {
        r.get<dummy0>(e).x = 2;
    });
    EXPECT_EQ(r.get<dummy0>(e).x, 1);
    r.update();
    EXPECT_EQ(r.get<dummy0>(e).x, 2);
}

TEST(Systems, test_system_update_after_removal)
{
    hl::silva::Registry r;
    hl::silva::Entity e = r.spawn_entity();

    r.register_components<dummy0>();

    r.emplace<dummy0>(e, 1);
    r.add_system([&e](hl::silva::Registry& r) { r.get<dummy0>(e).x = 2; });
    EXPECT_EQ(r.get<dummy0>(e).x, 1);
    r.update();
    EXPECT_EQ(r.get<dummy0>(e).x, 2);
    r.remove_system();
    r.get<dummy0>(e).x = 3;
    r.update();
    EXPECT_EQ(r.get<dummy0>(e).x, 3);
}

TEST(Systems, test_system_update_after_removal_2)
{
    hl::silva::Registry r;
    hl::silva::Entity e = r.spawn_entity();

    r.register_components<dummy0>();

    r.emplace<dummy0>(e, 1);
    r.add_system([&e](hl::silva::Registry& r) {
        r.get<dummy0>(e).x = 2;
    });
    r.update();
    EXPECT_EQ(r.get<dummy0>(e).x, 2);
    r.remove_system("dummy0");
    r.spawn_entity();
    r.get<dummy0>(e).x = 3;
    r.update();
    EXPECT_EQ(r.get<dummy0>(e).x, 3);
}

TEST(Systems, test_multiple_systems)
{
    hl::silva::Registry r;
    hl::silva::Entity e = r.spawn_entity();

    r.register_components<dummy0, dummy1>();

    r.emplace<dummy0>(e, 1);
    r.emplace<dummy1>(e, 2);
    r.add_system([&e](hl::silva::Registry& r) {
        r.get<dummy0>(e).x += 4;
    });
    r.add_system([&e](hl::silva::Registry& r) {
        r.get<dummy1>(e).x += 1;
    });
    r.update();
    r.update();
    r.update();
    r.update();
    EXPECT_EQ(r.get<dummy0>(e).x, 17);
    EXPECT_EQ(r.get<dummy1>(e).x, 6);
}

TEST(Systems, make_multiple_entities_and_remove_them)
{
    hl::silva::Registry r;

    r.register_components<dummy0, dummy1>();

    for (unsigned int i = 0; i < 100; i++) {
        hl::silva::Entity e = r.spawn_entity();
        r.emplace<dummy0>(e, 1);
        r.emplace<dummy1>(e, 2);
    }

    EXPECT_EQ(r.entities_count(), 100);

    r.add_system([](hl::silva::Registry& r) {
        for (auto [e, _, __] : r.view<dummy0, dummy1>()) {
            r.kill_entity(e);
        }
    });

    r.update();
    EXPECT_EQ(r.entities_count(), 0);
}

TEST(Views, test_create_views)
{
    hl::silva::Registry r;
    r.register_components<dummy0>();
    r.view<dummy0>();
}

TEST(Views, test_create_views2)
{
    hl::silva::Registry r;
    r.register_components<dummy0, dummy1>();
    r.view<dummy0, dummy1>();
}

TEST(Views, test_get_entity_from_view)
{
    hl::silva::Registry r;
    r.register_components<dummy0, dummy1>();
    hl::silva::Entity e = r.spawn_entity();
    r.emplace<dummy0>(e, 1);
    r.emplace<dummy1>(e, 2);
    auto view = r.view<dummy0, dummy1>();

    int i = 0;
    for (const auto& v : view) {
        EXPECT_EQ(hl::silva::get<dummy0>(v).x, 1);
        EXPECT_EQ(hl::silva::get<dummy1>(v).x, 2);
        i++;
    }
    EXPECT_EQ(i, 1);
}

TEST(Views, test_get_entity_from_view_2)
{
    hl::silva::Registry r;
    const unsigned int entityCount = 1000;

    r.register_components<dummy0, dummy1>();
    for (unsigned int i = 0; i < entityCount; i++) {
        hl::silva::Entity e = r.spawn_entity();
        r.emplace<dummy0>(e, 1);
        r.emplace<dummy1>(e, 2);
    }
    int i = 0;
    for (const auto& v : r.view<dummy0, dummy1>()) {
        i++;
    }
    EXPECT_EQ(i, entityCount);
}

TEST(Views, test_get_entity_from_non_matching_view)
{
    hl::silva::Registry r;

    r.register_components<dummy0, dummy1, dummy3>();

    auto view = r.view<dummy3>();
    const unsigned int entityCount = 100;

    for (unsigned int i = 0; i < entityCount; i++) {
        hl::silva::Entity e = r.spawn_entity();
        r.emplace<dummy0>(e, 1);
        r.emplace<dummy1>(e, 2);
    }
    int i = 0;
    for (const auto& v : view)
        i++;
    EXPECT_EQ(i, 0);
}

TEST(Views, test_expanded_ranged_for)
{
    hl::silva::Registry r;
    const unsigned int entityCount = 100;

    r.register_components<dummy0, dummy1>();
    for (unsigned int i = 0; i < entityCount; i++) {
        hl::silva::Entity e = r.spawn_entity();
        r.emplace<dummy0>(e, 1);
        r.emplace<dummy1>(e, 2);
    }
    int i = 0;
    for (const auto& [e, d0, d1] : r.view<dummy0, dummy1>()) {
        EXPECT_EQ(e.get_id(), i);
        EXPECT_EQ(d0.x, 1);
        EXPECT_EQ(d1.x, 2);
        i++;
    }
    EXPECT_EQ(i, entityCount);
}

TEST(Views, each)
{
    hl::silva::Registry r;
    const unsigned int entityCount = 100;

    r.register_components<dummy0, dummy1>();
    for (unsigned int i = 0; i < entityCount; i++) {
        hl::silva::Entity e = r.spawn_entity();
        r.emplace<dummy0>(e, 1);
        r.emplace<dummy1>(e, 2);
    }
    hl::silva::Entity::Id i = 0;
    r.view<dummy0, dummy1>().each([&i](const dummy0& d0, const dummy1& d1) {
        EXPECT_EQ(d0.x, 1);
        EXPECT_EQ(d1.x, 2);
        i++;
    });
    EXPECT_EQ(i, entityCount);
}

TEST(Views, each2)
{
    hl::silva::Registry r;
    const unsigned int entityCount = 100;

    r.register_components<dummy0, dummy1>();
    for (unsigned int i = 0; i < entityCount; i++) {
        hl::silva::Entity e = r.spawn_entity();
        r.emplace<dummy0>(e, 1);
        r.emplace<dummy1>(e, 2);
    }
    hl::silva::Entity::Id i = 0;
    r.view<dummy0, dummy1>().each2([&i](const hl::silva::Entity& e, const dummy0& d0, const dummy1& d1) {
        EXPECT_EQ(e.get_id(), i);
        EXPECT_EQ(d0.x, 1);
        EXPECT_EQ(d1.x, 2);
        i++;
    });
    EXPECT_EQ(i, entityCount);
}

TEST(SampleCase, sample1_with_r)
{
    hl::silva::Registry r;
    hl::silva::Entity e = r.spawn_entity();
    hl::silva::Entity e2 = r.spawn_entity();

    r.register_components<dummy0, dummy1, dummy2>();
    r.emplace<dummy0>(e, 1)
        .emplace_r<dummy1>(1)
        .emplace_r<dummy2>(1);

    r.emplace<dummy1>(e2, 2).emplace_r<dummy2>(3);

    hl::silva::View<dummy0, dummy1> v(r);

    v.each([](dummy0& v, dummy1& s) {
        v.x++;
        s.x += 2;
    });

    v.each2([](const hl::silva::Entity& e, const dummy0& v, dummy1& s) {
        s.x++;
    });

    v.each([](dummy0& v, dummy1& s) {
        v.x++;
        s.x += 2;
    });

    EXPECT_EQ(r.get<dummy0>(e).x, 3);
    EXPECT_EQ(r.get<dummy1>(e).x, 6);

    try {
        EXPECT_EQ(r.get<dummy0>(e2).x, 1);
    } catch (const hl::silva::Error& e) {
    }

    EXPECT_EQ(r.get<dummy1>(e2).x, 2);
}

static void random_entities_components_test_speed(const unsigned int entityCount, const unsigned time_limit_ms)
{
    if (std::getenv("SHLVL") != NULL && std::getenv("LD_PRELOAD") != NULL) {
        // GTEST_FATAL_FAILURE_(std::string("Not running speed tests with valgrind on").c_str());
        // return;
    }
    std::srand(std::time(nullptr));
    double start = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::high_resolution_clock::now().time_since_epoch()).count();
    hl::silva::Registry r;
    double elapsed;
    r.register_components<dummy0, dummy1, dummy2>();
    for (unsigned int i = 0; i < entityCount; i++) {
        hl::silva::Entity e = r.spawn_entity();
        r.emplace<dummy0>(e, 1)
            .emplace_r<dummy1>(1)
            .emplace_r<dummy2>(1);
        if (std::rand() % 2 == 0) {
            r.emplace<dummy1>(e, 2).emplace_r<dummy2>(3);
            r.kill_entity(hl::silva::Entity(std::rand() % r.entities_count()));
        }
        elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::high_resolution_clock::now().time_since_epoch()).count() - start;
    }
    if (elapsed > time_limit_ms) {
        GTEST_FATAL_FAILURE_((std::string("Time limit exceeded: ") + std::to_string(elapsed) + " < " + std::to_string(time_limit_ms)).c_str());
    }
}

TEST(Speed, create_random_entities_and_components_easy)
{
    random_entities_components_test_speed(1, 5);
}

TEST(Speed, create_random_entities_and_components_easy2)
{
    random_entities_components_test_speed(10, 5);
}

TEST(Speed, create_random_entities_and_components_easy3)
{
    random_entities_components_test_speed(100, 5);
}

TEST(Speed, create_random_entities_and_components_easy4)
{
    random_entities_components_test_speed(200, 10);
}

TEST(Speed, create_random_entities_and_components_easy5)
{
    random_entities_components_test_speed(400, 30);
}

TEST(Speed, create_random_entities_and_components_easy6)
{
    random_entities_components_test_speed(1000, 50);
}

TEST(Speed, create_random_entities_and_components_medium)
{
    random_entities_components_test_speed(5000, 200);
}

TEST(Speed, create_random_entities_and_components_medium2)
{
    random_entities_components_test_speed(8000, 250);
}

TEST(Speed, create_random_entities_and_components_medium3)
{
    random_entities_components_test_speed(8000, 300);
}

TEST(Speed, create_random_entities_and_components_hard)
{
    random_entities_components_test_speed(15000, 500);
}

TEST(Speed, create_random_entities_and_components_hard2)
{
    random_entities_components_test_speed(20000, 500);
}

TEST(Speed, create_random_entities_and_components_hard3)
{
    random_entities_components_test_speed(30000, 500);
}

TEST(Speed, create_random_entities_and_components_rly_hard)
{
    random_entities_components_test_speed(100000, 500);
}

TEST(Speed, create_random_entities_and_components_rly_hard2)
{
    random_entities_components_test_speed(500000, 500);
}

TEST(Speed, create_random_entities_and_components_game_engine)
{
    random_entities_components_test_speed(1000000, 500);
}

TEST(Speed, create_random_entities_and_components_game_enginex2)
{
    random_entities_components_test_speed(2000000, 500);
}

TEST(Speed, create_random_entities_and_components_game_enginex3)
{
    random_entities_components_test_speed(3000000, 500);
}

TEST(Speed, create_random_entities_and_components_game_enginex4)
{
    random_entities_components_test_speed(4000000, 500);
}

TEST(Speed, create_random_entities_and_components_game_enginex5)
{
    random_entities_components_test_speed(5000000, 500);
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
