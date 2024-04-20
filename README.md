# Silva

Simple dynamic data driven ecs

Starting to become a collection of useful game development tools..

## ECS example:

```cpp
#include "Silva"
#include <iostream>

struct Velocity {
    int x, y;
};

struct Some {
    int a;
};

struct Other {
    int b;
};

class Test {
public:
    int a, b, c;

    Test(int a, int b, int c)
        : a(a)
        , b(b)
        , c(c)
    {
    }
};

int main()
{
    hl::silva::Registry r;

    // Register components
    r.register_components<Velocity,Some,Other,Test>();

    r.spawn_entity().emplace<Velocity>(1, 2)
        .emplace<Some>(0)
        .emplace<Other>(1).emplace<Test>(1, 2, 3);

    r.spawn_entity().emplace<Velocity>(1, 2).emplace<Some>(0);


    // zipper are slices equivalent, They can be used to gather some informations
    // But avoid removing entities from the registry, while using them because it will invalidate the view
    // Also do not remove a used component in the view while using the zipper
    // You can use ranged for loops
    // (It returns references by default by using auto)
    // (You cannot use auto& because the tuple is a temporary object)
    // You may use auto&& but should not be necessary and adds complexity
    hl::silva::Zipper<Velocity, Some> v(r);
    for (auto [entity, velocity, some] : v) {
        std::cout << "Ranged: " << entity << " " << some.a << std::endl;
        some.a += entity.get_id();
    }

    // if you did not use ranged for (auto [e, v, s] : v) but the normal for (auto var : v)
    // you will have to use var.get<T>() to get the component

    // Add a system and the conresponding update function
    r.add_system(
        [](hl::silva::Registry& registry) {
            for (auto [entity, some] : registry.view<Some>()) {
                std::cout << entity << " " << some.a << std::endl;
                some.a++;
            }
        });

    r.add_csystem<Velocity>(
        [](hl::silva::Registry& r, hl::silva::Entity& entity) {
            Velocity& velocity = entity.get<Velocity>();
            std::cout << entity.get_id() << " " << velocity.x << " " << velocity.y << std::endl;
            velocity.x++;
            velocity.y++;
        });

    r.update();
    r.update();
    r.update();
    r.update(); // Call the update of the test System 4 times for proof of concept
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
