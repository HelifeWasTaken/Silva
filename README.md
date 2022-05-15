# Silva

Simple dynamic data driven ecs

## Example:

Considering thoses structs and classes:

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
```
```cpp
int main()
{
    silva::registry r;
    silva::Entity e = r.newEntity();
    silva::Entity e2 = r.newEntity();

    // To chain emplaces use the _r version for the other calls
    r.emplace<Velocity>(e, 1, 2)
        .emplace_r<Some>(1)
        .emplace_r<Other>((Other) { 1 })
        .emplace_r<Test>(1, 2, 3);

    r.emplace<Velocity>(e2, 1, 2)
        .emplace_r<Some>(3);

    // Add a system and the conresponding update function
    r.addSystem<Some>("test").setSystemUpdate(
        [](const silva::Entity& e, silva::registry& r) {
            auto& v = r.get<Some>(e);
            std::cout << "Entity: " << e << std::endl;
            std::cout << v.a << std::endl;
            v.a++;
        });
    r.update().update().update().update(); // Call the update of the test System 4 times

    // Views are slices equivalent that are readonly!
    // They can be used to gather some informations
    silva::View<Velocity, Some> v(r);


    // You can also avoid to specify the Entity parameter
    v.each(
        [](const Velocity& v, const Some& s) {
            std::cout << "Each: " << v.x << " " << v.y << std::endl;
        }
    );

    v.eachEntity(
        [](const silva::Entity& e, const Velocity& v, const Some& s) {
            std::cout << "Each With Entity: " << v.x << " " << v.y << std::endl;
        }
    );

    // You can use ranged for loops
    for (const auto& [e, v, s] : v) {
        std::cout << "Ranged: " << v.x << " " << v.y << std::endl;
    }
}
```