/*
** EPITECH PROJECT, 2023
** engine
** File description:
** Zipper
*/

#ifndef ZIPPER_HPP_
#define ZIPPER_HPP_

#include "Zipper_iterator.hpp"
#include <algorithm>

namespace ecs {
    /**
     * @brief Zipper helper class to iterate over multiple containers at the same time and only stop when an entity have all the components specified
     * 
     * @tparam Containers 
     */
    template <class ...Containers>
    class zipper {
        public:
            using iterator = zipper_iterator<Containers...>;
            using iterator_tuple = typename iterator::iterator_tuple;

            /**
             * @brief Construct a new zipper object
             * 
             * @param cs all the containers to iterate over
             */
            zipper(Containers &...cs) : _size(_compute_size(cs...)), _begin(std::make_tuple(cs.begin()...)), _end(_compute_end(cs...)) {
            };

            /**
             * @brief Get the begin iterator
             * 
             * @return iterator 
             */
            iterator begin() {
                return iterator(_begin, _size);
            }
            /**
             * @brief Get the end iterator
             * 
             * @return iterator 
             */
            iterator end() {
                return iterator(_end, 0);
            }

        private:
            static size_t _compute_size(Containers &...containers) {
                return std::min({containers.size()...});
            };
            static iterator_tuple _compute_end(Containers &...containers) {
                return std::make_tuple(containers.begin() + _compute_size(containers...) ...);
            }
        private:
            size_t _size;
            iterator_tuple _begin;
            iterator_tuple _end;
    };
}
#endif /* !ZIPPER_HPP_ */
