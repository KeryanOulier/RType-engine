#include <tuple>
#include <algorithm>

template<class ...Containers> class zipper;

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

    friend zipper<Containers...>;
    zipper_iterator(iterator_tuple const &it_tuple, size_t max) : _current(it_tuple), _idx(0), _max(max) {
        if(_max && !all_set(_seq)) {
            incr_all(_seq);
        }
    }
public:
    zipper_iterator(zipper_iterator const &z) = default;
    zipper_iterator &operator++() {
        incr_all(_seq);
        return *this;
    }
    zipper_iterator operator++(int) {
        zipper_iterator tmp(*this);
        operator++();
        return tmp;
    }
    value_type operator*() {
        return to_value(_seq);
    }
    // value_type operator->(); // TODO
    friend bool operator==(zipper_iterator const &lhs, zipper_iterator const &rhs) {
        return lhs._current == rhs._current;
    }
    friend bool operator!=(zipper_iterator const &lhs, zipper_iterator const &rhs) {
        return !(lhs == rhs);
    }

private:
    template<size_t... Is>
    void incr_all(std::index_sequence<Is...> seq) {
        if (_idx >= _max)
            return;
        _idx++;
        (std::get<Is>(_current)++, ...);

        while(_idx < _max && !all_set(seq)) {
            _idx++;
            (std::get<Is>(_current)++, ...);
        }
    }
    public:
    template<size_t... Is>
    bool all_set(std::index_sequence<Is...> seq) {
        return ((std::get<Is>(_current))->has_value() && ...);
    }
    template<size_t... Is>
    value_type to_value(std::index_sequence<Is...> seq) {
        return std::tie(_idx, (std::get<Is>(_current))->value()...);
    };
private:
    iterator_tuple _current;
    size_t _max;
    size_t _idx;
public:
    static constexpr std::index_sequence_for<Containers...> _seq{};
};