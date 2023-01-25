#include "Zipper_iterator.hpp"
#include <algorithm>

namespace ecs {
    template <class ...Containers>
    class zipper {
        public:
            using iterator = zipper_iterator<Containers...>;
            using iterator_tuple = typename iterator::iterator_tuple;

            zipper(Containers &...cs) : _size(_compute_size(cs...)), _begin(std::make_tuple(cs.begin()...)), _end(_compute_end(cs...)) {
            };

            iterator begin() {
                return iterator(_begin, _size);
            }
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
            iterator_tuple _begin;
            iterator_tuple _end;
            size_t _size;
    };
}