export module nacl:downcast;

import :meta;
import :ref;
import :option;

export namespace nacl {
    template<typename From, typename To>
    [[nodiscard]] Option<Ref<To>> Downcast(Ref<From> from) {
        static_assert(
            meta::IsConst<From> ? meta::IsConst<To> : true,
            "Could not downcast: From is const, but To is not!"
        );

        From* from_ptr = from.ptr();
        To* to_ptr = dynamic_cast<To*>(from_ptr);
        return OptionRefOf(to_ptr);
    }
} // export namespace nacl
