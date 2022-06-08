# How to use Silva (ECS)

## The registry

The registry is a central place to store all the information about the
components and the connections between them.

It is named `silva::registry`.

### Create your first Entity:

```cpp
silva::registry registry;
silva::Entity entity = registry.newEntity();
```

The use of an entity is to store any types of components.

### What is a component?

A component is a piece of data that is attached to an entity.

Here is how to attach a component:

```cpp
silva::registry registry;
silva::Entity entity = registry.newEntity();
registry.emplace<int>(entity, 42);
```

And here is how to get it

```cpp
int value = registry.get<int>(entity);
```

But what if I want to modify the value ?

```cpp
// Note that the reference to the variable is always returned
int& value = registry.get<int>(entity);
value = 6; // Modified the value in the registry
```

## The coroutines (a.k.a Systems)

A system is a function that is called every 1 / 60 seconds.

And applicates it on every entity that matches his requirements.

```cpp
struct Gravity {
    float x, y, z;
};

struct RigidBody {
    float mass;
    float velocity;
    float acceleration;
};

struct Position {
    int x, y, z;
};

int main()
{
    silva::registry registry;

    for (int i = 0; i < 100; i++) {
        silva::Entity e = registry.newEntity();
        registry.emplace<Position>(e, rand(), rand(), rand());
        registry.emplace<RigidBody>(e, 5, 0, 0);
        registry.emplace<Gravity>(e, 0, -9.8, 0);
    }

    // Only take the entities that have a RigidBody a Position and a Gravity component
    registry.addSystem<Position, RigidBody, Gravity>("EarthSystem");

    registry.setSystemUpdate("EarthSystem",
        [](const silva::Entity& entity, silva::registry& registry)
        {
            auto& position = registry.get<Position>(entity);
            auto& rigidBody = registry.get<RigidBody>(entity);
            auto& gravity = registry.get<Gravity>(entity);

            // This is not an accurate Earth system force application
            // It is just here for the example
            position.y += rigidBody.velocity;
            rigidBody.velocity += rigidBody.acceleration;
            position.x += gravity.x;
            position.y += gravity.y;
            position.z += gravity.z;
        }
    );

    registry.update(); // Call the update of every registered Systems
    return 0;
}
```

### ECS Aliases:

It is annoying to always pass the Entity element when you want to access a component.

So the system store in mind the last used Entity and the last used System to speed the development process.

Here is the last example rewritten with aliases

```cpp
int main()
{
    silva::registry registry;

    for (int i = 0; i < 100; i++) {
        registry.emplace<Position>(registry.newEntity(),rand(), rand(), rand())
            .emplace_r<RigidBody>(5, 0, 0) // Emplace r uses the last used Entity
            .emplace_r<Gravity>(0, -9.8, 0);
    }

    // Only take the entities that have a RigidBody a Position and a Gravity component
    registry.addSystem<Position, RigidBody, Gravity>("EarthSystem")
        .setSystemUpdate([](const silva::Entity& entity, silva::registry& registry)
        {
            auto& position = registry.get<Position>(entity);
            auto& rigidBody = registry.get<RigidBody>();
            auto& gravity = registry.get<Gravity>();

            // This is not an accurate Earth system force application
            // It is just here for the example
            position.y += rigidBody.velocity;
            rigidBody.velocity += rigidBody.acceleration;
            position.x += gravity.x;
            position.y += gravity.y;
            position.z += gravity.z;
        }
    );

    registry.update(); // Call the update of every registered Systems
    return 0;
}
```

## The for ranged loops:

For this example I will use the `silva::View` class to iterate over the entities.

```cpp
int main()
{
    silva::registry registry;

    for (int i = 0; i < 100; i++) {
        registry.emplace<int>(registry.newEntity(), 0);
    }

    // Get the view of the registry
    silva::View view = registry.view<int>();

    // Basic iteration over the entities
   for (auto& entity : view) {
        auto& value = silva::get<int>(entity);
        value++;
        if (value == 42) {
            break; // You can break the loop!
        }
    }

    // Unpack the values of the entities
    for (auto& [entity, value] : view) {
        value++;
        if (value == 42) {
            break; // You can break the loop!
        }
    }

    // Applicate functions like the system
    view.each([](const silva::Entity& entity, int& value) {
        value++;
        if (value == 42) {
            // You cannot break the loop :(
        }
    });
}
```

### The View with multiple components:

```cpp
int main()
{
    silva::registry registry;

    for (int i = 0; i < 100; i++) {
        registry.emplace<int>(registry.newEntity(), 0);
        registry.emplace<float>(registry.newEntity(), 0);
    }

    // Get the view of the registry
    silva::View view = registry.view<int, float>();

    // Basic iteration over the entities
    for (auto& entity : view) {
          auto& value = silva::get<int>(entity);
          auto& value2 = silva::get<float>(entity);
          value++;
          value2++;
          if (value == 42) {
                break; // You can break the loop!
          }
     }

    // Unpack the values of the entities
    for (auto& [entity, value, value2] : view) {
        value++;
        value2++;
        if (value == 42) {
            break; // You can break the loop!
        }
    }

    // Applicate functions like the system
    view.each([](const silva::Entity& entity, int& value, float& value2) {
        value++;
        value2++;
        if (value == 42) {
            // You cannot break the loop :(
        }
    });
}
```

# How to use Silva (StateMachine)

## The StateMachine

A state machine is a class that is used to manage the state of the game.

It is generally used to avoid big conditional branchings

## Your first state

```cpp
class MyState : public State {
public:
    MyState()
    {
        std::cout << "Hello World!" << std::endl;
    }

    ~MyState()
    {
        std::cout << "Goodbye World!" << std::endl;
    }

    void update() override {
        std::cout << "Update" << std::endl;
    }
};

int main()
{
    StateMachine stateMachine;
    stateMachine.changeState<MyState>();
    stateMachine.update();
    stateMachine.update();
}
```

Expected output:
```
Init
Update
Update
Goodbye World
```

## Your first stacked state!

When to use a stacked state?

Imagine your are in game and you want to pause it.

You don't want to destroy the current state.

So you create a new state and push it to the stack.

Here is the example:

```cpp
class GameState : public State {

    void update() override {
        std::cout << "Playing" << std::endl;

        if (KeyboardPressed(KEY_ESCAPE)) {
            stateMachine.pushState<PauseState>();
        }

        player.update();
    }

};

class PauseState : public State {

    void update() override {
        std::cout << "Paused" << std::endl;

        if (KeyboardPressed(KEY_ESCAPE)) {
            stateMachine.popState();
        }

        buttons.update();
    }

};
```

This example should make use alternatively the GameState and PauseState classes.

## Behaviours with StateMachine:

```cpp
class RunBehaviour : public State {
private:
    Player& player;

public:
    RunBehaviour(Player& animable) : player(player)
    {
    }

    void update() override {
        // Specific update code

        if (player.notMoving()) {
            player.changeState<IdleBehaviour>(player);
            return;
        }
    }
};

class IdleBehaviour : public State {
private:
    Player& player;

public:
    IdleBehaviour(Player& player) : player(player)
    {
    }

    void update() override {
        // Specific update code
        if (player.isMoving()) {
            player.changeState<RunBehaviour>(player);
            return;
        }
    }
};

class ActionBehaviour : public State {
private:
    Player& player;

public:
    ActionBehaviour(Player& player) : player(player)
    {
    }

    void update() override {
        // Specific update code

        if (player.finishedAction()) {
            player.changeState<IdleBehaviour>(player);
            return;
        }
    }
};

class Player {
private:
    Animable animable;
    StateMachine stateMachine;
    Vector2 velocity;

public:
    Player() { stateMachine.changeState<IdleBehaviour>(player); }

    void update() { stateMachine.update(); }

    bool isMoving() const { return velocity.x != 0 || velocity.y != 0; }

    bool notMoving() const { return !isMoving(); }

    template<typename T>
    void changeState(Player& player) { stateMachine.changeState<T>(player); }
};
```
