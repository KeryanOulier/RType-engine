#include <any>
#include <functional>
#include <typeindex>
#include <unordered_map>

#include "Entity.hpp"
#include "Sparse_array.hpp"

namespace ecs {
    /**
     * @brief Registry class that manages entities and components
     * 
     */
    class registry {
    public:
        // component managing
        /**
         * @brief Register a component type to the registry
         * 
         * @tparam Component to register
         * @return sparse_array of the registered component
         */
        template <class Component> sparse_array<Component> &register_component()
        {
            _components_array[std::type_index(typeid(Component))] =
                sparse_array<Component>();
            _remove_component_functions.push_back([](registry &reg, entity e) {
                reg.get_components<Component>().erase(e);
            });
            return get_components<Component>();
        }
        /**
         * @brief Get the sparse_array of a component
         * 
         * @tparam Component to get
         * @return sparse_array of the component
         */
        template <class Component> sparse_array<Component> &get_components()
        {
            return std::any_cast<sparse_array<Component> &>(
                _components_array[std::type_index(typeid(Component))]);
        }
        /**
         * @brief Get the sparse_array of a component (const)
         * 
         * @tparam Component to get
         * @return sparse_array of the component
         */
        template <class Component>
        sparse_array<Component> const &get_components() const
        {
            return std::any_cast<sparse_array<Component> const &>(
                _components_array.at(std::type_index(typeid(Component))));
        }

        // entity managing
        /**
         * @brief Create an entity
         * 
         * @return entity created
         */
        entity spawn_entity()
        {
            if (_available_ids.empty()) {
                return entity(_higgest_entity_id++);
            } else {
                auto id = _available_ids.back();
                _available_ids.pop_back();
                return entity(id);
            }
        }
        /**
         * @brief Get an entity from an index
         * 
         * @param index of the entity
         * @return entity 
         */
        entity entity_from_index(size_t index) const
        {
            return entity(index);
        }
        /**
         * @brief Kill an entity
         * 
         * @param e entity to kill
         */
        void kill_entity(entity e)
        {
            for (auto &f : _remove_component_functions) {
                f(*this, e);
            }
            _available_ids.push_back(e);
        }
        /**
         * @brief add a component to an entity
         * 
         * @tparam Component type to add
         * @param to entity to receive the component
         * @param component to add to the entity
         * @return sparse_array of the component
         */
        template <typename Component>
        typename sparse_array<Component>::reference_type add_component(
            entity const &to, Component &&component)
        {
            return get_components<Component>().insert_at(to,
                std::forward<Component>(component));
        }
        /**
         * @brief remove a component from an entity
         * 
         * @tparam Component type to remove
         * @param from entity to remove the component from
         */
        template <typename Component> void remove_component(entity const &from)
        {
            get_components<Component>().erase(from);
        }
        /**
         * @brief Get the max entity count of the registry
         * 
         * @return int max entity count
         */
        int get_max_entity_count() const
        {
            return _higgest_entity_id;
        }

    private:
        std::unordered_map<std::type_index, std::any> _components_array;

        int _higgest_entity_id = 0;
        std::vector<size_t> _available_ids;
        std::vector<std::function<void(registry &, entity const &)>>
            _remove_component_functions;
    };
}