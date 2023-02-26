/*
** EPITECH PROJECT, 2023
** engine
** File description:
** Zipper_iterator
*/

#ifndef ZIPPER_ITERATOR_HPP_
#define ZIPPER_ITERATOR_HPP_

#include <tuple>
#include <algorithm>

namespace ecs {
    template<class ...Containers> class zipper;

    /**
     * @brief Iterator class for the zipper class
     * 
     * @tparam Containers 
     */
    template<class ...Containers>
    class zipper_iterator {
        template <class Container>
        using iterator_t = decltype(std::declval<Container>().begin());
        template <class Container>
        using it_reference_t = typename iterator_t<Container>::reference;
    public:
        using value_type = std::tuple<std::size_t, decltype(std::declval<it_reference_t<Containers>>().value()) &...>;
        using reference = value_type;
        using pointer = void;
        using difference_type = size_t;
        using iterator_category = std::forward_iterator_tag;
        using iterator_tuple = std::tuple<iterator_t<Containers>...>;

        friend class zipper<Containers...>;
    private:
        /**
         * @brief Construct a new zipper iterator object
         * 
         * @param it_tuple tuple of iterators of the containers
         * @param max index of the last element
         */
        zipper_iterator(iterator_tuple const &it_tuple, size_t max) : _current(it_tuple), _max(max), _idx(0) {
            if(_max && !all_set(_seq)) {
                incr_all(_seq);
            }
        }
    public:
        /**
         * @brief Copy construct a new zipper iterator object
         * 
         * @param z object to copy
         */
        zipper_iterator(zipper_iterator const &z) = default;
        /**
         * @brief Overload of the ++ operator. Increment the iterator, then return it
         * 
         * @return zipper_iterator& 
         */
        zipper_iterator &operator++() {
            incr_all(_seq);
            return *this;
        }
        /**
         * @brief Overload of the ++ operator. Return the iterator, then increment it
         * 
         * @return zipper_iterator 
         */
        zipper_iterator operator++(int) {
            zipper_iterator tmp(*this);
            operator++();
            return tmp;
        }
        /**
         * @brief dereference the iterator
         * 
         * @return value_type 
         */
        value_type operator*() {
            return to_value(_seq);
        }
        /**
         * @brief Overload of the == operator. Compare two iterators
         * 
         * @param lhs first iterator
         * @param rhs second iterator
         * @return true if the iterators are equal
         * @return false otherwise
         */
        friend bool operator==(zipper_iterator const &lhs, zipper_iterator const &rhs) {
            return lhs._current == rhs._current;
        }
        /**
         * @brief Overload of the != operator. Compare two iterators
         * 
         * @param lhs first iterator
         * @param rhs second iterator
         * @return true if the iterators are not equal
         * @return false otherwise
         */
        friend bool operator!=(zipper_iterator const &lhs, zipper_iterator const &rhs) {
            return !(lhs == rhs);
        }

    private:
        template<size_t... Is>
        void incr_all(std::index_sequence<Is...> seq) {
            (void)seq;
            if (_idx >= _max)
                return;
            _idx++;
            (std::get<Is>(_current)++, ...);

            while(_idx < _max && !all_set(seq)) {
                _idx++;
                (std::get<Is>(_current)++, ...);
            }
        }
        template<size_t... Is>
        bool all_set(std::index_sequence<Is...> seq) {
            (void)seq;
            return ((std::get<Is>(_current))->has_value() && ...);
        }
        template<size_t... Is>
        value_type to_value(std::index_sequence<Is...> seq) {
            (void)seq;
            return std::tie(_idx, (std::get<Is>(_current))->value()...);
        };
    private:
        iterator_tuple _current;
        size_t _max;
        size_t _idx;
        static constexpr std::index_sequence_for<Containers...> _seq{};
    };
}

#endif /* !ZIPPER_ITERATOR_HPP_ */
