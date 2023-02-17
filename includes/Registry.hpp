#include <any>
#include <functional>
#include <typeindex>
#include <unordered_map>
#include <dlfcn.h>
#include <filesystem>
#include <iostream>

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

        /**
         * @brief Check if an entity has a component
         *
         * @tparam Component to check
         * @param e entity to check
         * @return true or false
         */
        template <typename Component> bool has_component(entity const &e) const
        {
            try {
                get_components<Component>()[e].has_value();
            } catch (std::exception) {
                return false;
            }
        }
        
        // SYSTEMS
        /**
         * @brief add a system to the registry
         * 
         * @tparam Components 
         * @tparam Function 
         * @param f 
         */
        template <class... Components, typename Function>
        void add_system(Function &&f) {
            _systems.push_back(
                [&f](registry &reg) { f(reg, reg.get_components<Components>()...); }
            );
        }

        /**
         * @brief run all the systems
         * 
         */
        void run_systems() {
            for (auto &f : _systems) {
                f(*this);
            }
        }
        // MODULE/lib
        using entrypoint_fcn = void (*)(ecs::registry &);

        /**
            @brief: Load a library and execute the entrypoint function
            @param lib_name: The name of the library to load
            @param reg: The registry to pass to the entrypoint function
            @param function_name: The name of the entrypoint function, default is "entrypoint"
        */
        void lib_entrypoint(const std::string &lib_name, const std::string &function_name="entrypoint")
        {
            auto handle = load_lib(lib_name);
            if (!handle) {
                return;
            }
            try {
                auto entrypoint = get_function<entrypoint_fcn>(function_name, handle);
                entrypoint(*this);
            } catch (const std::exception &e) {
                std::cerr << "Error while executing lib_entrypoint: " << e.what() << std::endl;
                close_lib(handle);
            }
        }
    private:
        void *load_lib(const std::string &lib_path) {
            if (!std::filesystem::exists(lib_path)) {
                std::cerr << "Cannot find library: " << lib_path << std::endl;
                return nullptr;
            }

            auto handle = dlopen(lib_path.c_str(), RTLD_LAZY);

            if (!handle) {
                std::cerr << "Cannot open library: " << dlerror() << std::endl;
                return nullptr;
            }
            return handle;
        }

        void close_lib(void *handle)
        {
            if (handle)
                dlclose(handle);
        }

        template <typename T>
        T get_function(const std::string &function_name, void *handle)
        {
            T function = (T) dlsym(handle, function_name.c_str());
            const char *dlsym_error = dlerror();

            if (dlsym_error) {
                std::cerr << "Cannot load symbol '" << function_name << "': " << dlsym_error << std::endl;
                throw std::runtime_error("Cannot load symbol");
            }
            return function;
        }

    private:
        std::unordered_map<std::type_index, std::any> _components_array;

        int _higgest_entity_id = 0;
        std::vector<size_t> _available_ids;
        std::vector<std::function<void(registry &, entity const &)>>
            _remove_component_functions;
        std::vector<std::function<void(registry &)>> _systems;
    };
}