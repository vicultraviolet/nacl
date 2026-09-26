export module nacl:concepts;

import :meta;
import :utils;

export namespace nacl {
    namespace concepts {
        template<typename T, typename U>
        concept Same = meta::IsSame<T, U>;

        template<typename T, typename... P0toN>
        concept OneOf = meta::IsOneOf<T, P0toN...>;

        template<typename T>
        concept Void = meta::IsVoid<T>;

        template<typename T>
        concept LvalueReference = meta::IsLvalueReference<T>;

        template<typename T>
        concept RvalueReference = meta::IsRvalueReference<T>;

        template<typename T>
        concept ConstReference = meta::IsConstReference<T>;

        template<typename T>
        concept Const = meta::IsConst<T>;

        template<typename T>
        concept Reference = meta::IsReference<T>;

        template<typename T>
        concept Ptr = meta::IsPtr<T>;

        template<typename From, typename To>
        concept Convertible = meta::IsConvertible<From, To>;

        template<typename F, typename... Args>
        concept Invocable = requires(F&& fn, Args&&... args) {
            { Preserve<F>(fn)(Preserve<Args>(args)...) };
        };

        template<typename T>
        concept Optional = requires(const T t) {
            { T::_None() } -> Same<T>;
            { t._is_some() } -> Same<bool>;
        };
    } // namespace concepts
} // export namespace nacl
