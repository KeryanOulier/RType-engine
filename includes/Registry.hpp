#include <any>
#include <functional>
#include <typeindex>
#include <unordered_map>

#include "Entity.hpp"
#include "Sparse_array.hpp"

class registry
{
  public:
    // component managing
    template <class Component> sparse_array<Component> &register_component()
    {
        _components_array[std::type_index(typeid(Component))] = sparse_array<Component>();
        _remove_component_functions.push_back(
            [](registry &reg, entity e) { reg.get_components<Component>().erase(e); });
        return get_components<Component>();
    }

    template <class Component> sparse_array<Component> &get_components()
    {
        return std::any_cast<sparse_array<Component> &>(_components_array[std::type_index(typeid(Component))]);
    }

    template <class Component> sparse_array<Component> const &get_components() const
    {
        return std::any_cast<sparse_array<Component> const &>(_components_array.at(std::type_index(typeid(Component))));
    }

    // entity managing
    entity spawn_entity()
    {
        if (_available_ids.empty())
        {
            return entity(_higgest_entity_id++);
        }
        else
        {
            auto id = _available_ids.back();
            _available_ids.pop_back();
            return entity(id);
        }
    }
    entity entity_from_index(size_t index) const
    {
        return entity(index);
    }
    void kill_entity(entity e)
    {
        for (auto &f : _remove_component_functions)
        {
            f(*this, e);
        }
        _available_ids.push_back(e);
    }
    template <typename Component>
    typename sparse_array<Component>::reference_type add_component(entity const &to, Component &&component)
    {
        return get_components<Component>().insert_at(to, std::forward<Component>(component));
    }
    template <typename Component> void remove_component(entity const &from)
    {
        get_components<Component>().erase(from);
    }

  private:
    std::unordered_map<std::type_index, std::any> _components_array;

    int _higgest_entity_id = 0;
    std::vector<size_t> _available_ids;
    std::vector<std::function<void(registry &, entity)>> _remove_component_functions;
};