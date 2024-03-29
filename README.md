# Silva

Simple dynamic data driven ecs

Starting to become a collection of useful game development tools..

## ECS example:

```cpp
#include "Silva.hpp"
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
    silva::registry r;
    silva::Entity e = r.spawn_entity();
    silva::Entity e2 = r.spawn_entity();

    // To chain emplaces use the _r version for the other calls
    r.emplace<Velocity>(e, 1, 2)
        .emplace_r<Some>(0)
        .emplace_r<Other>(1)
        .emplace_r<Test>(1, 2, 3);

    r.emplace<Velocity>(e2, 1, 2).emplace_r<Some>(0);

    // Add a system and the conresponding update function
    r.add_system(
        [](silva::registry& registry) {
            for (auto& [entity, some] : registry.view<Some>()) {
                std::cout << entity << " " << some.a << std::endl;
                some.a++;
            }
        });
    r.update()
        .update()
        .update()
        .update(); // Call the update of the test System 4 times

    // zipper are slices equivalent
    // They can be used to gather some informations
    // But avoid removing entities from the registry
    // while using them because it will invalidate the view
    silva::zipper<Velocity, Some> v(r);

// Commented code is down deprecated it is considered better to use
// STL std::begin and std::end or foreach than to implement or own each
// It is considered better for portability
// Now zipper is simply an iterator over all the valid types
// It may be improved by implementing cache for the systems using
// directly the zipper but this might break backwards compatibility
// A possible implementation of a system that would not just be a lambda
// would be:
// registry.add_system<TypeA, TypeB, TypeC>([](Entity& e, TypeA& a, TypeB& b, TypeC& c) {} );
// and in c++17 (or 20 idk anymore) template parameter is deduced so:
// registry.add_system([](Entity& e, TypeA& a, TypeB& b, TypeC& c) {} );
// would be enough
/*
    // You can also avoid to specify the Entity parameter
    v.each([](const Velocity& velocity, const Some& some) {
        std::cout << "Each: "
                  << "Entity(Noid)" << some.a << std::endl;
    });

    v.eachEntity([](const silva::Entity& e, const Velocity& velocity, Some& some) {
        std::cout << "Each With Entity: " << e << " " << some.a << std::endl;
        some.a++;
    });
*/
    // You can use ranged for loops
    for (auto& [entity, velocity, some] : v) {
        std::cout << "Ranged: " << e << " " << some.a << std::endl;
        some.a++;
    }

    // And this for
    for (auto& e : v) {
        std::cout << "For: " << silva::get<Some>(e).a << std::endl;
    }
}
```

## SilvaState:

```cpp
#include "SilvaState.hpp"
#include <iostream>

silva::StateMachine *g_sm:

class SampleState : public silva::State {
public:
    void init() override
    {
        std::cout << "Enter" << std::endl;
    }

    void exit() override
    {
        std::cout << "Exit" << std::endl;
    }

    void update() override
    {
        std::cout << "Update" << std::endl;

        if (someEvent) {
            g_sm->changeState<OtherState>(); // placeholder state that can behave like this state
        } else if (someOtherEvent) {
            g_sm->stop(); // stop the state machine
            return;
        }
        // If you called stop() accessing members of the state
        // will result in undefined behavior (probably a crash)
    }

// In case the draw needs to be separated from the update
#ifdef SILVA_DRAW
    void draw() override
    {
        std::cout << "Draw" << std::endl;
    }
#endif

// In case the event management needs to be separated from the update
#ifdef SILVA_HANDLE_EVENT
    void handleEvent() override
    {
        std::cout << "Handle Event" << std::endl;
    }
#endif

    SampleState()
    {
        std::cout << "Constructor" << std::endl;
    }

    ~SampleState()
    {
        std::cout << "Destructor" << std::endl;
    }
};

int main()
{
    // Create a state machine (Note that the state machine must always have a base state)
    g_sm = new silva::StateMachine();

    g_sm->changeState<SampleState>();
    while (g_sm.update()) { // Update the state machine
#ifdef SILVA_DRAW
        g_sm.draw(); // Draw the state machine
#endif
#ifdef SILVA_HANDLE_EVENT
        g_sm.handleEvent(); // Handle the event
#endif
    }
    delete g_sm;
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
