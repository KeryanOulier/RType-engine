# RType-engine
This is the engine for the eîtech project RType

# Table of Content

# How to install

# How it Works

The game-engine is an ECS, meaning it is compose of Entities, Components and Systems. There are also 1 key concepts in the engine: the Registry.

- Entities: They are the object of the game, they are represented by an ID
- Components: They are the data of the Entities
- Systems: They are the logic of the game, they are composed of a set of Components and they are applied to all Entities that have the required Components
- Registry: It is the "database", it contains all the Entities and their Components

# How to create an entity

To create an entity you need to use the Registry.
    
```cpp
registry.spawn_entity();
```

# How to add a component to an entity

First you need to create a component. A component is only a struct/class that contains data.
```cpp
# exemple: this is a component that contain a position
struct position {
    int x;
    int y;
};
```
Then you need to register the component to the registry.
```cpp
registry.register_component<position>();
```
Now you can add the component to an entity.
```cpp
registry.add_component<position>(entity_id, {0, 0});
```

# How to create a system
W.I.P