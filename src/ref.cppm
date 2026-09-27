export module nacl:ref;

import :types;
import :concepts;
import :panic;
import :utils;

export namespace nacl {
    template<typename T>
    class Ref {
    public:
        using ValueType = T;
    public:
        [[nodiscard]] static constexpr Ref New(T& referenced) {
            return Ref(&referenced);
        }

        [[nodiscard]] static constexpr Ref Of(T* data) {
            Assert(data, "Failed to create Ref from ptr: Null ptr!");
            return Ref(data);
        }

        ~Ref(void) { m_Data = nullptr; }

        Ref(const Ref& other) : m_Data(other.m_Data) {}
        Ref& operator=(const Ref& other) {
            if (this == &other)
                return *this;

            m_Data = other.m_Data;

            return *this;
        }

        Ref(Ref&& other) noexcept
        : m_Data(Exchange(other.m_Data, nullptr))
        {}
        Ref& operator=(Ref&& other) noexcept {
            if (this == &other)
                return *this;

            m_Data = Exchange(other.m_Data, nullptr);

            return *this;
        }

        [[nodiscard]] constexpr T* ptr(void) const { return m_Data; }
		[[nodiscard]] constexpr T& operator*(void) const {
    		Assert(m_Data, "Failed to dereference Ref: Null ptr!");
    		return *m_Data;
		}
		[[nodiscard]] constexpr T* operator->(void) const {
    		Assert(m_Data, "Failed to dereference Ref: Null ptr!");
    		return m_Data;
		}

		[[nodiscard]] constexpr bool operator==(Ref other) const {
    		return m_Data == other.m_Data;
		}
		[[nodiscard]] constexpr bool operator!=(Ref other) const { return m_Data != other.m_Data; }

		template<typename U> requires meta::IsConvertible<T*, U*>
		[[nodiscard]] operator Ref<U>(void) const {
    		return Ref<U>::_Of(As<U*>(m_Data));
		}

		template<typename U>
		[[nodiscard]] explicit operator Ref<U>(void) const {
    		return Ref<U>::_Of(Transmute<U*>(m_Data));
		}

        [[nodiscard]] constexpr static Ref _Of(T* data) { return Ref(data); }

        [[nodiscard]] constexpr static Ref _None(void) { return Ref(nullptr); }
        [[nodiscard]] constexpr bool _is_some(void) const { return m_Data; }
    private:
        explicit Ref(T* data) : m_Data(data) {}
    private:
        T* m_Data;
    };

    template<concepts::Void T>
    class Ref<T> {
    public:
        using ValueType = T;
    public:
        [[nodiscard]] static constexpr Ref Of(T* data) {
            Assert(data, "Failed to create Ref from ptr: Null ptr!");
            return Ref(data);
        }

        ~Ref(void) { m_Data = nullptr; }

        Ref(const Ref& other) : m_Data(other.m_Data) {}
        Ref& operator=(const Ref& other) {
            if (this == &other)
                return *this;

            m_Data = other.m_Data;

            return *this;
        }

        Ref(Ref&& other) noexcept
        : m_Data(Exchange(other.m_Data, nullptr))
        {}
        Ref& operator=(Ref&& other) noexcept {
            if (this == &other)
                return *this;

            m_Data = Exchange(other.m_Data, nullptr);

            return *this;
        }

        [[nodiscard]] constexpr T* ptr(void) const { return m_Data; }

		[[nodiscard]] constexpr bool operator==(Ref other) const {
    		return m_Data == other.m_Data;
		}
		[[nodiscard]] constexpr bool operator!=(Ref other) const { return m_Data != other.m_Data; }

		template<typename U> requires meta::IsConvertible<T*, U*>
		[[nodiscard]] operator Ref<U>(void) const {
    		return Ref<U>::_Of(As<U*>(m_Data));
		}

		template<typename U>
		[[nodiscard]] explicit operator Ref<U>(void) const {
    		return Ref<U>::_Of(Transmute<U*>(m_Data));
		}

        [[nodiscard]] constexpr static Ref _Of(T* data) { return Ref(data); }

        [[nodiscard]] constexpr static Ref _None(void) { return Ref(nullptr); }
        [[nodiscard]] constexpr bool _is_some(void) const { return m_Data; }
    private:
        explicit Ref(T* data) : m_Data(data) {}
    private:
        T* m_Data;
    };

    template<typename T>
    [[nodiscard]] Ref<T> NewRef(T& referenced) { return Ref<T>::New(referenced); }

    template<typename T>
    [[nodiscard]] Ref<T> RefOf(T* ptr) { return Ref<T>::Of(ptr); }
} // export namespace nacl
