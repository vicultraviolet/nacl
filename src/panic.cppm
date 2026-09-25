export module nacl:panic;

export namespace nacl {
    void Panic(const char* message) {
        throw message;
    }

    void Assert(bool x, const char* message) {
    #ifndef NDEBUG
        if (!x)
            Panic(message);
    #endif
    }
} // export namespace nacl
