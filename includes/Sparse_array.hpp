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
        sparse_array() = default;
        sparse_array(sparse_array const &) = default;
        sparse_array(sparse_array &&) noexcept = default;

        ~sparse_array() = default;

        sparse_array &operator=(sparse_array const &) = default;
        sparse_array &operator=(sparse_array &&) noexcept = default;

        // vector functions overload
        reference_type operator[](size_t idx)
        {
            return _data[idx];
        }
        const_reference_type operator[](size_t idx) const
        {
            return _data[idx];
        };
        iterator begin()
        {
            return _data.begin();
        };
        const_iterator begin() const
        {
            return _data.begin();
        };
        const_iterator cbegin() const
        {
            return _data.cbegin();
        };
        iterator end()
        {
            return _data.end();
        };
        const_iterator end() const
        {
            return _data.end();
        };
        const_iterator cend() const
        {
            return _data.cend();
        };
        size_type size() const
        {
            return _data.size();
        };

        reference_type insert_at(size_type pos, Component const &component)
        {
            if (pos >= _data.size()) {
                _data.resize(pos + 1);
            }
            _data[pos] = component;
            return _data[pos];
        }
        reference_type insert_at(size_type pos, Component &&component)
        {
            if (pos >= _data.size()) {
                _data.resize(pos + 1);
            }
            _data[pos] = std::move(component);
            return _data[pos];
        }
        template <class... Params> void erase(size_type pos)
        {
            if (pos >= _data.size()) {
                return;
            }
            _data[pos].reset();
        }
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
