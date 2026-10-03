module;

#include "./macros.hpp"

export module nacl:tuple;

import :tags;
import :concepts;
import :utils;

export namespace nacl {
    template<typename... Signature>
    class Tuple;

    template<>
    class Tuple<> {
    public:
        constexpr Tuple(void) = default;
        constexpr Tuple(tags::InPlace) {}

        [[nodiscard]] static constexpr Tuple New(void) { return Tuple(); }

        [[nodiscard]] static constexpr usize Length(void) { return 0; }

        [[nodiscard]] constexpr bool operator==(const Tuple& other) const { return true; }
        [[nodiscard]] constexpr bool operator!=(const Tuple& other) const { return false; }
    };

    template<typename Head, typename... Tail>
    class Tuple<Head, Tail...> : private Tuple<Tail...> {
    private:
        using Base = Tuple<Tail...>;
    public:
        [[nodiscard]] static Tuple New(Head&& h, Tail&&... tail) {
            return Tuple(
                Preserve<Head>(h),
                Preserve<Tail>(tail)...
            );
        }

        template<concepts::Maker HeadDeferred, concepts::Maker... TailDeferred>
        [[nodiscard]] static Tuple NewFrom(
            HeadDeferred&& head_deferred,
            TailDeferred&&... tail_deferred
        ) {
            return Tuple(
                tags::InPlace{},
                Preserve<HeadDeferred>(head_deferred),
                Preserve<TailDeferred>(tail_deferred)...
            );
        }

        ~Tuple(void) = default;

        Tuple(const Tuple& other)
        : Base(other),
          m_Head(other.m_Head)
        {}
        Tuple& operator=(const Tuple& other) {
            if (this != &other) {
                Base::operator=(other);
                m_Head = other.m_Head;
            }
            return *this;
        }

        Tuple(Tuple&& other) noexcept
        : Base((Base&&)other),
          m_Head(Preserve<Head>(other.m_Head))
        {}
        Tuple& operator=(Tuple&& other) noexcept {
            if (this != &other) {
                Base::operator=((Base&&)other);
                m_Head = Preserve<Head>(other.m_Head);
            }
            return *this;
        }

        [[nodiscard]] static constexpr usize Length(void) { return sizeof...(Tail) + 1; }

        [[nodiscard]] constexpr Base& tail(void) { return (Base&)*this; }
        [[nodiscard]] constexpr const Base& tail(void) const { return (const Base&)*this; }

        [[nodiscard]] constexpr Head& head(void) { return m_Head; }
        [[nodiscard]] constexpr const Head& head(void) const { return m_Head; }

        [[nodiscard]] constexpr bool operator==(const Tuple& other) const {
            return m_Head == other.m_Head && tail() == other.tail();
        }
        [[nodiscard]] constexpr bool operator!=(const Tuple& other) const {
            return !(*this == other);
        }
    protected:
        Tuple(Head&& h, Tail&&... tail)
        : Base(Preserve<Tail>(tail)...),
          m_Head(Preserve<Head>(h))
        {}

        template<concepts::Maker HeadDeferred, concepts::Maker... TailDeferred>
        Tuple(tags::InPlace,
            HeadDeferred&& head_deferred,
            TailDeferred&&... tail_deferred
        )
        : Base(tags::InPlace{}, Preserve<TailDeferred>(tail_deferred)...),
          m_Head(Preserve<HeadDeferred>(head_deferred).make())
        {}
    private:
        Head m_Head;
    };

    template<typename... Signature>
    [[nodiscard]] auto NewTuple(Signature&&... args) {
        return Tuple<Signature...>::New(Preserve<Signature>(args)...);
    }
    template<concepts::Maker... Args>
    [[nodiscard]] auto NewTupleFrom(Args&&... args) {
        return Tuple<typename Args::ResultOf...>::NewFrom(Preserve<Args>(args)...);
    }

    namespace meta {
        template<usize I, typename T>
        struct tuple_element;

        template<typename Head, typename... Tail>
        struct tuple_element<0, Tuple<Head, Tail...>> {
            using type = Head;
        };

        template<usize I, typename Head, typename... Tail>
        struct tuple_element<I, Tuple<Head, Tail...>> {
            using type = typename tuple_element<I - 1, Tuple<Tail...>>::type;
        };

        template<usize I, typename T>
        using TupleElement = typename tuple_element<I, T>::type;

        template<usize I, typename... Signature>
        struct get;

        template<typename Head, typename... Tail>
        struct get<0, Head, Tail...> {
            [[nodiscard]] constexpr static Head& apply(Tuple<Head, Tail...>& t) {
                return t.head();
            }
            [[nodiscard]] constexpr static const Head& apply(const Tuple<Head, Tail...>& t) {
                return t.head();
            }
        };

        template<usize I, typename Head, typename... Tail>
        struct get<I, Head, Tail...> {
            [[nodiscard]] constexpr static auto& apply(Tuple<Head, Tail...>& t) {
                return get<I - 1, Tail...>::apply(t.tail());
            }
            [[nodiscard]] constexpr static const auto& apply(const Tuple<Head, Tail...>& t) {
                return get<I - 1, Tail...>::apply(t.tail());
            }
        };
    } // namespace meta

    template<usize I, typename... Signature>
    [[nodiscard]] constexpr auto& Get(Tuple<Signature...>& t) {
        static_assert(I < sizeof...(Signature));
        return meta::get<I, Signature...>::apply(t);
    }

    template<usize I, typename... Signature>
    [[nodiscard]] constexpr const auto& Get(const Tuple<Signature...>& t) {
        static_assert(I < sizeof...(Signature));
        return meta::get<I, Signature...>::apply(t);
    }

    template<usize I, typename... Signature>
    [[nodiscard]] constexpr auto&& Get(Tuple<Signature...>&& t) {
        return AsRvalue(Get<I>(t));
    }

    template<usize I, typename... Signature>
    [[nodiscard]] constexpr auto& get(Tuple<Signature...>& tuple) {
        return Get<I>(tuple);
    }
    template<usize I, typename... Signature>
    [[nodiscard]] constexpr const auto& get(const Tuple<Signature...>& tuple) {
        return Get<I>(tuple);
    }
    template<usize I, typename... Signature>
    [[nodiscard]] constexpr auto&& get(Tuple<Signature...>&& tuple) {
        return Get<I>(AsRvalue(tuple));
    }
} // export namespace nacl

export namespace std {
    using size_t = decltype(sizeof(0));

    template<typename T>
    struct tuple_size;

    template<typename... Signature>
    struct tuple_size<nacl::Tuple<Signature...>> {
        static constexpr decltype(sizeof(0)) value = nacl::Tuple<Signature...>::Length();
    };

    template<typename T>
    struct tuple_size<const T> : tuple_size<T> {};

    template<size_t I, typename T>
    struct tuple_element;

    template<size_t I, typename... Signature>
    struct tuple_element<I, nacl::Tuple<Signature...>> {
        using type = nacl::meta::TupleElement<I, nacl::Tuple<Signature...>>;
    };

    template<size_t I, typename T>
    struct tuple_element<I, const T> {
        using type = const typename tuple_element<I, T>::type;
    };
} // export namespace std
