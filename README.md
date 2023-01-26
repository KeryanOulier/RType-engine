
# RType-engine

This is the engine for the epitech project RType

# Table of Content

- [How it works](#how-it-works)
  - [How to create a register](#how-to-create-a-register)
  - [How to create an entity](#how-to-create-an-entity)
  - [How to add a component to an entity](#how-to-add-a-component-to-an-entity)
  - [How to create a system](#how-to-create-a-system)
  - [Full example](#full-example)

# How it Works

The game-engine is an ECS, meaning it is compose of Entities, Components and Systems. There are also 1 key concepts in the engine: the Registry.

- Entities: They are the object of the game, they are represented by an ID
- Components: They are the data of the Entities
- Systems: They are the logic of the game, they are composed of a set of Components and they are applied to all Entities that have the required Components
- Registry: It is the "database", it contains all the Entities and their Components

## How to create a register

To create a registry you just need to create a object of type registry

```cpp
ecs::registry reg;
```

## How to create an entity

To create an entity you need to use the Registry.

```cpp
ecs::entity e = reg.spawn_entity();
```

## How to add a component to an entity

First you need to create a component. A component is only a struct/class that contains data.

```cpp
// exemple: this is a component that contain a position
struct position {
    int x;
    int y;
};
```

Then you need to register the component to the registry. You only need to register the component once.

```cpp
reg.register_component<position>();
```

Now you can add the component to an entity.

```cpp
reg.add_component<component_type>(entity_id, component_value);
```

## How to create a system

Next, a system is a function this is applied to all entities that have the required components.

```cpp
// exemple: this is a system that output the position and the velocity of all entities that have them.
void logger_system(ecs::registry &reg, ecs::sparse_array<position> positions, ecs::sparse_array<velocity> velocities) {
    for (auto [id, pos, vel] : ecs::zipper(positions, velocities)) {
        std::cout << "Entity " << id << " has position " << pos.x << " " << pos.y << " and velocity " << vel.x << " " << vel.y << std::endl;
    }
}
```

Here we use the zipper class to iterate over all the entities that have the required components. The zipper class is a helper class that allow you to iterate over multiple sparse_array at the same time.
Also, the "sparse_array" parameters are obtained by calling:

```cpp
sparse_array<component_type> component = reg.get_components<component_type>();
```

## Full example

```cpp
#include <iostream>
#include "Registry.hpp"
#include "Zipper.hpp"

// component position to store the position of an entity
struct position {
    int x;
    int y;
};

// component velocity to store the velocity of an entity
struct velocity {
    int x;
    int y;
};

// system that output the position and the velocity of all entities that have them.
void logger_system(ecs::registry &reg, ecs::sparse_array<position> positions, ecs::sparse_array<velocity> velocities) {

    // iterate over all entities that have the required components thanks to the zipper class
    for (auto [id, pos, vel] : ecs::zipper(positions, velocities)) {
        std::cout << "Entity " << id << " has position " << pos.x << " " << pos.y << " and velocity " << vel.x << " " << vel.y << std::endl;
    }
}

int main(void) {
    // creation of the registry
    ecs::registry reg;

    // registration of all the components
    reg.register_component<position>();
    reg.register_component<velocity>();

    // creation of the entities
    ecs::entity e1 = reg.spawn_entity();
    ecs::entity e2 = reg.spawn_entity();

    // add the components to the entity 1
    reg.add_component<position>(e1, {0, 0});
    reg.add_component<velocity>(e1, {1, 0});

    // add the components to the entity 2
    reg.add_component<position>(e2, {3, 2});
    reg.add_component<velocity>(e2, {0, -1});

    logger_system(reg, reg.get_components<position>(), reg.get_components<velocity>());
}
```

In this exemple, the sytem is called only once, but in a game context, the system should be called every frame in the gameloop.
