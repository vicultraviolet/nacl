export module nacl:aligned_heap_allocator;

import :ref;
import :allocate_bytes;
import :utils;

export namespace nacl {
    template<typename T>
    class AlignedHeapAllocator {
    public:
        using ValueType = T;

        [[nodiscard]] static constexpr auto New(void) { return AlignedHeapAllocator(); }
        [[nodiscard]] static constexpr auto Default(void) { return AlignedHeapAllocator(); }

        AlignedHeapAllocator(const AlignedHeapAllocator& other) {}
        AlignedHeapAllocator& operator=(const AlignedHeapAllocator& other) {
            return *this;
        }
        AlignedHeapAllocator(AlignedHeapAllocator&& other) {}
        AlignedHeapAllocator& operator=(AlignedHeapAllocator&& other) {
            return *this;
        }

        [[nodiscard]] Ref<T> alloc(usize length) {
            return As<Ref<T>>(AllocateBytes(sizeof(T) * length, alignof(T)));
        }
        void dealloc(Ref<T> ref, usize length_) {
            DeallocateBytes(As<Ref<ubyte>>(ref));
        }

        template<typename... Args>
        void construct(Ref<T> ref, Args&&... args) {
            ConstructAt(ref.ptr(), Preserve<Args>(args)...);
        }
        template<typename F, typename... Args>
        void construct_with(Ref<T> ref, F&& fn, Args&&... args) {
            ConstructAtWith(ref.ptr(), Preserve<F>(fn), Preserve<Args>(args)...);
        }
        void destruct(Ref<T> ref) noexcept {
            DestructAt(ref.ptr());
        }
    private:
        constexpr AlignedHeapAllocator(void) = default;
    };

    template<typename T>
    using Allocator = AlignedHeapAllocator<T>;
} // export namespace nacl
