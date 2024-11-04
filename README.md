# Silva

Simple dynamic data driven ecs

## Collections of tools

If you are here for [SilvaCollection](https://github.com/HelifeWasTaken/SilvaCollection)

## ECS example:

```cpp
#include "Silva"

struct Vec3 {
    float x, y, z;

    Vec3() : x(0), y(0), z(0) {}
    Vec3(float px, float py, float pz) : x(px), y(py), z(pz) {}

    Vec3(const Vec3& other) = default;
    Vec3(Vec3&& other) = default;
    Vec3 &operator=(Vec3&& other) = default;
    Vec3 &operator=(const Vec3& other) = default;

    friend Vec3 operator+(const Vec3& a, const Vec3& b) { return Vec3{a.x + b.x, a.y + b.y, a.z + b.z}; }
    Vec3& operator+=(const Vec3& b) { return *this = *this + b; }
};

struct Gravity : public Vec3 {
    Gravity() : Vec3(0, -9.8, 0) {}
    Gravity(const Vec3& gravity) : Vec3(gravity) {}
};

struct Position : public Vec3 {
    Position() = default;
    Position(const Vec3& position) : Vec3(position) {}
};

struct RigidBody : public Vec3 {
    float &mass = Vec3::x;
    float &velocity = Vec3::y; 
    float &accecleration = Vec3::z;

    RigidBody(float pmass, float pvelocity, float pacceleration)
        : Vec3(pmass, pvelocity, pacceleration)
    {}

    RigidBody() = default;
};

// This is not an accurate Earth system force application, It is just here for the example

static inline void gravity_system(hl::silva::Registry& registry)
{
    // Only take the entities that have a RigidBody a Position and a Gravity component
    for (auto&& [entity, position, rigidBody, gravity] : registry.view<Position, RigidBody, Gravity>()) {
        position.y += rigidBody.velocity;
        rigidBody.velocity += rigidBody.acceleration;
        position = gravity + position;
    }
}

static inline void gravity_system_method2(hl::silva::Registry& registry)
{
    registry.view<Position, RigidBody, Gravity>.each([](Position& position, RigidBody& body, Gravity& gravity) {
        position.y += rigidBody.velocity;
        rigidBody.velocity += rigidBody.acceleration;
        position = gravity + position;
    });
}

static inline void gravity_system_method3(hl::silva::Registry& registry)
{
    registry.view<Position, RigidBody, Gravity>.each<true>([](const Entity& entity, Position& position, RigidBody& body, Gravity& gravity) {
        position.y += rigidBody.velocity;
        rigidBody.velocity += rigidBody.acceleration;
        position = gravity + position;
    });
}

int main()
{
    hl::silva::Registry registry;

    registry.register_components<Position, RigidBody, Gravity>();

    for (int i = 0; i < 100; i++) {
        hl::silva::Entity e = registry.spawn_entity();
        e.emplace<Position>(rand(), rand(), std::max(rand(), 0))
            .emplace<RigidBody>(5, 0, 0) // You can chain emplaces when using the entity!
            .emplace<Gravity>();
    }

    registry.add_system("Gravity", gravity_system);

    registry.update(); // Call the update of every registered Systems
    return 0;
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
