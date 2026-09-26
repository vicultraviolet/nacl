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

        [[nodiscard]] static constexpr Ref FromPtr(T* ptr) {
            Assert(ptr, "Failed to create Ref from ptr: Null ptr!");
            return Ref(ptr);
        }

        ~Ref(void) { m_Ptr = nullptr; }

        Ref(const Ref& other) : m_Ptr(other.m_Ptr) {}
        Ref& operator=(const Ref& other) {
            if (this == &other)
                return *this;

            m_Ptr = other.m_Ptr;

            return *this;
        }

        Ref(Ref&& other) noexcept
        : m_Ptr(Exchange(other.m_Ptr, nullptr))
        {}
        Ref& operator=(Ref&& other) noexcept {
            if (this == &other)
                return *this;

            m_Ptr = Exchange(other.m_Ptr, nullptr);

            return *this;
        }

        [[nodiscard]] constexpr T* ptr(void) const { return m_Ptr; }
		[[nodiscard]] constexpr T& operator*(void) const {
    		Assert(m_Ptr, "Failed to dereference Ref: Null ptr!");
    		return *m_Ptr;
		}
		[[nodiscard]] constexpr T* operator->(void) const {
    		Assert(m_Ptr, "Failed to dereference Ref: Null ptr!");
    		return m_Ptr;
		}

		[[nodiscard]] constexpr bool operator==(Ref other) const { return m_Ptr == other.m_Ptr; }
		[[nodiscard]] constexpr bool operator!=(Ref other) const { return m_Ptr != other.m_Ptr; }

		template<typename U> requires meta::IsConvertible<T*, U*>
		[[nodiscard]] operator Ref<U>(void) const {
    		return Ref<U>::_FromPtr(As<U*>(m_Ptr));
		}

		template<typename U>
		[[nodiscard]] explicit operator Ref<U>(void) const {
    		return Ref<U>::_FromPtr(Transmute<U*>(m_Ptr));
		}

        [[nodiscard]] constexpr static Ref _FromPtr(T* ptr) { return Ref(ptr); }

        [[nodiscard]] constexpr static Ref _None(void) { return Ref(nullptr); }
        [[nodiscard]] constexpr bool _is_some(void) const { return m_Ptr; }
    private:
        explicit Ref(T* ptr) : m_Ptr(ptr) {}
    private:
        T* m_Ptr;
    };

    template<concepts::Void T>
    class Ref<T> {
    public:
        using ValueType = T;
    public:
        [[nodiscard]] static constexpr Ref FromPtr(T* ptr) {
            Assert(ptr, "Failed to create Ref from ptr: Null ptr!");
            return Ref(ptr);
        }

        ~Ref(void) { m_Ptr = nullptr; }

        Ref(const Ref& other) : m_Ptr(other.m_Ptr) {}
        Ref& operator=(const Ref& other) {
            if (this == &other)
                return *this;

            m_Ptr = other.m_Ptr;

            return *this;
        }

        Ref(Ref&& other) noexcept
        : m_Ptr(Exchange(other.m_Ptr, nullptr))
        {}
        Ref& operator=(Ref&& other) noexcept {
            if (this == &other)
                return *this;

            m_Ptr = Exchange(other.m_Ptr, nullptr);

            return *this;
        }

        [[nodiscard]] constexpr T* ptr(void) const { return m_Ptr; }

    	[[nodiscard]] constexpr bool operator==(Ref other) const { return m_Ptr == other.m_Ptr; }
    	[[nodiscard]] constexpr bool operator!=(Ref other) const { return m_Ptr != other.m_Ptr; }

    	template<typename U> requires meta::IsConvertible<T*, U*>
    	[[nodiscard]] operator Ref<U>(void) const {
      		return Ref<U>::_FromPtr(As<U*>(m_Ptr));
    	}

    	template<typename U>
    	[[nodiscard]] explicit operator Ref<U>(void) const {
      		return Ref<U>::_FromPtr(Transmute<U*>(m_Ptr));
    	}

        [[nodiscard]] constexpr static Ref _FromPtr(T* ptr) { return Ref(ptr); }

        [[nodiscard]] constexpr static Ref _None(void) { return Ref(nullptr); }
        [[nodiscard]] constexpr bool _is_some(void) const { return m_Ptr; }
    private:
        explicit Ref(T* ptr) : m_Ptr(ptr) {}
    private:
        T* m_Ptr;
    };

    template<typename T>
    [[nodiscard]] Ref<T> NewRef(T& referenced) { return Ref<T>::New(referenced); }

    template<typename T>
    [[nodiscard]] Ref<T> RefFromPtr(T* ptr) { return Ref<T>::FromPtr(ptr); }
} // export namespace nacl
