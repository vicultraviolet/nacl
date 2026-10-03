export module nacl:sequence;

import :types;

export namespace nacl {
    template<typename T, T... Vals>
    struct ValueSequence {
        using ValueType = T;
        [[nodiscard]] static constexpr usize Length(void) { return sizeof...(Vals); }
    };

    template<usize... Is>
    using IndexSequence = ValueSequence<usize, Is...>;

    namespace detail {
        template<typename T, T I, T N, T... Vals>
        struct make_value_sequence {
            using type = typename make_value_sequence<T, I + 1, N, Vals..., I>::type;
        };

        template<typename T, T N, T... Vals>
        struct make_value_sequence<T, N, N, Vals...> {
            using type = ValueSequence<T, Vals...>;
        };
    } // namespace detail

    template<typename T, T N>
    using MakeValueSequence = typename detail::make_value_sequence<T, 0, N>::type;

    template<usize N>
    using MakeIndexSequence = MakeValueSequence<usize, N>;
} // export namespace nacl
