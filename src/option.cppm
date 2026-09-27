export module nacl:option;

import :types;
import :unit;
import :meta;
import :concepts;
import :utils;
import :panic;
import :ref;

export namespace nacl {
    template<concepts::Optional T>
    class NonIntrusiveOption;

    template<typename T>
    class IntrusiveOption;

    namespace meta {
        template<typename T>
        struct option : meta::type_is<IntrusiveOption<T>> {};

        template<concepts::Optional T>
        struct option<T> : meta::type_is<NonIntrusiveOption<T>> {};

        template<typename T>
        using Option = typename option<T>::type;
    } // namespace meta

    template<typename T>
    using Option = meta::Option<T>;

    [[nodiscard]] constexpr Unit None(void) { return Unit{}; }

    template<concepts::Optional T>
    class NonIntrusiveOption {
    public:
        using ValueType = T;

        using AsRef = Option<Ref<T>>;
        using AsConstRef = Option<Ref<const T>>;
    public:
        template<typename... Args>
        [[nodiscard]] static NonIntrusiveOption Some(Args&&... args) {
            NonIntrusiveOption opt;
            ConstructAt(opt.ptr(), Preserve<Args>(args)...);
            return opt;
        }

        template<typename F, typename... Args>
        [[nodiscard]] static NonIntrusiveOption SomeWith(F&& fn, Args&&... args) {
            NonIntrusiveOption opt;
            ConstructAtWith(opt.ptr(), Preserve<F>(fn), Preserve<Args>(args)...);
            return opt;
        }

        [[nodiscard]] static NonIntrusiveOption None(void) {
            return NonIntrusiveOption(Unit{});
        }

        ~NonIntrusiveOption(void) {
            DestructAt(ptr());
        }
        void discard(void) {
            if (is_some())
                *ptr() = T::_None();
        }

        NonIntrusiveOption(const NonIntrusiveOption& other) {
            ConstructAt(ptr(), *other.ptr());
        }
        NonIntrusiveOption& operator=(const NonIntrusiveOption& other) {
            if (this != &other)
                *ptr() = *other.ptr();

            return *this;
        }

        NonIntrusiveOption(NonIntrusiveOption&& other) noexcept {
            ConstructAt(ptr(), AsRvalue(*other.ptr()));
        }
        NonIntrusiveOption& operator=(NonIntrusiveOption&& other) noexcept {
            if (this != &other)
                *ptr() = AsRvalue(*other.ptr());

            return *this;
        }

        NonIntrusiveOption(Unit) {
            ConstructAtWith(ptr(), T::_None);
        }
        NonIntrusiveOption& operator=(Unit) {
            discard();
            return *this;
        }

        [[nodiscard]] T unwrap(void) {
            Assert(is_some(), "Could not unwrap Option: no value!");
            return AsRvalue(*ptr());
        }
        [[nodiscard]] T unwrap_or(T x) {
            if (is_some())
                return AsRvalue(*ptr());
            else
                return x;
        }
        template<typename F, typename... Args>
        [[nodiscard]] T unwrap_or_with(F&& fn, Args&&... args) {
            if (is_some())
                return AsRvalue(*ptr());
            else
                return Preserve<F>(fn)(Preserve<Args>(args)...);
        }

        template<typename F>
        bool map(F&& fn) {
            if (is_none())
                return false;

            Preserve<F>(fn)(*ptr());
            return true;
        }
        template<typename F>
        bool map(F&& fn) const {
            if (is_none())
                return false;

            Preserve<F>(fn)(*ptr());
            return true;
        }

        [[nodiscard]] AsRef as_ref(void) {
            if (is_none())
                return AsRef::None();
            else
                return AsRef::SomeWith(Ref<T>::New, *ptr());
        }
        [[nodiscard]] AsConstRef as_ref(void) const {
            if (is_none())
                return AsConstRef::None();
            else
                return AsConstRef::SomeWith(Ref<const T>::New, *ptr());
        }

        [[nodiscard]] constexpr T* ptr(void) { return Transmute<T*>(m_Data); }
        [[nodiscard]] constexpr const T* ptr(void) const { return Transmute<const T*>(m_Data); }

        [[nodiscard]] constexpr bool is_some(void) const { return ptr()->_is_some(); }
        [[nodiscard]] constexpr bool is_none(void) const { return !ptr()->_is_some(); }
    private:
        NonIntrusiveOption(void) {}
    private:
        alignas(T) ubyte m_Data[sizeof(T)];
    };

    template<typename T>
    class IntrusiveOption {
    public:
        using ValueType = T;

        using AsRef = Option<Ref<T>>;
        using AsConstRef = Option<Ref<const T>>;
    public:
        template<typename... Args>
        [[nodiscard]] static IntrusiveOption Some(Args&&... args) {
            IntrusiveOption opt(true);
            ConstructAt(opt.ptr(), Preserve<Args>(args)...);
            return opt;
        }

        template<typename F, typename... Args>
        [[nodiscard]] static IntrusiveOption SomeWith(F&& fn, Args&&... args) {
            IntrusiveOption opt(true);
            ConstructAtWith(opt.ptr(), Preserve<F>(fn), Preserve<Args>(args)...);
            return opt;
        }

        [[nodiscard]] static IntrusiveOption None(void) {
            return IntrusiveOption(Unit{});
        }

        ~IntrusiveOption(void) { discard(); }
        void discard(void) {
            if (m_IsSome)
                DestructAt(ptr());

            m_IsSome = false;
        }

        IntrusiveOption(const IntrusiveOption& other) {
            if (other.m_IsSome) {
                ConstructAt(ptr(), *other.ptr());
                m_IsSome = true;
            }
        }
        IntrusiveOption& operator=(const IntrusiveOption& other) {
            if (this == &other)
                return *this;

            if (other.m_IsSome) {
                if (m_IsSome) {
                    *ptr() = *other.ptr();
                } else {
                    ConstructAt(ptr(), *other.ptr());
                    m_IsSome = true;
                }
            } else {
                discard();
            }

            return *this;
        }

        IntrusiveOption(IntrusiveOption&& other) noexcept {
            if (other.m_IsSome) {
                ConstructAt(ptr(), AsRvalue(*other.ptr()));
                m_IsSome = true;

                other.discard();
            }
        }
        IntrusiveOption& operator=(const IntrusiveOption&& other) noexcept {
            if (this == &other)
                return *this;

            if (other.m_IsSome) {
                if (m_IsSome) {
                    *ptr() = AsRvalue(*other.ptr());

                    other.discard();
                } else {
                    ConstructAt(ptr(), AsRvalue(*other.ptr()));
                    m_IsSome = true;

                    other.discard();
                }
            } else {
                discard();
            }

            return *this;
        }

        IntrusiveOption(Unit) {}
        IntrusiveOption& operator=(Unit) {
            discard();
            return *this;
        }

        [[nodiscard]] T unwrap(void) {
            Assert(m_IsSome, "Could not unwrap Option: no value!");

            T unwrapped = AsRvalue(*ptr());
            discard();
            return unwrapped;
        }
        [[nodiscard]] T unwrap_or(T x) {
            if (!m_IsSome) {
                return x;
            } else {
                T unwrapped = AsRvalue(*ptr());
                discard();
                return unwrapped;
            }
        }
        template<typename F, typename... Args>
        [[nodiscard]] T unwrap_or_with(F&& fn, Args&&... args) {
            if (!m_IsSome) {
                return Preserve<F>(fn)(Preserve<Args>(args)...);
            } else {
                T unwrapped = AsRvalue(*ptr());
                discard();
                return unwrapped;
            }
        }

        template<typename F>
        bool map(F&& fn) {
            if (!m_IsSome)
                return false;

            Preserve<F>(fn)(*ptr());
            return true;
        }
        template<typename F>
        bool map(F&& fn) const {
            if (!m_IsSome)
                return false;

            Preserve<F>(fn)(*ptr());
            return true;
        }

        [[nodiscard]] AsRef as_ref(void) {
            if (!m_IsSome)
                return AsRef::None();
            else
                return AsRef::SomeWith(Ref<T>::New, *ptr());
        }
        [[nodiscard]] AsConstRef as_ref(void) const {
            if (!m_IsSome)
                return AsConstRef::None();
            else
                return AsConstRef::SomeWith(Ref<const T>::New, *ptr());
        }

        [[nodiscard]] constexpr T* ptr(void) { return Transmute<T*>(m_Data); }
        [[nodiscard]] constexpr const T* ptr(void) const { return Transmute<const T*>(m_Data); }

        [[nodiscard]] constexpr bool is_some(void) const { return m_IsSome; }
        [[nodiscard]] constexpr bool is_none(void) const { return !m_IsSome; }
    private:
        IntrusiveOption(bool is_some = false)
        : m_IsSome(is_some)
        {}
    private:
        alignas(T) ubyte m_Data[sizeof(T)];
        bool m_IsSome = false;
    };

    template<typename T>
    [[nodiscard]] constexpr auto Some(T&& x) {
        using NoRef = meta::RemoveReference<T>;
        return Option<NoRef>::Some(Preserve<T>(x));
    }

    template<typename T, typename... Args>
    [[nodiscard]] constexpr Option<T> MakeSome(Args&&... args) {
        return Option<T>::Some(Preserve<Args>(args)...);
    }

    template<typename F, typename... Args>
    [[nodiscard]] constexpr auto SomeWith(F&& fn, Args&&... args) {
        using T = meta::ResultOf<F, Args...>;
        return Option<T>::SomeWith(
            Preserve<F>(fn),
            Preserve<Args>(args)...
        );
    }

    template<typename T>
    [[nodiscard]] constexpr Option<T> None(void) { return Option<T>::None(); }

    template<typename T>
    [[nodiscard]] Option<Ref<T>> OptionRefOf(T* ptr) {
        if (ptr)
            return SomeWith(Ref<T>::_Of, ptr);
        else
            return None();
    }
} // export namespace nacl
