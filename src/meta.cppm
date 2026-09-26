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

        template<typename... Args>
        using Void = void;

        template<typename T, typename U>
        struct is_same : false_type {};
        template<typename T>
        struct is_same<T, T> : true_type {};
        template<typename T, typename U>
        constexpr bool IsSame = is_same<T, U>::value;

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
        struct is_lvalue_reference : is_same<T, T&> {};
        template<typename T>
        constexpr bool IsLvalueReference = is_lvalue_reference<T>::value;

        template<typename T>
        struct is_rvalue_reference : is_same<T, T&&> {};
        template<typename T>
        constexpr bool IsRvalueReference = is_rvalue_reference<T>::value;

        template<typename T>
        struct is_const_reference : is_same<T, const T&> {};
        template<typename T>
        constexpr bool IsConstReference = is_const_reference<T>::value;

        template<typename T>
        struct is_const : is_one_of<T, const T, const volatile T> {};
        template<typename T>
        constexpr bool IsConst = is_const<T>::value;

        template<typename T>
        struct is_reference : is_one_of<T, T&, T&&, const T&, const T&&> {};
        template<typename T>
        constexpr bool IsReference = is_reference<T>::value;

        template<typename T>
        struct is_ptr : is_one_of<T, T*, T* const, T* volatile, T* const volatile> {};
        template<typename T>
        constexpr bool IsPtr = is_ptr<T>::value;

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

        template<bool B, typename T = void>
        struct enable_when {};
        template<typename T>
        struct enable_when<true, T> : type_is<T> {};
        template<bool B, typename T = void>
        using EnableWhen = typename enable_when<B, T>::type;

        template<bool B, typename T, typename U>
        struct conditional;
        template<typename T, typename U>
        struct conditional<true, T, U> : type_is<T> {};
        template<typename T, typename U>
        struct conditional<false, T, U> : type_is<U> {};
        template<bool B, typename T, typename U>
        using Conditional = typename conditional<B, T, U>::type;

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

        template<typename F, typename... Args>
        struct result_of : type_is<
            decltype(
                (Unevaluated<F>())(Unevaluated<Args>()...)
            )
        > {};
        template<typename F, typename... Args>
        using ResultOf = typename result_of<F, Args...>::type;
    } // namespace meta
} // export namespace nacl
