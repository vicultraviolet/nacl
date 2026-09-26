export module nacl:unit;

export namespace nacl {
    struct Unit {
        constexpr bool operator==(Unit other) const { return true; }
        constexpr bool operator!=(Unit other) const { return false; }
    };
} // export namespace nacl
