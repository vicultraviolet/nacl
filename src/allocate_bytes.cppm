export module nacl:allocate_bytes;

import :panic;
import :allocate_aligned;
import :ref;
import :option;
import :utils;

export namespace nacl {
    [[nodiscard]] inline Ref<ubyte> AllocateBytes(usize size, usize alignment) {
        void* ptr = AllocateAligned(size, alignment);
        Assert(ptr, "Bad alloc!");
        return RefOf(Transmute<ubyte*>(ptr));
    }

    [[nodiscard]] inline Option<Ref<ubyte>>
    TryAllocateBytes(usize size, usize alignment) noexcept {
        void* ptr = AllocateAligned(size, alignment);
        if (!ptr)
            return None();

        return SomeWith(RefOf<ubyte>, Transmute<ubyte*>(ptr));
    }

    inline void DeallocateBytes(Ref<ubyte> ref) noexcept {
        FreeAligned(As<void*>(ref.ptr()));
    }

    inline void TryDeallocateBytes(Option<Ref<ubyte>> ref) noexcept {
        ref.map(DeallocateBytes);
    }
} // export namespace nacl
