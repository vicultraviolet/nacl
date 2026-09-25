export module nacl:meta;

export namespace nacl {
    namespace meta {
        template<typename T>
        constexpr T Unevaluated(void);

        template<typename T, T val>
        struct constant {
            using ValueType = T;
            static constexpr T value = val;
        };

        struct true_type : constant<bool, true> {};
        struct false_type : constant<bool, false> {};

        template<typename T>
        struct type_is {
            using type = T;
        };

        template<typename T, typename... P0toN>
        struct is_one_of;

        template<typename T>
        struct is_one_of<T> : false_type {};

        template<typename T, typename... P1toN>
        struct is_one_of<T, T, P1toN...> : true_type {};

        template<typename T, typename U, typename... P1toN>
        struct is_one_of<T, U, P1toN...> : is_one_of<T, P1toN...> {};

        template<typename T, typename... P0toN>
        constexpr bool IsOneOf = is_one_of<T, P0toN...>::value;

        template<typename T>
        struct is_void : is_one_of<T, void, const void, volatile void, const volatile void> {};

        template<typename T>
        constexpr bool IsVoid = is_void<T>::value;

        template<typename T>
        struct is_lvalue_reference : false_type {};
        template<typename T>
        struct is_lvalue_reference<T&> : true_type {};

        template<typename T>
        constexpr bool IsLvalueReference = is_lvalue_reference<T>::value;

        template<typename T>
        struct remove_reference : type_is<T> {};
        template<typename T>
        struct remove_reference<T&> : type_is<T> {};
        template<typename T>
        struct remove_reference<T&&> : type_is<T> {};

        template<typename T>
        using RemoveReference = typename remove_reference<T>::type;

        template<typename T>
        struct remove_const : type_is<T> {};
        template<typename T>
        struct remove_const<const T> : type_is<T> {};

        template<typename T>
        using RemoveConst = typename remove_const<T>::type;

        template<typename T>
        struct remove_volatile : type_is<T> {};
        template<typename T>
        struct remove_volatile<volatile T> : type_is<T> {};

        template<typename T>
        using RemoveVolatile = typename remove_volatile<T>::type;

        template<typename T>
        struct remove_ptr : type_is<T> {};
        template<typename T>
        struct remove_ptr<T*> : type_is<T> {};
        template<typename T>
        struct remove_ptr<T* const> : type_is<T> {};
        template<typename T>
        struct remove_ptr<T* volatile> : type_is<T> {};
        template<typename T>
        struct remove_ptr<T* const volatile> : type_is<T> {};

        template<typename T>
        using RemovePtr = typename remove_ptr<T>::type;

        namespace detail {
            template<typename T>
            auto test_returnable(int) -> decltype(
                void(static_cast<T(*)()>(nullptr)), true_type{}
            );
            template<typename>
            auto test_returnable(...) -> false_type;

            template<typename From, typename To>
            auto test_implicitly_convertible(int) -> decltype(
                void(Unevaluated<void(&)(To)>()(Unevaluated<From>())), true_type{}
            );
            template<typename, typename>
            auto test_implicitly_convertible(...) -> false_type;
        } // namespace detail

        template<typename From, typename To>
        struct is_convertible : constant<bool,
            (decltype(detail::test_returnable<To>(0))::value &&
             decltype(detail::test_implicitly_convertible<From, To>(0))::value) ||
            (IsVoid<From> && IsVoid<To>)
        > {};

        template<typename From, typename To>
        constexpr bool IsConvertible = is_convertible<From, To>::value;
    } // namespace meta
} // export namespace nacl
