export module nacl:deferred;

import :meta;
import :utils;
import :sequence;
import :tuple;

export namespace nacl {
    namespace detail {
        template<typename T, typename Tuple, usize... I>
        [[nodiscard]] constexpr T NewFromTuple(Tuple&& t, IndexSequence<I...>) {
            return T(Get<I>(Preserve<Tuple>(t))...);
        }

        template<typename T, typename F, typename Tuple, usize... I>
        [[nodiscard]] constexpr T CallWithTuple(F&& fn, Tuple&& t, IndexSequence<I...>) {
            return Preserve<F>(fn)(Get<I>(Preserve<Tuple>(t))...);
        }
    } // namespace detail

    template<typename T, typename _Tuple>
    [[nodiscard]] constexpr T NewFromTuple(_Tuple&& tuple) {
        return detail::NewFromTuple<T>(
            Preserve<_Tuple>(tuple),
            MakeIndexSequence<_Tuple::Length()>{}
        );
    }

    template<typename F, typename Args>
    constexpr auto CallWithTuple(F&& fn, Tuple<Args...>& tuple) {
        using T = meta::ResultOf<F, Args...>;
        return detail::CallWithTupleImpl<T, F>(
            Preserve<F>(fn),
            tuple,
            MakeIndexSequence<sizeof...(Args)>{}
        );
    }
    template<typename F, typename... Args>
    constexpr auto CallWithTuple(F&& fn, const Tuple<Args...>& tuple) {
        using T = meta::ResultOf<F, Args...>;
        return detail::CallWithTupleImpl<T, F>(
            Preserve<F>(fn),
            tuple,
            MakeIndexSequence<sizeof...(Args)>{}
        );
    }
    template<typename F, typename... Args>
    constexpr auto CallWithTuple(F&& fn, Tuple<Args...>&& tuple) {
        using T = meta::ResultOf<F, Args...>;
        return detail::CallWithTupleImpl<T, F>(
            Preserve<F>(fn),
            AsRvalue(tuple),
            MakeIndexSequence<sizeof...(Args)>{}
        );
    }

    template<typename T, typename... Args>
    class DeferredConstruct {
    public:
        using ResultOf = T;
    public:
        Tuple<Args...> tuple;
    public:
        [[nodiscard]] static DeferredConstruct New(Args&&... args) {
            return DeferredConstruct {
                Tuple<Args...>::New(
                    Preserve<Args>(args)...
                );
            };
        }

        template<typename F>
        constexpr T with(F&& fn) const & {
            return CallWithTuple(Preserve<F>(fn), tuple);
        }
        template<typename F>
        constexpr T with(F&& fn) & {
            return CallWithTuple(Preserve<F>(fn), tuple);
        }
        template<typename F>
        constexpr T with(F&& fn) && {
            return CallWithTuple(Preserve<F>(fn), AsRvalue(tuple));
        }

        [[nodiscard]] constexpr T make(void) const & { return NewFromTuple<T>(tuple); }
        [[nodiscard]] constexpr T make(void) & { return NewFromTuple<T>(tuple); }
        [[nodiscard]] constexpr T make(void) && { return NewFromTuple<T>(Move(tuple)); }
    };

    template<typename T, typename... Args>
    [[nodiscard]] auto NewDeferredConstruct(Args&&... args) {
        return DeferredConstruct<T, Args...>::New(Preserve<Args>(args)...);
    }

    template<typename F, typename... Args>
    class DeferredCall {
    public:
        using ResultOf = meta::ResultOf<F, Args...>;
        using Function = F;
    public:
        Tuple<F, Args...> tuple;
    public:
        [[nodiscard]] static DeferredCall New(F&& fn, Args&&... args) {
            return DeferredCall {
                Tuple<F, Args...>::New(
                    Preserve<F>(fn),
                    Preserve<Args>(args)...
                );
            };
        }

        constexpr ResultOf call(void) const & {
            return CallWithTuple(tuple.head(), tuple.tail());
        }
        constexpr ResultOf call(void) & {
            return CallWithTuple(tuple.head(), tuple.tail());
        }
        constexpr ResultOf call(void) && {
            return CallWithTuple(AsRvalue(tuple.head()), AsRvalue(tuple.tail()));
        }

        [[nodiscard]] constexpr ResultOf make(void) const & { return call(); }
        [[nodiscard]] constexpr ResultOf make(void) & { return call(); }
        [[nodiscard]] constexpr ResultOf make(void) && { return call(); }
    };

    template<typename F, typename... Args>
    [[nodiscard]] auto NewDeferredCall(F&& fn, Args&&... args) {
        return DeferredCall<F, Args...>::New(
            Preserve<F>(fn),
            Preserve<Args>(args)...
        );
    }
} // export namespace nacl
