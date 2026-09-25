export module nacl:concepts;

import :meta;

export namespace nacl {
    namespace concepts {
        template<typename T, typename... P0toN>
        concept IsOneOf = meta::IsOneOf<T, P0toN...>;

        template<typename T>
        concept IsVoid = meta::IsVoid<T>;
    } // namespace concepts
} // export namespace nacl
