# How to use Silva (ECS)

## The registry

The registry is a central place to store all the information about the
components and the connections between them.

It is named `silva::registry`.

### Create your first Entity:

```cpp
hl::silva::registry registry;
hl::silva::Entity entity = registry.spawn_entity();
```

The use of an entity is to store any types of components.

### What is a component?

A component is a piece of data that is attached to an entity.

Here is how to attach a component:

```cpp
hl::silva::registry registry;
registry.register_components<int>();
hl::silva::Entity entity = registry.spawn_entity();
registry.emplace_component<int>(entity, 42);
// or
entity.emplace<int>(42);
```

And here is how to get it

```cpp
int value = registry.get_component<int>(entity);
// or
int value = entity.get<int>();
```

But what if I want to modify the value ?

```cpp
// Note that the reference to the variable is always returned
int& value = registry.get_component<int>(entity);
// or
int& value = entity.get<int>();

value = 6; // Modified the value in the registry
```

## The for ranged loops:

For this example I will use the `silva::View` class to iterate over the entities.

```cpp
int main()
{
    hl::silva::registry registry;

    registry.register_component<int>();

    for (int i = 0; i < 100; i++) {
        registry.emplace_component<int>(registry.spawn_entity(), 0); // Entity is Implicitly casted to the Id
    }

    // Get the view of the registry
    hl::silva::view view = registry.view<int>();

    // Unpack the values of the entities
    for (auto& [entity, value] : view) {
        value++;
        if (value == 42) {
            break; // You can break the loop!
        }
    }
}
```

### The View with multiple components:

```cpp
int main()
{
    hl::silva::registry registry;

    registry.register_component<int, float>();

    for (int i = 0; i < 100; i++) {
        registry.emplace<int>(registry.spawn_entity(), 0);
        registry.emplace<float>(registry.spawn_entity(), 0);
    }

    // Get the view of the registry
    silva::view view = registry.view<int, float>();

    // Unpack the values of the entities
    for (auto& [entity, value, value2] : view) {
        value++;
        value2++;
        if (value == 42) {
            break; // You can break the loop!
        }
    }
}
```

## The coroutines (a.k.a Systems)

A system is a function that is called every update call

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

```cpp
int main()
{
    hl::silva::registry registry;

    registry.register_components<Position, RigidBody, Gravity>();

    for (int i = 0; i < 100; i++) {
        hl::silva::Entity e = registry.spawn_entity();
        e.emplace<Position>(rand(), rand(), rand())
            .emplace<RigidBody>(5, 0, 0) // You can chain emplaces when using the entity!
            .emplace<Gravity>(0, -9.8, 0);
    }

    // Only take the entities that have a RigidBody a Position and a Gravity component
    registry.add_system([](hl::silva::registry& registry)
        {
            for (auto&& [entity, position, rigidBody, gravity] : registry.view<Position, RigidBody, Gravity>()) {
                // This is not an accurate Earth system force application
                // It is just here for the example
                position.y += rigidBody.velocity;
                rigidBody.velocity += rigidBody.acceleration;
                position.x += gravity.x;
                position.y += gravity.y;
                position.z += gravity.z;
            }
        }
    );

    registry.update(); // Call the update of every registered Systems
    return 0;
}
```

You can also constrain a system to specific requirements using `add_csystem` (a.k.a `constrained_system`)
It can be used like so:
```cpp
// Each call will be a currently valid Entity of the constrains given
// Removing a component that is used by the constraint to the registry may break the process
// Here each time the function is called we are ensured that the entity has a Gravity and RigidBody
// We pass down the registry but should not be required with the function provided inside the Entity class
registry.add_csystem<Gravity, RigidBody>([](hl::silva::registry& registry, hl::silva::Entity& entity) {
    entity.get<Gravity>().x /* for example and we are assured that the component exist */ ;
});
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
            state_manager().push<PauseState>(); // is directly push
            // may return afterwards if necessary in case you do not want to continue directly after
            // a push
            // return;
        }

        player.update();
    }

};

class PauseState : public State {

    void update() override {
        std::cout << "Paused" << std::endl;

        if (KeyboardPressed(KEY_ESCAPE)) {
            state_manager.pop(); // Will be actually popped after the function end
            return; // to avoid updating further and having race conditions with your own program
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
	    player.animable.loadAnimation("Run");// or whatever how it should work
    }

    void update() override {
        // Specific update code

        if (!player.isMoving()) {
            player.changeState<IdleBehaviour>();
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
	    player.animable.loadAnimation("Idle"); // or whatever how it should work
    }

    void update() override {
        // Specific update code
        if (player.isMoving()) {
            player.changeState<RunBehaviour>();
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
	    player.animable.loadAnimation("Action");// or whatever how it should work
    }

    void finishedAction() {
	bool hasFinished = false;
	// *code* 
        // or maybe inside the player class implement the logic of this
	return hasFinished;
    }

    void update() override {
        // Specific update code

        if (finishedAction()) {
            player.changeState<IdleBehaviour>();
            return;
        }
    }
};

struct Player {
    Vector2 velocity;
    Animable animable;
    StateMachine stateMachine;

    Player() { changeState<IdleBehaviour>(); }

    void update() { animable.update(); stateMachine.update(); }

    bool isMoving() const { return velocity.x || velocity.y ; }

    // simple alias instead of player.stateMachine.replace<T>(player);
    template<typename T>
    void changeState() { stateMachine.replace<T>(*this); }
};
```
