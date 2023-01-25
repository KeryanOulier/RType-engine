/*
** EPITECH PROJECT, 2023
** bs
** File description:
** Entity
*/

#ifndef ENTITY_HPP_
#define ENTITY_HPP_

#include <string>

namespace ecs {
    /**
     * @brief Entity class that represent an entity thanks to an id
     * 
     */
    class entity {
        friend class registry;
        /**
         * @brief Construct a new entity object with an id
         * 
         * @param id
         */
        explicit entity(size_t id)
            : _id(id)
        {
        }

    public:
        /**
         * @brief inplicit conversion operator to size_t
         * 
         * @return id of the entity
         */
        operator size_t() const
        {
            return _id;
        }

    private:
        size_t _id; /**< id of the entity */
    };
}

#endif /* !ENTITY_HPP_ */