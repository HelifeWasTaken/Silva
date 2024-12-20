# Silva

Simple dynamic data driven ecs

## Collections of tools

If you are here for [SilvaCollection](https://github.com/HelifeWasTaken/SilvaCollection)

## ECS example:

```cpp
#include "Silva"
#include <iostream> // iostream
#include <ctime> // srand -> time

struct Gravity {
    float x = 0, y = -9.81f, z = 0;
};

struct Position {
    float x = 0, y = 0, z = 0;
};

struct RigidBody {
    float mass = 0, velocity = 0, acceleration = 0;
};

// This is not an accurate Earth system force application, It is just here for the example

static inline void gravity_system(hl::silva::Registry& registry)
{
    // Only take the entities that have a RigidBody a Position and a Gravity component
    for (auto&& [entity, position, body, gravity] : registry.view<Position, RigidBody, Gravity>()) {
        // You can modify the requested entity components for all the entities that have them
        // You can modify the entity here
        // You can break the loop
        // Removing a component from the entity if used by the system may cause a crash

        position.x += gravity.x;
        position.y += gravity.y;
        position.z += gravity.z;

        body.acceleration += body.mass * gravity.x;
        body.acceleration += body.mass * gravity.y;
        body.acceleration += body.mass * gravity.z;

        body.velocity += body.acceleration;
        body.acceleration = 0;

        std::cout << "Entity " << entity.get_id() << " has a position of (" << position.x << ", " << position.y << ", " << position.z << ")" << std::endl;
    }
}

static inline void gravity_system_method2(hl::silva::Registry& registry)
{
    registry.view<Position, RigidBody, Gravity>().each([](Position& position, RigidBody& body, Gravity& gravity) {
            // ...
            // You can modify the requested entity components for all the entities that have them
            // You cannot modify the entity here
            // You cannot break the loop
    });
}

static inline void gravity_system_method3(hl::silva::Registry& registry)
{
    registry.view<Position, RigidBody, Gravity>().each<true>([](hl::silva::Entity& entity, Position& position, RigidBody& body, Gravity& gravity) {
            // ...
            // You can modify the requested entity components for all the entities that have them
            // you can modify the entity here
            // You cannot break the loop
            // Removing a component from the entity if used by the system may cause a crash
    });
}

struct _dummy {_dummy() { std::srand(std::time(nullptr)); }} dummy; // Seed the random number generator at the start of the program
                                                                    // not clean just here for the example

static const auto get_random_float = []() -> float { return (float)rand() / RAND_MAX; };
static const unsigned int entity_max = 100;
static const unsigned int max_iterations = 100;

static const unsigned int entity_count = std::max(1U, (unsigned int)rand() % entity_max);
static const unsigned int iterations = std::max(1U, (unsigned int)rand() % max_iterations);

int main()
{
    hl::silva::Registry registry;

    registry.register_components<Position, RigidBody, Gravity>();

    for (unsigned int i = 0; i < entity_count; i++) {
        hl::silva::Entity e = registry.spawn_entity();

        e.emplace<Position>(get_random_float(), get_random_float(), get_random_float())
            .emplace<RigidBody>(5.f, 0.f, 0.f) // You can chain emplaces when using the entity!
            .emplace<Gravity>();
    }

    registry.add_system("Gravity", gravity_system);

    for (unsigned int i = 0; i < iterations; i++) {
        std::cout << "Iteration " << i << std::endl,
        registry.update(); // Call the update of every registered Systems
    }

    //return 0;
}
```

## SilvaState:

```cpp
#include "SilvaState"
#include <iostream>

class SampleState : public hl::silva::State {
public:
    SampleState()
    {
        std::cout << "Constructor" << std::endl;
    }

    ~SampleState()
    {
        std::cout << "Destructor" << std::endl;
    }

    void init() override
    {
        std::cout << "Enter" << std::endl;
    }

    void update() override
    {
        std::cout << "Update" << std::endl;

        if (someEvent) {
            state_manager().replace<OtherState>(); // placeholder state that can behave like this state
        } else if (someOtherEvent) {
            state_manager().stop(); // stop the state machine
            return;
        }
        // If you called stop() accessing members of the state
        // will result in undefined behavior (probably a crash)
    }

// In case the draw needs to be separated from the update
#ifdef SILVA_STATE_RENDER
    void render() override
    {
        std::cout << "Draw" << std::endl;
    }
#endif

// In case the event management needs to be separated from the update
#ifdef SILVA_STATE_EVENT
    void event() override
    {
        std::cout << "Handle Event" << std::endl;
    }
#endif
};

int main()
{
    // Create a state machine (Note that the state machine must always have a base state)
    hl::silva::StateManager sm;

    sm.push<SampleState>();
    while (sm.update()) { // Update the state machine
#ifdef SILVA_STATE_RENDER
        sm.render(); // Draw the state machine
#endif
#ifdef SILVA_STATE_EVENT
        sm.event(); // Handle the event
#endif
    }
    return 0;
}
```

## LICENSE

```
 README.md
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 SOFTWARE.
```
