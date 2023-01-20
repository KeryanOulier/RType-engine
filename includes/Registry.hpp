#include <typeindex>
#include <unordered_map>
#include <any>
#include "Sparse_array.hpp"
class registry {
    public:

        // component managing
        template <class Component>
        sparse_array<Component> &register_component() {
            _components_array[std::type_index(typeid(Component))] = sparse_array<Component>();
            return get_components<Component>();
        }

        template <class Component>
        sparse_array<Component> &get_components() {
            return std::any_cast<sparse_array<Component> &>(_components_array[std::type_index(typeid(Component))]);
        }

        template <class Component>
        sparse_array<Component> const &get_components() const {
            return std::any_cast<sparse_array<Component> const &>(_components_array.at(std::type_index(typeid(Component))));
        }
    private:
        std::unordered_map<std::type_index, std::any> _components_array;

};