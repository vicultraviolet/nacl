export module nacl:utils;

import :meta;

export namespace nacl {
    template<typename T>
    [[nodiscard]] constexpr T&& Preserve(meta::RemoveReference<T>& x) noexcept {
        return static_cast<T&&>(x);
    }
    template<typename T>
    [[nodiscard]] constexpr T&& Preserve(meta::RemoveReference<T>&& t) noexcept {
        static_assert(!meta::IsLvalueReference<T>, "Bad forward!");
        return static_cast<T&&>(t);
    }

    template<typename To, typename From>
    constexpr To As(From&& from) {
        return static_cast<To>(Preserve<From>(from));
    }
    template<typename To, typename From>
    constexpr To Transmute(From&& from) {
        return reinterpret_cast<To>(Preserve<From>(from));
    }

    template<typename T>
    constexpr meta::RemoveReference<T>&& AsRvalue(T&& from) {
        return As<meta::RemoveReference<T>&&>(Preserve<T>(from));
    }
} // export namespace nacl
