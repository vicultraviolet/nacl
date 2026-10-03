export module nacl:list;

import :types;
import :tags;
import :meta;
import :panic;
import :utils;
import :ref;
import :array_iterators;
import :aligned_heap_allocator;

#if defined(__has_cpp_attribute) && __has_cpp_attribute(msvc::no_unique_address)
    #define NO_UNIQUE_ADDRESS [[msvc::no_unique_address]]
#elif defined(__has_cpp_attribute) && __has_cpp_attribute(no_unique_address)
    #define NO_UNIQUE_ADDRESS [[no_unique_address]]
#else
    #define NO_UNIQUE_ADDRESS
#endif

export namespace nacl {
    template<typename T>
    class List {
    public:
        using ValueType = T;
        using AllocatorType = Allocator<T>;

        using Iterator = ArrayIterator<List>;
        using ConstIterator = ArrayConstIterator<List>;
        using ReverseIterator = ArrayReverseIterator<List>;
        using ConstReverseIterator = ArrayConstReverseIterator<List>;
    public:
        [[nodiscard]] static List Empty(void) {
            return List(nullptr, 0, 0);
        }
        [[nodiscard]] static List Default(void) { return Empty(); }

        [[nodiscard]] static List Reserve(usize capacity) {
            auto list = Empty();
            list.reserve(capacity);
            return list;
        }

        ~List(void) { discard(); }

        List(const List& other)
        : List(nullptr, 0, 0) {
            reserve(other.m_Capacity);
            while (m_Length != other.m_Length)
                add_to_back(other[m_Length]);
        }
        List& operator=(const List& other) {
            if (this == &other)
                return *this;

            if (m_Capacity != other.m_Capacity) {
                discard();
                reserve(other.m_Capacity);
            } else {
                clear();
            }

            while (m_Length != other.m_Length)
                add_to_back(other[m_Length]);

            return *this;
        }

        List(List&& other)
        : m_Data(Exchange(other.m_Data, nullptr)),
          m_Length(Exchange(other.m_Length, 0)),
          m_Capacity(Exchange(other.m_Capacity, 0)),
          m_Allocator(Allocator<T>::New())
        {}
        List& operator=(List&& other) {
            if (this == &other)
                return *this;

            discard();

            m_Data = Exchange(other.m_Data, nullptr);
            m_Length = Exchange(other.m_Length, 0);
            m_Capacity = Exchange(other.m_Capacity, 0);

            return *this;
        }

        template<typename... Args>
        usize add_to_back(Args&&... args) {
            _grow_if();
            m_Allocator.construct(
                RefOf(m_Data + m_Length),
                Preserve<Args>(args)...
            );
            return m_Length++;
        }
        template<typename F, typename... Args>
        usize add_to_back_with(F&& fn, Args&&... args) {
            _grow_if();
            m_Allocator.construct_with(
                RefOf(m_Data + m_Length),
                Preserve<F>(fn),
                Preserve<Args>(args)...
            );
            return m_Length++;
        }
        void remove_from_back(void) {
            m_Allocator.destruct(RefOf(m_Data + m_Length - 1));
            m_Length--;
        }

        void clear(void) {
            for (auto it = rbegin(); it != rend(); it++)
                it->~T();

            m_Length = 0;
        }

        void reserve(usize new_capacity) {
            Assert(new_capacity > m_Length,
                "Could not reserve memory for List: new capacity is less than current length!"
            );
            if (new_capacity == m_Capacity)
                return;

            Ref<T> new_data = m_Allocator.alloc(new_capacity);
            for (usize i = 0; i < m_Length; i++) {
                auto current_item = RefOf(new_data.ptr() + i);
                m_Allocator.construct(current_item, AsRvalue(m_Data[i]));
            }

            if (m_Data)
                m_Allocator.dealloc(RefOf(m_Data), m_Capacity);
            m_Capacity = new_capacity;
            m_Data = new_data.ptr();
        }

        void grow(usize n) {
            reserve(m_Capacity + n);
        }
        void grow_by(float r) {
            reserve((usize)(m_Capacity * r + 0.5f));
        }

        void discard(void) {
            clear();

            m_Allocator.dealloc(RefOf(m_Data), m_Capacity);
            m_Data = nullptr;
            m_Capacity = 0;
        }

        [[nodiscard]] constexpr auto at(usize i) {
            return Iterator::New(m_Data + i);
        }
        [[nodiscard]] constexpr auto at(usize i) const {
            return ConstIterator::New(m_Data + i);
        }
        [[nodiscard]] constexpr auto rat(usize i) {
            return at(m_Length).reverse() + 1 + i;
        }
        [[nodiscard]] constexpr auto rat(usize i) const {
            return at(m_Length).reverse() + 1 + i;
        }

        [[nodiscard]] constexpr auto begin(void) { return at(0); }
        [[nodiscard]] constexpr auto end(void) { return at(m_Length); }

        [[nodiscard]] constexpr auto begin(void) const { return at(0); }
        [[nodiscard]] constexpr auto end(void) const { return at(m_Length); }

        [[nodiscard]] constexpr auto rbegin(void) { return rat(0); }
        [[nodiscard]] constexpr auto rend(void) { return rat(m_Length); }

        [[nodiscard]] constexpr auto rbegin(void) const { return rat(0); }
        [[nodiscard]] constexpr auto rend(void) const { return rat(m_Length); }

        [[nodiscard]] constexpr T& operator[](usize i) { return m_Data[i]; }
        [[nodiscard]] constexpr const T& operator[](usize i) const { return m_Data[i]; }

        [[nodiscard]] constexpr Ref<T> ref(void) { return RefFromPtr(m_Data); }
        [[nodiscard]] constexpr Ref<const T> ref(void) const { return RefFromPtr(m_Data); }
        [[nodiscard]] constexpr Ref<T> ref(void) { return RefOf(m_Data); }
        [[nodiscard]] constexpr Ref<const T> ref(void) const { return RefOf(m_Data); }

        [[nodiscard]] constexpr T* ptr(void) { return m_Data; }
        [[nodiscard]] constexpr const T* ptr(void) const { return m_Data; }

        [[nodiscard]] constexpr bool is_empty(void) const { return m_Length == 0; }

        [[nodiscard]] constexpr usize length(void) const { return m_Length; }
        [[nodiscard]] constexpr usize capacity(void) const { return m_Capacity; }
        [[nodiscard]] constexpr const auto& allocator(void) const { return m_Allocator; }
    private:
        List(T* data, usize length, usize capacity)
        : m_Data(data),
          m_Length(length), m_Capacity(capacity),
          m_Allocator(Allocator<T>::New())
        {}

        void _grow_if(void) {
            if (m_Length == m_Capacity) {
                if (m_Capacity == 0)
                    reserve(4);
                else
                    grow_by(2.0f);
            }
        }
    private:
        T* m_Data;
        usize m_Length, m_Capacity;
        NO_UNIQUE_ADDRESS Allocator<T> m_Allocator;
    };
} // export namespace nacl
