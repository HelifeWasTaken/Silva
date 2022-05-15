# Silva

Simple dynamic data driven ecs

## Example:

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
    silva::Entity e = r.newEntity();
    silva::Entity e2 = r.newEntity();

    // To chain emplaces use the _r version for the other calls
    r.emplace<Velocity>(e, 1, 2)
        .emplace_r<Some>(0)
        .emplace_r<Other>((Other) { 1 })
        .emplace_r<Test>(1, 2, 3);

    r.emplace<Velocity>(e2, 1, 2).emplace_r<Some>(0);

    // Add a system and the conresponding update function
    r.addSystem<Some>("test").setSystemUpdate(
        [](const silva::Entity& e, silva::registry& r) {
            auto& v = r.get<Some>(e);
            std::cout << e << " " << v.a << std::endl;
            v.a++;
        });
    r.update()
        .update()
        .update()
        .update(); // Call the update of the test System 4 times

    // Views are slices equivalent
    // They can be used to gather some informations
    // But avoid removing entities from the registry
    // while using them because it will invalidate the view
    silva::View<Velocity&, Some&> v(r);

    // You can also avoid to specify the Entity parameter
    v.each([](const Velocity& v, const Some& s) {
        std::cout << "Each: "
                  << "Entity(Noid)" << s.a << std::endl;
    });

    v.eachEntity([](const silva::Entity& e, const Velocity& v, Some& s) {
        std::cout << "Each With Entity: " << e << " " << s.a << std::endl;
        s.a++;
    });

    // You can use ranged for loops
    for (auto& [e, v, s] : v) {
        std::cout << "Ranged: " << e << " " << s.a << std::endl;
        s.a++;
    }

    // And this for
    for (auto& e : v) {
        std::cout << "For: " << std::get<Some&>(e).a << std::endl;
    }
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

