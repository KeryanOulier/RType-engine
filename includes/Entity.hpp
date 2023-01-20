/*
** EPITECH PROJECT, 2023
** bs
** File description:
** Entity
*/

#ifndef ENTITY_HPP_
#define ENTITY_HPP_

#include <string>

class entity
{
    friend class registry;
    explicit entity(size_t number) : _number(number)
    {
    }

  public:
    operator size_t() const
    {
        return _number;
    }

  private:
    size_t _number;
};

#endif /* !ENTITY_HPP_ */