module;

#include <stddef.h>

export module nacl:utils;

import :meta;

export inline void* operator new(size_t, void* p) noexcept { return p; }
export inline void operator delete(void*, void*) noexcept {}

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

    template<typename T, typename U = T>
    constexpr T Exchange(T& obj, U&& new_value) {
        T old_value = AsRvalue(obj);
        obj = Preserve<U>(new_value);
        return old_value;
    }

    template<typename T, typename... Args>
    constexpr T* ConstructAt(T* ptr, Args&&... args) {
        return ::new (ptr) T(Preserve<Args>(args)...);
    }
    template<typename T, typename F, typename... Args>
    constexpr T* ConstructAtWith(T* ptr, F&& fn, Args&&... args) {
        return ::new (ptr) T(Preserve<F>(fn)(Preserve<Args>(args)...));
    }
    template<typename T, typename D>
    constexpr T* ConstructAtFrom(T* ptr, D&& deferred) {
        return ::new (ptr) T(Preserve<D>(deferred).make());
    }
    template<typename T>
    constexpr void DestructAt(T* ptr) {
        ptr->~T();
    }
} // export namespace nacl
