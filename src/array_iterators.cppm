export module nacl:array_iterators;

import :types;
import :ref;

export namespace nacl {
    template<typename Array>
    class ArrayIterator {
        using This = ArrayIterator;
	public:
        using Reversed = typename Array::ReverseIterator;
        using ValueType = typename Array::ValueType;
        using ContainerType = Array;
	public:
	    [[nodiscard]] static constexpr This New(ValueType* ptr) {
			return This(ptr);
		}

		constexpr auto& operator++(void) { m_Ptr++; return *this; }
		constexpr auto  operator++(int) { return This(m_Ptr++); }

		constexpr auto& operator--(void) { m_Ptr--; return *this; }
		constexpr auto  operator--(int) { return This(m_Ptr--); }

		constexpr auto& operator+=(usize x) { m_Ptr += x; return *this; }
		constexpr auto& operator-=(usize x) { m_Ptr -= x; return *this; }

		[[nodiscard]] constexpr auto operator+(usize x) const { return This(m_Ptr + x); }
		[[nodiscard]] constexpr auto operator-(usize x) const { return This(m_Ptr - x); }

		[[nodiscard]] constexpr isize operator-(This other) const { return m_Ptr - other.m_Ptr; }

		[[nodiscard]] constexpr ValueType* operator->(void) const { return m_Ptr; }
		[[nodiscard]] constexpr ValueType& operator*(void) const { return *m_Ptr; }

		[[nodiscard]] constexpr Ref<ValueType> ref(void) const { return RefOf(m_Ptr); }
		[[nodiscard]] constexpr ValueType* ptr(void) const { return m_Ptr; }

		[[nodiscard]] constexpr Reversed reverse(void) const { return Reversed::New(m_Ptr); }

		[[nodiscard]] constexpr bool operator==(This other) const { return m_Ptr == other.m_Ptr; }
		[[nodiscard]] constexpr bool operator!=(This other) const { return m_Ptr != other.m_Ptr; }

		[[nodiscard]] constexpr bool operator> (This other) const { return m_Ptr >  other.m_Ptr; }
		[[nodiscard]] constexpr bool operator< (This other) const { return m_Ptr <  other.m_Ptr; }
		[[nodiscard]] constexpr bool operator>=(This other) const { return m_Ptr >= other.m_Ptr; }
		[[nodiscard]] constexpr bool operator<=(This other) const { return m_Ptr <= other.m_Ptr; }

		[[nodiscard]] static auto _None(void) { return This(nullptr); }
		[[nodiscard]] constexpr bool _is_some(void) const { return m_Ptr; }
	private:
    	explicit constexpr ArrayIterator(ValueType* ptr) : m_Ptr(ptr) {}
	private:
	    ValueType* m_Ptr;
    };

    template<typename Array>
    class ArrayConstIterator {
        using This = ArrayConstIterator;
	public:
        using Reversed = typename Array::ConstReverseIterator;
        using ValueType = const typename Array::ValueType;
        using ContainerType = Array;
	public:
        [[nodiscard]] static constexpr This New(ValueType* ptr) {
            return This(ptr);
        }

		constexpr auto& operator++(void) { m_Ptr++; return *this; }
		constexpr auto  operator++(int) { return This(m_Ptr++); }

		constexpr auto& operator--(void) { m_Ptr--; return *this; }
		constexpr auto  operator--(int) { return This(m_Ptr--); }

		constexpr auto& operator+=(usize x) { m_Ptr += x; return *this; }
		constexpr auto& operator-=(usize x) { m_Ptr -= x; return *this; }

		[[nodiscard]] constexpr auto operator+(usize x) const { return This(m_Ptr + x); }
		[[nodiscard]] constexpr auto operator-(usize x) const { return This(m_Ptr - x); }

		[[nodiscard]] constexpr isize operator-(This other) const { return m_Ptr - other.m_Ptr; }

		[[nodiscard]] constexpr ValueType* operator->(void) const { return m_Ptr; }
		[[nodiscard]] constexpr ValueType& operator*(void) const { return *m_Ptr; }

		[[nodiscard]] constexpr Ref<ValueType> ref(void) const { return RefOf(m_Ptr); }
		[[nodiscard]] constexpr ValueType* ptr(void) const { return m_Ptr; }

		[[nodiscard]] constexpr Reversed reverse(void) const { return Reversed::New(m_Ptr); }

		[[nodiscard]] constexpr bool operator==(This other) const { return m_Ptr == other.m_Ptr; }
		[[nodiscard]] constexpr bool operator!=(This other) const { return m_Ptr != other.m_Ptr; }

		[[nodiscard]] constexpr bool operator> (This other) const { return m_Ptr >  other.m_Ptr; }
		[[nodiscard]] constexpr bool operator< (This other) const { return m_Ptr <  other.m_Ptr; }
		[[nodiscard]] constexpr bool operator>=(This other) const { return m_Ptr >= other.m_Ptr; }
		[[nodiscard]] constexpr bool operator<=(This other) const { return m_Ptr <= other.m_Ptr; }

		[[nodiscard]] static auto _None(void) { return This(nullptr); }
		[[nodiscard]] constexpr bool _is_some(void) const { return m_Ptr; }
	private:
        explicit constexpr ArrayConstIterator(ValueType* ptr) : m_Ptr(ptr) {}
	private:
	    ValueType* m_Ptr;
    };

    template<typename Array>
    class ArrayReverseIterator {
        using This = ArrayReverseIterator;
	public:
        using Reversed = typename Array::Iterator;
        using ValueType = typename Array::ValueType;
        using ContainerType = Array;
	public:
    	[[nodiscard]] static constexpr This New(ValueType* ptr) {
            return This(ptr);
        }

		constexpr auto& operator++(void) { m_Ptr--; return *this; }
		constexpr auto  operator++(int) { return This(m_Ptr--); }

		constexpr auto& operator--(void) { m_Ptr++; return *this; }
		constexpr auto  operator--(int) { return This(m_Ptr++); }

		constexpr auto& operator+=(usize x) { m_Ptr -= x; return *this; }
		constexpr auto& operator-=(usize x) { m_Ptr += x; return *this; }

		[[nodiscard]] constexpr auto operator+(usize x) const { return This(m_Ptr - x); }
		[[nodiscard]] constexpr auto operator-(usize x) const { return This(m_Ptr + x); }

		[[nodiscard]] constexpr isize operator-(This other) const { return other.m_Ptr - m_Ptr; }

		[[nodiscard]] constexpr ValueType* operator->(void) const { return m_Ptr; }
		[[nodiscard]] constexpr ValueType& operator*(void) const { return *m_Ptr; }

		[[nodiscard]] constexpr Ref<ValueType> ref(void) const { return RefOf(m_Ptr); }
		[[nodiscard]] constexpr ValueType* ptr(void) const { return m_Ptr; }

		[[nodiscard]] constexpr Reversed reverse(void) const { return Reversed::New(m_Ptr); }

		[[nodiscard]] constexpr bool operator==(This other) const { return other.m_Ptr == m_Ptr; }
		[[nodiscard]] constexpr bool operator!=(This other) const { return other.m_Ptr != m_Ptr; }

		[[nodiscard]] constexpr bool operator> (This other) const { return other.m_Ptr >  m_Ptr; }
		[[nodiscard]] constexpr bool operator< (This other) const { return other.m_Ptr <  m_Ptr; }
		[[nodiscard]] constexpr bool operator>=(This other) const { return other.m_Ptr >= m_Ptr; }
		[[nodiscard]] constexpr bool operator<=(This other) const { return other.m_Ptr <= m_Ptr; }

		[[nodiscard]] static auto _None(void) { return This(nullptr); }
		[[nodiscard]] constexpr bool _is_some(void) const { return m_Ptr; }
	private:
    	explicit constexpr ArrayReverseIterator(ValueType* ptr) : m_Ptr(ptr) {}
	private:
	    ValueType* m_Ptr;
    };

    template<typename Array>
    class ArrayConstReverseIterator {
        using This = ArrayConstReverseIterator;
	public:
        using Reversed = typename Array::ConstIterator;
        using ValueType = const typename Array::ValueType;
        using ContainerType = Array;
	public:
    	[[nodiscard]] static constexpr This New(ValueType* ptr) {
            return This(ptr);
        }

		constexpr auto& operator++(void) { m_Ptr--; return *this; }
		constexpr auto  operator++(int) { return This(m_Ptr--); }

		constexpr auto& operator--(void) { m_Ptr++; return *this; }
		constexpr auto  operator--(int) { return This(m_Ptr++); }

		constexpr auto& operator+=(usize x) { m_Ptr -= x; return *this; }
		constexpr auto& operator-=(usize x) { m_Ptr += x; return *this; }

		[[nodiscard]] constexpr auto operator+(usize x) const { return This(m_Ptr - x); }
		[[nodiscard]] constexpr auto operator-(usize x) const { return This(m_Ptr + x); }

		[[nodiscard]] constexpr isize operator-(This other) const { return other.m_Ptr - m_Ptr; }

		[[nodiscard]] constexpr ValueType* operator->(void) const { return m_Ptr; }
		[[nodiscard]] constexpr ValueType& operator*(void) const { return *m_Ptr; }

		[[nodiscard]] constexpr Ref<ValueType> ref(void) const { return RefOf(m_Ptr); }
		[[nodiscard]] constexpr ValueType* ptr(void) const { return m_Ptr; }

		[[nodiscard]] constexpr Reversed reverse(void) const { return Reversed::New(m_Ptr); }

		[[nodiscard]] constexpr bool operator==(This other) const { return other.m_Ptr == m_Ptr; }
		[[nodiscard]] constexpr bool operator!=(This other) const { return other.m_Ptr != m_Ptr; }

		[[nodiscard]] constexpr bool operator> (This other) const { return other.m_Ptr >  m_Ptr; }
		[[nodiscard]] constexpr bool operator< (This other) const { return other.m_Ptr <  m_Ptr; }
		[[nodiscard]] constexpr bool operator>=(This other) const { return other.m_Ptr >= m_Ptr; }
		[[nodiscard]] constexpr bool operator<=(This other) const { return other.m_Ptr <= m_Ptr; }

		[[nodiscard]] static auto _None(void) { return This(nullptr); }
		[[nodiscard]] constexpr bool _is_some(void) const { return m_Ptr; }
	private:
    	explicit constexpr ArrayConstReverseIterator(ValueType* ptr) : m_Ptr(ptr) {}
	private:
	    ValueType* m_Ptr;
    };
} // export namespace nacl
