export module nacl:box;

import :meta;
import :concepts;
import :utils;
import :ref;
import :aligned_heap_allocator;

export namespace nacl {
    template<typename T>
    class Box {
    public:
        using ValueType = T;
    public:
        [[nodiscard]] static Box New(Ref<T> data) { return Box(data); }
        [[nodiscard]] static Box Of(T* ptr) { return Box(RefOf(ptr)); }

        template<typename... Args>
        [[nodiscard]] static Box Make(Args&&... args) {
            auto allocator = Allocator<T>::New();
            Ref<T> data = allocator.alloc(1);
            allocator.construct(data, Preserve<Args>(args)...);
            return Box(data);
        }
        template<typename F, typename... Args>
        [[nodiscard]] static Box MakeWith(F&& fn, Args&&... args) {
            auto allocator = Allocator<T>::New();
            Ref<T> data = allocator.alloc(1);
            allocator.construct_with(data, Preserve<F>(fn), Preserve<Args>(args)...);
            return Box(data);
        }
        template<concepts::Maker D>
        [[nodiscard]] static Box MakeFrom(D&& deferred) {
            auto allocator = Allocator<T>::New();
            Ref<T> data = allocator.alloc(1);
            allocator.construct_from(data, Preserve<D>(deferred));
            return Box(data);
        }

        ~Box(void) { discard(); }

        Box(const Box& other) = delete;
        Box& operator=(const Box& other) = delete;

        Box(Box&& other) : m_Data(AsRvalue(other.m_Data)) {}
        Box& operator=(Box&& other) {
            if (this != &other) {
                discard();
                m_Data = AsRvalue(other.m_Data);
            }
            return *this;
        }

        template<typename U> requires concepts::BaseOf<U, T>
		[[nodiscard]] constexpr operator Box<U>(void) && {
		    Ref<T> data = m_Data;
			m_Data = Ref<T>::_None();
			Ref<U> converted_data = As<Ref<U>>(data);
    		return Box<U>::Of(converted_data);
		}

		// WARNING: use carefully!
        void discard(void) {
            if (m_Data._is_some()) {
                auto allocator = Allocator<T>::New();
                allocator.destruct(m_Data);
                allocator.dealloc(m_Data, 1);
                m_Data = Ref<T>::_None();
            }
        }
        // WARNING: use carefully!
        [[nodiscard]] Ref<T> release(void) {
            Ref<T> data = m_Data;
            m_Data = Ref<T>::_None();
            return data;
        }
        // WARNING: use carefully!
        [[nodiscard]] T take(void) {
            Assert(_is_some(), "Could not take from Box: Invalid Box!");
            T value = AsRvalue(*m_Data);
            discard();
            return value;
        }

		[[nodiscard]] constexpr T& operator*(void) { return *m_Data; }
		[[nodiscard]] constexpr const T& operator*(void) const { return *m_Data; }

		[[nodiscard]] constexpr T* operator->(void) { return m_Data.ptr(); }
		[[nodiscard]] constexpr const T* operator->(void) const { return m_Data.ptr(); }

		[[nodiscard]] constexpr Ref<T> ref(void) { return m_Data; }
		[[nodiscard]] constexpr Ref<const T> ref(void) const { return m_Data; }

		[[nodiscard]] constexpr operator Ref<T>(void) { return m_Data; }
		[[nodiscard]] constexpr operator Ref<const T>(void) const { return m_Data; }

		[[nodiscard]] constexpr bool operator==(const Box& other) const {
    		return m_Data == other.m_Data;
		}
		[[nodiscard]] constexpr bool operator!=(const Box& other) const {
    		return m_Data != other.m_Data;
		}

        [[nodiscard]] static Box _None(void) { return Box(Ref<T>::_None()); }
        [[nodiscard]] constexpr bool _is_some(void) const { return m_Data._is_some(); }
    private:
        explicit Box(Ref<T> ref) : m_Data(ref) {}
    private:
        Ref<T> m_Data;
    };

    template<typename To, typename From>
    [[nodiscard]] Option<Box<To>> Downcast(Box<From>&& from) {
        Option<Ref<To>> to = Downcast<To, From>(from.ref());
        if (to.is_none()) {
            return None();
        } else {
            (void)from.release();
            return SomeWith(Box<To>::New, to.unwrap());
        }
	}

	template<typename T>
	[[nodiscard]] constexpr Box<T> NewBox(Ref<T> ref) { return Box<T>::New(ref); }

	template<typename T>
	[[nodiscard]] constexpr Box<T> BoxOf(T* ptr) { return Box<T>::Of(ptr); }

	template<typename T, typename... Args>
	[[nodiscard]] constexpr Box<T> MakeBox(Args&&... args) {
    	return Box<T>::Make(Preserve<Args>(args)...);
	}

	template<typename F, typename... Args>
	[[nodiscard]] constexpr auto MakeBoxWith(F&& fn, Args&&... args) {
	    using T = typename meta::ResultOf<F, Args...>;
		return Box<T>::MakeWith(Preserve<F>(fn), Preserve<Args>(args)...);
	}

	template<concepts::Maker D>
	[[nodiscard]] constexpr auto MakeBoxFrom(D&& deferred) {
	    using T = typename D::ResultOf;
		return Box<T>::MakeFrom(Preserve<D>(deferred));
	}
} // export namespace nacl
