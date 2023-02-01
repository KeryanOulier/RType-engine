/*
** EPITECH PROJECT, 2023
** bs
** File description:
** Sparse_array
*/

#ifndef SPARSE_ARRAY_HPP_
#define SPARSE_ARRAY_HPP_

#include <optional>
#include <utility>
#include <vector>

namespace ecs {
    /**
     * @brief Sparse array class, used to store an array of optional components
     * 
     * @tparam Component 
     */
    template <typename Component> class sparse_array {
    public:
        using value_type = std::optional<Component>;
        using reference_type = value_type &;
        using const_reference_type = value_type const &;
        using container_t = std::vector<value_type>;
        using size_type = typename container_t::size_type;
        using iterator = typename container_t::iterator;
        using const_iterator = typename container_t::const_iterator;

    public:
        /**
         * @brief Construct a new sparse array object
         * 
         */
        sparse_array() = default;
        /**
         * @brief Copy construct a new sparse array object
         * 
         * @param from sparse_array to copy
         */
        sparse_array(sparse_array const &from) = default;
        /**
         * @brief Move construct a new sparse array object
         * 
         * @param from sparse_array to move
         */
        sparse_array(sparse_array &&from) noexcept = default;
        
        /**
         * @brief Destroy the sparse array object
         * 
         */
        ~sparse_array() = default;

        /**
         * @brief Copy assign a new sparse array object
         * 
         * @param from sparse_array to copy
         */
        sparse_array &operator=(sparse_array const &from) = default;
        /**
         * @brief Move assign a new sparse array object
         * 
         * @param from sparse_array to move
         */
        sparse_array &operator=(sparse_array &&from) noexcept = default;

        // vector functions overload
        /**
         * @brief Overload of operator[] to access the sparse_array at a given index. Can throw a std::out_of_range exception.
         * 
         * @param idx to access
         * @return reference_type 
         */
        reference_type operator[](size_t idx)
        {
            if (idx >= _data.size())
                throw std::out_of_range("Index out of range");
            return _data[idx];
        }
        /**
         * @brief Overload of operator[] to access the sparse_array at a given index. Can throw a std::out_of_range exception. (const)
         * 
         * @param idx to access
         * @return const_reference_type 
         */
        const_reference_type operator[](size_t idx) const
        {
            if (idx >= _data.size())
                throw std::out_of_range("Index out of range");
            return _data[idx];
        };
        /**
         * @brief Overlaod of begin() to access the begin of the sparse_array
         * 
         * @return iterator 
         */
        iterator begin()
        {
            return _data.begin();
        };
        /**
         * @brief Overlaod of begin() to access the begin of the sparse_array (const)
         * 
         * @return const_iterator 
         */
        const_iterator begin() const
        {
            return _data.begin();
        };
        /**
         * @brief Overlaod of cbegin() to access the begin of the sparse_array (const)
         * 
         * @return const_iterator 
         */
        const_iterator cbegin() const
        {
            return _data.cbegin();
        };
        /**
         * @brief Overlaod of end() to access the end of the sparse_array
         * 
         * @return iterator 
         */
        iterator end()
        {
            return _data.end();
        };
        /**
         * @brief Overlaod of end() to access the end of the sparse_array (const)
         * 
         * @return const_iterator 
         */
        const_iterator end() const
        {
            return _data.end();
        };
        /**
         * @brief Overlaod of cend() to access the end of the sparse_array (const)
         * 
         * @return const_iterator 
         */
        const_iterator cend() const
        {
            return _data.cend();
        };
        /**
         * @brief Overlaod of size() to access the size of the sparse_array
         * 
         * @return size_type 
         */
        size_type size() const
        {
            return _data.size();
        };
        /**
         * @brief Insert a component at a given position in the sparse_array. If the position is out of range, the sparse_array will be resized.
         * 
         * @param pos to instert to
         * @param component component to insert
         * @return reference_type 
         */
        reference_type insert_at(size_type pos, Component const &component)
        {
            if (pos >= _data.size()) {
                _data.resize(pos + 1);
            }
            _data[pos] = component;
            return _data[pos];
        }
        /**
         * @brief Insert a component at a given position in the sparse_array. If the position is out of range, the sparse_array will be resized. Like the previous one, but move the component instead of copying it.
         * 
         * @param pos to instert to
         * @param component component to insert
         * @return reference_type 
         */
        reference_type insert_at(size_type pos, Component &&component)
        {
            if (pos >= _data.size()) {
                _data.resize(pos + 1);
            }
            _data[pos] = std::move(component);
            return _data[pos];
        }
        /**
         * @brief Remove a component at a given position in the sparse_array. Does not resize the sparse_array, it only replace the component by nothing. If the position is out of range, nothing will happen.
         * 
         * @tparam Params 
         * @param pos of the component to remove
         */
        template <class... Params> void erase(size_type pos)
        {
            if (pos >= _data.size()) {
                return;
            }
            _data[pos].reset();
        }
        /**
         * @brief Get the index of a component in the sparse_array. If the component is not in the sparse_array, -1 will be returned.
         * 
         * @param value component to get the index of
         * @return size_type 
         */
        size_type get_index(value_type const &value) const
        {
            if (!value.has_value())
                return -1;

            auto addrval = std::addressof(value.value());

            for (auto it = _data.begin(); it != _data.end(); ++it) {
                if (it->has_value() && std::addressof(it->value()) == addrval) {
                    return std::distance(_data.begin(), it);
                }
            }
            return -1;
        }

    private:
        container_t _data;
    };
}

#endif /* !SPARSE_ARRAY_HPP_ */
