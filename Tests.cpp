#include "Silva.hpp"
#include <gtest/gtest.h>
#include <chrono>

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

using namespace silva;

TEST(Entities, create_registry)
{
    registry r;
}

TEST(Entities, create_entity)
{
    registry r;

    Entity e = r.newEntity();
}

TEST(Entities, test_entity_id)
{
    registry r;
    Entity e = r.newEntity();
    EXPECT_EQ(e.id, 0);
}

TEST(Entities, test_entity_id_after_creation)
{
    registry r;
    Entity e = r.newEntity();
    EXPECT_EQ(e.id, 0);
    e = r.newEntity();
    EXPECT_EQ(e.id, 1);
}

TEST(Entities, test_remove_entity)
{
    registry r;
    Entity e = r.newEntity();
    EXPECT_EQ(e.id, 0);
    r.removeEntity(e);
    e = r.newEntity();
    EXPECT_EQ(e.id, 0);
}

TEST(Components, test_emplace_component_to_entity)
{
    registry r;
    Entity e = r.newEntity();

    r.emplace<dummy1>(e);
}

TEST(Components, test_emplace_component_to_entity_2)
{
    registry r;
    Entity e = r.newEntity();

    r.emplace<dummy1>(e, 1);
}

TEST(Components, test_get_component_from_entity_eq)
{
    registry r;
    Entity e = r.newEntity();

    r.emplace<dummy1>(e, 1);

    auto& c = r.get<dummy1>(e);
    EXPECT_EQ(c.x, 1);
}

TEST(Components, test_get_component_from_entity_after_removal)
{
    registry r;
    Entity e = r.newEntity();

    r.emplace<dummy1>(e, 1);

    r.remove<dummy1>(e);
    try {
        auto& c = r.get<dummy1>(e);
    } catch (const Error& e) {
        return;
    }
    GTEST_FAIL();
}

TEST(Components, test_get_component_from_entity_after_removal_2)
{
    registry r;
    Entity e = r.newEntity();

    r.emplace<dummy1>(e, 1);
    r.removeEntity(e);

    try {
        auto& c = r.get<dummy1>(e);
    } catch (const Error& e) {
        return;
    }
    GTEST_FAIL();
}

TEST(Components, test_get_component_from_entity_after_removal_3)
{
    registry r;
    Entity e = r.newEntity();

    r.emplace<dummy1>(e, 1);
    r.removeEntity(e);
    r.newEntity();

    try {
        auto& c = r.get<dummy1>(e);
    } catch (const Error& e) {
        return;
    }
    GTEST_FAIL();
}

TEST(Components, test_emplace_same_component)
{
    registry r;
    Entity e = r.newEntity();

    r.emplace<dummy1>(e, 1);
    r.emplace<dummy1>(e, 2);

    auto& c = r.get<dummy1>(e);
    EXPECT_EQ(c.x, 2);
}

TEST(Components, test_emplace_multiple_components_to_entity)
{
    registry r;
    Entity e = r.newEntity();

    r.emplace<dummy1>(e, 1);
    r.emplace<dummy2>(e, 2);

    auto& c1 = r.get<dummy1>(e);
    auto& c2 = r.get<dummy2>(e);
    EXPECT_EQ(c1.x, 1);
    EXPECT_EQ(c2.x, 2);
}

TEST(Components, test_get_component_from_entity_after_removal_4)
{
    registry r;
    Entity e = r.newEntity();

    r.emplace<dummy1>(e, 1);
    r.emplace<dummy2>(e, 2);

    r.removeEntity(e);

    try {
        auto& c = r.get<dummy1>(e);
    } catch (const Error& e) {
        return;
    }
    GTEST_FAIL();
}

TEST(Components, test_get_component_from_entity_after_removal_5)
{
    registry r;
    Entity e = r.newEntity();

    r.emplace<dummy1>(e, 1);
    r.emplace<dummy2>(e, 2);

    r.removeEntity(e);
    r.newEntity();

    try {
        auto& c = r.get<dummy1>(e);
    } catch (const Error& e) {
        return;
    }
    GTEST_FAIL();
}

TEST(Systems, test_system_creation)
{
    registry r;
    r.addSystem<dummy0>("dummy0");
}

TEST(Systems, test_system_creation_2)
{
    registry r;
    r.addSystem<dummy0>("dummy0");
    r.addSystem<dummy1>("dummy1");
}

TEST(Systems, test_system_set_func)
{
    registry r;

    r.addSystem<dummy0>("dummy0");
    r.setSystemUpdate("dummy0", [](const Entity& e, registry& r) {});
}

TEST(Systems, test_system_test_update)
{
    registry r;
    Entity e = r.newEntity();

    r.emplace<dummy0>(e, 1);
    r.addSystem<dummy0>("dummy0");
    r.setSystemUpdate("dummy0", [](const Entity& e, registry& r) {
        r.get<dummy0>(e).x = 2;
    });
    r.update();
    EXPECT_EQ(r.get<dummy0>(e).x, 2);
}

TEST(Systems, test_system_test_update_2)
{
    registry r;
    Entity e = r.newEntity();

    r.emplace<dummy0>(e, 1);
    r.addSystem<dummy1>("dummy0");
    r.setSystemUpdate("dummy0", [](const Entity& e, registry& r) {
        r.get<dummy1>(e).x = 2;
    });
    r.update();
    EXPECT_EQ(r.get<dummy0>(e).x, 1);
}

TEST(Systems, test_multiple_systems)
{
    registry r;
    Entity e = r.newEntity();

    r.emplace<dummy0>(e, 1);
    r.addSystem<dummy0>("dummy0");
    r.addSystem<dummy1>("dummy1");
    r.setSystemUpdate("dummy0", [](const Entity& e, registry& r) {
        r.get<dummy0>(e).x = 2;
    });
    r.setSystemUpdate("dummy1", [](const Entity& e, registry& r) {
        r.get<dummy1>(e).x = 2;
        GTEST_FAIL();
    });
    r.update();
    EXPECT_EQ(r.get<dummy0>(e).x, 2);
    try {
        EXPECT_EQ(r.get<dummy1>(e).x, 1);
    } catch (const Error& e) {
        return;
    }
    GTEST_FAIL();
}

TEST(Systems, test_system_update_after_removal)
{
    registry r;
    Entity e = r.newEntity();

    r.emplace<dummy0>(e, 1);
    r.addSystem<dummy0>("dummy0");
    r.setSystemUpdate("dummy0", [](const Entity& e, registry& r) {
        r.get<dummy0>(e).x = 2;
    });
    r.update();
    EXPECT_EQ(r.get<dummy0>(e).x, 2);
    r.removeSystem("dummy0");
    r.get<dummy0>(e).x = 3;
    r.update();
    EXPECT_EQ(r.get<dummy0>(e).x, 3);
}

TEST(Systems, test_system_update_after_removal_2)
{
    registry r;
    Entity e = r.newEntity();

    r.emplace<dummy0>(e, 1);
    r.addSystem<dummy0>("dummy0");
    r.setSystemUpdate("dummy0", [](const Entity& e, registry& r) {
        r.get<dummy0>(e).x = 2;
    });
    r.update();
    EXPECT_EQ(r.get<dummy0>(e).x, 2);
    r.removeSystem("dummy0");
    r.newEntity();
    r.get<dummy0>(e).x = 3;
    r.update();
    EXPECT_EQ(r.get<dummy0>(e).x, 3);
}

TEST(Systems, test_system_with_multiple_components)
{
    registry r;
    Entity e = r.newEntity();

    r.emplace<dummy0>(e, 1);
    r.emplace<dummy1>(e, 2);
    r.addSystem<dummy0, dummy1>("dummy0");
    r.setSystemUpdate("dummy0", [](const Entity& e, registry& r) {
        r.get<dummy0>(e).x = 2;
    });
    r.update();
    EXPECT_EQ(r.get<dummy0>(e).x, 2);
    EXPECT_EQ(r.get<dummy1>(e).x, 2);
}

TEST(Systems, test_system_with_multiple_components_2)
{
    registry r;
    Entity e = r.newEntity();

    r.emplace<dummy0>(e, 1);
    r.addSystem<dummy0, dummy1>("dummy0");
    r.setSystemUpdate("dummy0", [](const Entity& e, registry& r) {
        r.get<dummy0>(e).x = 2;
    });
    r.update();
    EXPECT_EQ(r.get<dummy0>(e).x, 1);
}

TEST(Systems, test_system_with_multiple_components_3)
{
    registry r;
    Entity e = r.newEntity();

    r.emplace<dummy0>(e, 1);
    r.emplace<dummy1>(e, 2);
    r.addSystem<dummy0, dummy1>("dummy0");
    r.setSystemUpdate("dummy0", [](const Entity& e, registry& r) {
        r.get<dummy0>(e).x += 2;
        r.get<dummy1>(e).x += 2;
    });
    r.update();
    EXPECT_EQ(r.get<dummy0>(e).x, 3);
    EXPECT_EQ(r.get<dummy1>(e).x, 4);
}

TEST(Systems, test_multiple_systems_and_multiple_components)
{
    registry r;
    Entity e = r.newEntity();

    r.emplace<dummy0>(e, 1);
    r.emplace<dummy1>(e, 2);
    r.addSystem<dummy0>("dummy0");
    r.addSystem<dummy1>("dummy1");
    r.setSystemUpdate("dummy0", [](const Entity& e, registry& r) {
        r.get<dummy0>(e).x += 4;
    });
    r.setSystemUpdate("dummy1", [](const Entity& e, registry& r) {
        r.get<dummy1>(e).x += 1;
    });
    r.update().update().update().update();
    EXPECT_EQ(r.get<dummy0>(e).x, 17);
    EXPECT_EQ(r.get<dummy1>(e).x, 6);
}

TEST(Systems, test_system_with_multiple_components_4)
{
    registry r;
    Entity e = r.newEntity();

    r.emplace<dummy0>(e, 1);
    r.emplace<dummy1>(e, 2);
    r.addSystem<dummy0, dummy1>("dummy0");
    r.setSystemUpdate("dummy0", [](const Entity& e, registry& r) {
        r.get<dummy0>(e).x += 4;
        r.get<dummy1>(e).x += 1;
    });
    r.update();
    EXPECT_EQ(r.get<dummy0>(e).x, 5);
    EXPECT_EQ(r.get<dummy1>(e).x, 3);
}

TEST(Systems, make_multiple_entities_and_remove_them)
{
    registry r;
    for (unsigned int i = 0; i < 100; i++) {
        Entity e = r.newEntity();
        r.emplace<dummy0>(e, 1);
        r.emplace<dummy1>(e, 2);
    }
    r.addSystem<dummy0, dummy1>("dummy0");
    r.setSystemUpdate("dummy0", [](const Entity& e, registry& r) {
        r.removeEntity(e);
    });
    r.update();
    EXPECT_EQ(r.entitiesCount(), 0);
}

TEST(Views, test_create_views)
{
    registry r;
    r.view<dummy0>();
}

TEST(Views, test_create_views2)
{
    registry r;
    r.view<dummy0, dummy1>();
}

TEST(Views, test_get_entity_from_view)
{
    registry r;
    Entity e = r.newEntity();
    r.emplace<dummy0>(e, 1);
    r.emplace<dummy1>(e, 2);
    auto view = r.view<dummy0, dummy1>();

    int i = 0;
    for (auto& v : view) {
        EXPECT_EQ(get<dummy0>(v).x, 1);
        EXPECT_EQ(get<dummy1>(v).x, 2);
        i++;
    }
    EXPECT_EQ(i, 1);
}

TEST(Views, test_get_entity_from_view_2)
{
    registry r;
    const unsigned int entityCount = 1000;

    for (unsigned int i = 0; i < entityCount; i++) {
        Entity e = r.newEntity();
        r.emplace<dummy0>(e, 1);
        r.emplace<dummy1>(e, 2);
    }
    int i = 0;
    for (auto& v : r.view<dummy0, dummy1>()) {
        i++;
    }
    EXPECT_EQ(i, entityCount);
}

TEST(Views, test_get_entity_from_non_matching_view)
{
    registry r;
    auto view = r.view<dummy3>();
    const unsigned int entityCount = 100;

    for (unsigned int i = 0; i < entityCount; i++) {
        Entity e = r.newEntity();
        r.emplace<dummy0>(e, 1);
        r.emplace<dummy1>(e, 2);
    }
    int i = 0;
    for (auto& v : view)
        i++;
    EXPECT_EQ(i, 0);
}

TEST(Views, test_expanded_ranged_for)
{
    registry r;
    const unsigned int entityCount = 100;

    for (unsigned int i = 0; i < entityCount; i++) {
        Entity e = r.newEntity();
        r.emplace<dummy0>(e, 1);
        r.emplace<dummy1>(e, 2);
    }
    int i = 0;
    for (auto& [e, d0, d1] : r.view<dummy0, dummy1>()) {
        EXPECT_EQ(e.id, i);
        EXPECT_EQ(d0.x, 1);
        EXPECT_EQ(d1.x, 2);
        i++;
    }
    EXPECT_EQ(i, entityCount);
}

TEST(Views, each)
{
    registry r;
    const unsigned int entityCount = 100;

    for (unsigned int i = 0; i < entityCount; i++) {
        Entity e = r.newEntity();
        r.emplace<dummy0>(e, 1);
        r.emplace<dummy1>(e, 2);
    }
    EntityId i = 0;
    r.view<dummy0, dummy1>().each([&i](const dummy0& d0, const dummy1& d1) {
        EXPECT_EQ(d0.x, 1);
        EXPECT_EQ(d1.x, 2);
        i++;
    });
    EXPECT_EQ(i, entityCount);
}

TEST(Views, each2)
{
    registry r;
    const unsigned int entityCount = 100;

    for (unsigned int i = 0; i < entityCount; i++) {
        Entity e = r.newEntity();
        r.emplace<dummy0>(e, 1);
        r.emplace<dummy1>(e, 2);
    }
    EntityId i = 0;
    r.view<dummy0, dummy1>().each2([&i](const Entity& e, const dummy0& d0, const dummy1& d1) {
        EXPECT_EQ(e.id, i);
        EXPECT_EQ(d0.x, 1);
        EXPECT_EQ(d1.x, 2);
        i++;
    });
    EXPECT_EQ(i, entityCount);
}

TEST(Views, each2_entity)
{
    registry r;
    const unsigned int entityCount = 100;

    for (unsigned int i = 0; i < entityCount; i++) {
        Entity e = r.newEntity();
        r.emplace<dummy0>(e, 1);
        r.emplace<dummy1>(e, 2);
    }
    EntityId i = 0;
    r.view<dummy0, dummy1>().eachEntity([&i](const Entity& e, const dummy0& d0, const dummy1& d1) {
        EXPECT_EQ(e.id, i);
        EXPECT_EQ(d0.x, 1);
        EXPECT_EQ(d1.x, 2);
        i++;
    });
    EXPECT_EQ(i, entityCount);
}

TEST(SampleCase, sample1_with_r)
{
    registry r;
    Entity e = r.newEntity();
    Entity e2 = r.newEntity();

    r.emplace<dummy0>(e, 1)
        .emplace_r<dummy1>(1)
        .emplace_r<dummy2>(1);

    r.emplace<dummy1>(e2, 2).emplace_r<dummy2>(3);

    r.addSystem<dummy1>("test").setSystemUpdate(
        [](const Entity& e, registry& r) {
            auto& v = r.get<dummy1>(e);
            v.x++;
        });
    r.update()
        .update()
        .update()
        .update();

    View<dummy0, dummy1> v(r);

    v.each([](dummy0& v, const dummy1& s) {
        v.x++;
    });

    v.eachEntity([](const Entity& e, const dummy0& v, dummy1& s) {
        s.x++;
    });

    EXPECT_EQ(r.get<dummy0>(e).x, 2);
    EXPECT_EQ(r.get<dummy1>(e).x, 6);
    EXPECT_EQ(r.get<dummy2>(e).x, 1);
    EXPECT_EQ(r.get<dummy1>(e2).x, 6);
    EXPECT_EQ(r.get<dummy2>(e2).x, 3);
}

static void random_entities_components_test_speed(const unsigned int entityCount, const unsigned time_limit_ms)
{
    double start = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::high_resolution_clock::now().time_since_epoch()).count();
    registry r;
    double elapsed;

    std::srand(std::time(nullptr));

    for (unsigned int i = 0; i < entityCount; i++) {
        Entity e = r.newEntity();
        r.emplace<dummy0>(e, 1)
            .emplace_r<dummy1>(1)
            .emplace_r<dummy2>(1);
        if (std::rand() % 2 == 0) {
            r.emplace<dummy1>(e, 2).emplace_r<dummy2>(3);
            r.removeEntity(Entity(std::rand() % r.entitiesCount()));
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

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}