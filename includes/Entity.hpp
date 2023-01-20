/*
** EPITECH PROJECT, 2023
** bs
** File description:
** Entity
*/

#ifndef ENTITY_HPP_
#define ENTITY_HPP_

#include <string>

class entity {
public:
    explicit entity(size_t number) : _number(number) {}
    operator size_t() const { return _number; }

private:
    size_t _number;
};

#endif /* !ENTITY_HPP_ */