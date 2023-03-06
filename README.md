
# RType-engine

This is the engine for the epitech project RType

# Table of Content

- [How it works](#how-it-works)
  - [Registry](#registry)
    - [Creation](#registry-creation)
  - [Entity](#entity)
    - [Creation](#entity-creation)
  - [Component](#component)
    - [Registration](#component-registration)
    - [Addition](#component-addition)
    - [SerializedObject](#component-from-serialized-object)
  - [System](#system)
    - [Creation](#system-creation)
    - [Addition](#system-addition)
    - [Run](#system-run)
  - [Event](#event)
    - [Registration](#event-registration)
    - [Trigger](#event-trigger)
  - [Full example](#full-example)
  - [Modules](#modules)
    - [Creation](#module-creation)
    - [Addition](#module-addition)

# How it Works

The game-engine is an ECS, meaning it is compose of Entities, Components and Systems. There are also 1 key concepts in the engine: the Registry.

- Entities: They are the object of the game, they are represented by an ID
- Components: They are the data of the Entities
- Systems: They are the logic of the game, they are composed of a set of Components and they are applied to all Entities that have the required Components
- Registry: It is the "database", it contains all the Entities and their Components

## Registry

### Registry creation

To create a registry you just need to create a object of type registry

```cpp
ecs::registry reg;
```

## Entity

### Entity creation

To create an entity you need to use the Registry.

```cpp
ecs::entity e = reg.spawn_entity();
```

or you can generate an entity with a specific ID

```cpp
ecs::entity e = reg.entity_from_index(42);
```

## Component

First you need to create a component. A component is only a struct/class that contains data.

```cpp
// exemple: this is a component that contain a position
struct position {
    int x;
    int y;
};
```

### Component registration

You need to register the component to the registry. You only need to register the component once.

```cpp
reg.register_component<position>();
```

### Component addition

Add the component to an entity with:

```cpp
reg.add_component<component_type>(entity_id, component_value);
```

### Component from serialized object

You can also add components to an entity with a serialized object (like json or yaml). To do that you need to register the component another way.

```cpp
reg.register_component<component_type, SerializedObject>("name_of_the_component", function_to_deserialize_the_object);
```

You can add multiple SerializedObject, but you need to specify the function to deserialize the object for each.

Then you can add the component to an entity with a serialized object.

```cpp
reg.add_component<SerializedObject>("name_of_the_component", entity_id, object_to_deserialize);
```

## System

A system is a function this is applied to all entities that have the required components.

### System creation

```cpp
// exemple: this is a system that output the position and the velocity of all entities that have them.
void logger_system(ecs::registry &reg,
std::vector<ecs::entity> entities,
ecs::sparse_array<position> positions, ecs::sparse_array<velocity> velocities) {
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

### System addition

Add the system to the registry with:

```cpp
reg.add_system<component_type1, component_type2, ...>(system_function, 0);
```

The "0" is the priority of the system, the lower the priority, the sooner the system will be called (default 0).

### System run

Run all the systems with:

```cpp
reg.run_systems(entities);
```

## Event

### Event registration

To register an event you need to create a function that will be called when the event is triggered.

```cpp
void event_function(ecs::registry &reg, std::vector<ecs::entity> entities, int value) {
    // do something with the value
}
```

Then you need to register the event with:

```cpp
reg.add_event<int>("event_name", event_function);
```

The templates of this function are all the parameter that the event can take in addition to the registry and the entity vector.

### Event trigger

To trigger an event you need to call:

```cpp
reg.trigger_event<int>("event_name", value);
```

The templates of this function are all the parameter that the event can take in addition to the registry and the entity vector.

If the template parameter in the add_event and the trigger_event are different, the behavior is undefined.

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
void logger_system(ecs::registry &reg,
std::vector<ecs::entity> entities,
ecs::sparse_array<position> positions,
ecs::sparse_array<velocity> velocities) {

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

    reg.add_system<position, velocity>(logger_system);

    std::vector<ecs::entity> entities;
    entities.push_back(e1);
    entities.push_back(e2);

    reg.run_systems(entities);
}
```

In this exemple, the sytem is called only once, but in a game context, the run_system function should be called every frame in the gameloop.

## Modules

Modules are used to add a feature to the game that can be added or removed.

### Module creation

To create a module you need to create a dynamic library that contains an entrypoint function. It will be called when the module is loaded and need to setup all the components, systems and events needed for the feature added by the modul.

```cpp
extern "C" {
    void entrypoint(ecs::registry &reg) {
        // register all the components, systems and events
    }
}
```

### Module loading

To load a module you need to call:

```cpp
reg.load_module("relative/path/to/the/module");
```
