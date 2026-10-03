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

        template<concepts::Maker HeadArgs, concepts::Maker... TailArgs>
        [[nodiscard]] static Tuple NewFrom(
            HeadArgs&& head_args,
            TailArgs&&... tail_args
        ) {
            return Tuple(
                tags::InPlace{},
                Preserve<HeadArgs>(head_args),
                Preserve<TailArgs>(tail_args)...
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
        : Base(As<Base&&>(other)),
          m_Head(Preserve<Head>(other.m_Head))
        {}
        Tuple& operator=(Tuple&& other) noexcept {
            if (this != &other) {
                Base::operator=(As<Base&&>(other));
                m_Head = Preserve<Head>(other.m_Head);
            }
            return *this;
        }

        [[nodiscard]] static constexpr usize Length(void) { return sizeof...(Tail) + 1; }

        [[nodiscard]] constexpr Base& tail(void) { return As<Base&>(*this); }
        [[nodiscard]] constexpr const Base& tail(void) const { return As<const Base&>(*this); }

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
        : Base(Forward<Tail>(tail)...),
          m_Head(Forward<Head>(h))
        {}

        template<concepts::Maker HeadArgs, concepts::Maker... TailArgs>
        Tuple(tags::InPlace, HeadArgs&& head_args, TailArgs&&... tail_args)
        : Base(tags::InPlace{}, Preserve<TailArgs>(tail_args)...),
          m_Head(Preserve<HeadArgs>(head_args).make())
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
    struct tuple_element<I, const T> : tuple_element<I, T> {};

    template<size_t I, typename... Signature>
    [[nodiscard]] constexpr auto& get(nacl::Tuple<Signature...>& tuple) {
        return nacl::Get<I>(tuple);
    }
    template<size_t I, typename... Signature>
    [[nodiscard]] constexpr const auto& get(const nacl::Tuple<Signature...>& tuple) {
        return nacl::Get<I>(tuple);
    }
    template<size_t I, typename... Signature>
    [[nodiscard]] constexpr auto&& get(nacl::Tuple<Signature...>&& tuple) {
        return nacl::Get<I>(nacl::AsRvalue(tuple));
    }
} // export namespace std
