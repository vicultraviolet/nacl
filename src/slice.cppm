export module nacl:slice;

import :meta;
import :utils;
import :ref;
import :array_iterators;

export namespace nacl {
    template<typename T>
    class Slice {
    public:
        using ValueType = T;

        using Iterator = meta::Conditional<
            meta::IsConst<T>,
            ArrayConstIterator<Slice>,
            ArrayIterator<Slice>
        >;
        using ConstIterator = ArrayConstIterator<Slice>;
        using ReverseIterator = meta::Conditional<
            meta::IsConst<T>,
            ArrayConstReverseIterator<Slice>,
            ArrayReverseIterator<Slice>
        >;
        using ConstReverseIterator = ArrayConstReverseIterator<Slice>;
    public:
        [[nodiscard]] static Slice Empty(void) { return Slice(nullptr, 0); }
        [[nodiscard]] static Slice Default(void) { return Slice(nullptr, 0); }

        [[nodiscard]] static Slice New(Ref<T> data, usize length) {
            return Slice(data.ptr(), length);
        }
        [[nodiscard]] static Slice New(T* data, usize length) {
            return Slice(data, length);
        }

        template<typename Container>
        [[nodiscard]] static Slice Of(Container&& container) {
            return Slice(
                Preserve<Container>(container).ptr(),
                Preserve<Container>(container).length()
            );
        }

        ~Slice(void) = default;

        Slice(const Slice& other)
        : m_Data(other.m_Data),
          m_Length(other.m_Length)
        {}
        Slice& operator=(const Slice& other) {
            if (this != &other) {
                m_Data = other.m_Data;
                m_Length = other.m_Length;
            }
            return *this;
        }

        Slice(Slice&& other) noexcept
        : m_Data(Exchange(other.m_Data, nullptr)),
          m_Length(Exchange(other.m_Length, 0))
        {}
        Slice& operator=(Slice&& other) noexcept {
            if (this != &other) {
                m_Data = Exchange(other.m_Data, nullptr);
                m_Length = Exchange(other.m_Length, 0);
            }
            return *this;
        }

        [[nodiscard]] constexpr auto at(usize i) const {
            return Iterator::New(m_Data + i);
        }
        [[nodiscard]] constexpr auto rat(usize i) const {
            return at(m_Length).reverse() + 1 + i;
        }

        [[nodiscard]] constexpr auto begin(void) const { return at(0); }
        [[nodiscard]] constexpr auto end(void) const { return at(m_Length); }

        [[nodiscard]] constexpr auto rbegin(void) const { return rat(0); }
        [[nodiscard]] constexpr auto rend(void) const { return rat(m_Length); }

        [[nodiscard]] constexpr T& operator[](usize i) const { return m_Data[i]; }

        [[nodiscard]] constexpr T* ptr(void) const { return m_Data; }
        [[nodiscard]] constexpr usize length(void) const { return m_Length; }

        [[nodiscard]] constexpr Ref<T> ref(void) const { return RefFromPtr(m_Data); }
        [[nodiscard]] constexpr bool is_empty(void) const { return m_Length == 0; }

        template <typename U> requires meta::IsConvertible<T(*)[], U(*)[]>
		[[nodiscard]] operator Slice<U>(void) const {
            return Slice<U>::New(As<U*>(m_Data), m_Length);
		}

		template<typename U>
		[[nodiscard]] explicit operator Slice<U>(void) const {
		    return Slice<U>::New(Transmute<U*>(m_Data), m_Length);
		}
    private:
        Slice(T* data, usize length)
        : m_Data(data), m_Length(length)
        {}
    private:
        T* m_Data;
        usize m_Length;
    };

    template<typename T>
    [[nodiscard]] Slice<T> NewSlice(Ref<T> data, usize length) {
        return Slice<T>::New(data, length);
    }
    template<typename T>
    [[nodiscard]] Slice<T> NewSlice(T* data, usize length) {
        return Slice<T>::New(data, length);
    }

    template<typename Container>
    [[nodiscard]] auto SliceOf(Container&& container) {
        using ValueType = meta::Conditional<
            meta::IsConst<meta::RemoveReference<Container>>,
            const typename meta::RemoveReference<Container>::ValueType,
            typename meta::RemoveReference<Container>::ValueType
        >;
        return Slice<ValueType>::Of(Preserve<Container>(container));
    }
} // export namespace nacl
