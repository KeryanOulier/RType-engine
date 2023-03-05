#ifndef REGISTRY_HPP_
#define REGISTRY_HPP_

#include <any>
#include <functional>
#include <typeindex>
#include <unordered_map>
#include <filesystem>
#include <iostream>

#ifdef _WIN32
#define NOMINMAX
#include <windows.h>
#else
#include <dlfcn.h>
#endif

#include "Entity.hpp"
#include "Sparse_array.hpp"

namespace ecs {
    /**
     * @brief Registry class that manages entities and components
     *
     */
    class registry {
        template<class Component, class ObjectType> using serializerFunction = std::function<Component(ObjectType &)>;
        template<class ObjectType> using componentCreator = std::function<void(entity, ObjectType &)>;
        template<class ObjectType> using serializerMap = std::unordered_map<std::string, componentCreator<ObjectType>>;
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

        template <class Component, typename... ObjectType, typename... Function>
        sparse_array<Component> &register_component(const std::string &component_name, Function &&...f)
        {
            _components_array[std::type_index(typeid(Component))] =
                sparse_array<Component>();
            _remove_component_functions.push_back([](registry &reg, entity e) {
                reg.get_components<Component>().erase(e);
            });
            (put_in_map<Component, ObjectType>(component_name, f), ...);
            return get_components<Component>();
        }

        template <class Component, typename ObjectType, typename Function>
        void put_in_map(const std::string &component_name, Function &&f)
        {
            if (_components_from_type.find(std::type_index(typeid(ObjectType))) == _components_from_type.end()) {
                _components_from_type[std::type_index(typeid(ObjectType))] = std::unordered_map<std::string, std::function<void(entity, ObjectType &)>>();
            }
            serializerMap<ObjectType> &map = std::any_cast<serializerMap<ObjectType> &>(_components_from_type[std::type_index(typeid(ObjectType))]);
            if (map.find(component_name) == map.end()) {
                map.insert({component_name, [&](entity e, ObjectType v) {
                    add_component<Component>(e, f(v));
                }});
            }
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

        template <typename ObjectType>
        void add_component(const std::string &component_name, const entity &to, ObjectType &object)
        {
            using unreferencedObjectType = std::remove_reference_t<ObjectType>;
            if (_components_from_type.find(std::type_index(typeid(unreferencedObjectType))) == _components_from_type.end())
                throw std::runtime_error("No component registered for this type : " + std::string(typeid(unreferencedObjectType).name()));
            serializerMap<unreferencedObjectType> &map = std::any_cast<serializerMap<unreferencedObjectType> &>(_components_from_type[std::type_index(typeid(unreferencedObjectType))]);
            if (map.find(component_name) == map.end())
                throw std::runtime_error("No component registered for this type : " + component_name);
            map[component_name](to, object);
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
                if (get_components<Component>().size() <= e) return false;
                return get_components<Component>()[e].has_value();
            } catch (std::exception) {
                return false;
            }
        }

    // SYSTEMS
    private:
        class system {
            public:
                system(std::function<void(registry &, std::vector<entity> &)> &&f, int priority = 0) : _f(f), _priority(priority) {}
                int get_priority() const { return _priority; }
                void operator()(registry &reg, std::vector<entity> &entities) { _f(reg, entities); }
            private:
                std::function<void(registry &, std::vector<entity> &)> _f;
                int _priority;
        };
    public:
        /**
         * @brief add a system to the registry
         *
         * @tparam Components
         * @tparam Function
         * @param f the function to execute
         * @param priority the priority in which the system will be executed
         */
        template <class... Components, typename Function>
        void add_system(Function &&f, int priority=0) {
            _systems.emplace_back(
                [&f](registry &reg, std::vector<entity> &entities) {
                    f(reg, entities, reg.get_components<Components>()...);
                },
                priority
            );
            std::sort(_systems.begin(), _systems.end(), [](const system &a, const system &b) {
                return a.get_priority() < b.get_priority();
            });
        }

        /**
         * @brief run all the systems
         * @param e a vector of entities to pass to the systems, it is useful for systems that need to access other entities, to manage a scene for example
         */
        void run_systems(std::vector<entity> &e)
        {
            for (auto &f : _systems) {
                f(*this, e);
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
        
        /**
            @brief: Load all the libraries in a folder and execute the entrypoint function
            @param folder_path: The path of the folder to load all the libraries in it
        */
       void all_libs_entrypoint(const std::string &folder_path)
       {
            if (!std::filesystem::exists(folder_path)) {
                std::cerr << "Error while executing all_libs_entrypoint: " << folder_path << " does not exist" << std::endl;
                return;
            }

            if (!std::filesystem::is_directory(folder_path)) {
                std::cerr << "Error while executing all_libs_entrypoint: " << folder_path << " is not a directory" << std::endl;
                return;
            }

            for (const auto &entry : std::filesystem::directory_iterator(folder_path)) {
                if (entry.is_directory()) {
                        // all_libs_entrypoint(entry.path().string());
                } else {
#ifdef _WIN32
                    if (entry.path().extension() == ".dll") {
#else
                    if (entry.path().extension() == ".so") {
#endif
                        lib_entrypoint(entry.path().string());
                    }
                }
            }
       }

        bool add_lib(const std::string &lib_name)
        {
            if (is_lib_loaded(lib_name)) return false;
            _loaded_libs.push_back(lib_name);
            return true;
        }
        bool is_lib_loaded(const std::string &lib_name)
        {
            return std::find(_loaded_libs.begin(), _loaded_libs.end(), lib_name) != _loaded_libs.end();
        }
    private:
        void *load_lib(const std::string &lib_path) {
            if (!std::filesystem::exists(lib_path)) {
                std::cerr << "Cannot find library: " << lib_path << std::endl;
                return nullptr;
            }

#ifdef _WIN32
            auto fpath = std::filesystem::absolute(lib_path);
            auto handle = LoadLibrary(fpath.c_str());
#else
            auto handle = dlopen(lib_path.c_str(), RTLD_LAZY);
#endif

            if (!handle) {
                std::string error_message;
#ifdef _WIN32
                DWORD error = GetLastError();
                LPSTR messageBuffer = nullptr;
                FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                    NULL, error, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&messageBuffer, 0, NULL);
                error_message = messageBuffer;
                LocalFree(messageBuffer);
#else
                error_message = dlerror();
#endif
                std::cerr << "Cannot open library: " << error_message << std::endl;
                return nullptr;
            }
            return handle;
        }

        void close_lib(void *handle)
        {
            if (handle) {
#ifdef _WIN32
                FreeLibrary((HMODULE)handle);
#else
                dlclose(handle);
#endif
            }
        }

        template <typename T>
        T get_function(const std::string &function_name, void *handle)
        {
            T function = nullptr;
#ifdef _WIN32
            function = (T)GetProcAddress((HMODULE)handle, function_name.c_str());
#else
            function = (T)dlsym(handle, function_name.c_str());
#endif

            if (!function) {
                std::string error_message;
#ifdef _WIN32
                DWORD error = GetLastError();
                LPSTR messageBuffer = nullptr;
                FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                    NULL, error, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&messageBuffer, 0, NULL);
                error_message = messageBuffer;
                LocalFree(messageBuffer);
#else
                error_message = dlerror();
#endif
                std::cerr << "Cannot load symbol '" << function_name << "': " << error_message << std::endl;
                throw std::runtime_error("Cannot load symbol");
            }
            return function;
        }
    public:
        void set_state(const std::string &state)
        {
            _state = state;
        }
        const std::string &get_state() const
        {
            return _state;
        }

        template<typename... Args, typename Function>
        void add_event(const std::string &event_name, Function &&f)
        {
            _events[event_name].emplace_back(std::function<void(registry &, std::vector<entity> &enitiesn, Args...)>(f));
        }

        template<typename... Args>
        void trigger_event(const std::string &event_name, std::vector<entity> &entities, Args... args)
        {
            for (auto &f : _events[event_name]) {
                std::any_cast<std::function<void(registry &, std::vector<entity> &, Args...)>>(f)(*this, entities, args...);
            }
        }

    private:
        std::unordered_map<std::type_index, std::any> _components_array;
        std::unordered_map<std::string, std::function<void(entity const &, std::any)>> _components_adder;
        int _higgest_entity_id = 0;
        std::vector<size_t> _available_ids;
        std::vector<std::function<void(registry &, entity const &)>>
            _remove_component_functions;
        std::vector<system> _systems;
        std::unordered_map<std::type_index, std::any> _components_from_type;
        std::string _state;
        std::vector<std::string> _loaded_libs;
        std::unordered_map<std::string, std::vector<std::any>> _events;
    };
}

#endif /* !REGISTRY_HPP_ */
