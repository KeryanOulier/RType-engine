/*
** EPITECH PROJECT, 2023
** rtype-engine
** File description:
** Libs_manager
*/

#ifndef LIBS_MANAGER_HPP_
#define LIBS_MANAGER_HPP_

#include <dlfcn.h>
#include <string>
#include <vector>
#include <iostream>
#include <filesystem>
#include "Registry.hpp"

class libs_manager {
    public:
        libs_manager() = default;
        ~libs_manager() = default;

        using entrypoint_fcn = void (*)(ecs::registry &);

        /**
            @brief: Load a library and execute the entrypoint function
            @param lib_name: The name of the library to load
            @param reg: The registry to pass to the entrypoint function
            @param function_name: The name of the entrypoint function, default is "entrypoint"
        */
        void lib_entrypoint(const std::string &lib_name, ecs::registry &reg, const std::string &function_name="entrypoint")
        {
            auto handle = load_lib(lib_name);
            if (!handle) {
                return;
            }
            try {
                auto entrypoint = get_function<entrypoint_fcn>(function_name, handle);
                entrypoint(reg);
            } catch (const std::exception &e) {
                std::cerr << "Error while executing lib_entrypoint: " << e.what() << std::endl;
                close_lib(handle);
            }
        }

    private:
        void *load_lib(const std::string &lib_path)
        {
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
};

#endif /* !LIBS_MANAGER_HPP_ */
