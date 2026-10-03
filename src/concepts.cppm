export module nacl:concepts;

import :types;
import :meta;
import :utils;

export namespace nacl {
    namespace concepts {
        template<typename T, typename U>
        concept Same = meta::IsSame<T, U>;

        template<typename T, typename... P0toN>
        concept OneOf = meta::IsOneOf<T, P0toN...>;

        template<typename T>
        concept Void = meta::IsVoid<T>;

        template<typename T>
        concept LvalueReference = meta::IsLvalueReference<T>;

        template<typename T>
        concept RvalueReference = meta::IsRvalueReference<T>;

        template<typename T>
        concept ConstReference = meta::IsConstReference<T>;

        template<typename T>
        concept Const = meta::IsConst<T>;

        template<typename T>
        concept Reference = meta::IsReference<T>;

        template<typename T>
        concept Ptr = meta::IsPtr<T>;

        template<typename T>
        concept Union = meta::IsUnion<T>;

        template<typename  T>
        concept Class = meta::IsClass<T>;

        template<typename From, typename To>
        concept Convertible = meta::IsConvertible<From, To>;

        template<typename Base, typename Derived>
        concept BaseOf = meta::IsBaseOf<Base, Derived>;

        template<typename F, typename... Args>
        concept Invocable = requires(F&& fn, Args&&... args) {
            { Preserve<F>(fn)(Preserve<Args>(args)...) };
        };

        template<typename T>
        concept Optional = requires(const T t) {
            { T::_None() } -> Same<T>;
            { t._is_some() } -> Same<bool>;
        };

        template<typename T>
        concept Readable = requires(T x) {
            typename T::ValueType;
            { *x } -> Convertible<typename T::ValueType>;
            //{ x.operator->() } -> Same<typename T::ValueType*>;
        };

        template<typename T>
        concept Incrementable = requires(T x) {
            { ++x } -> Same<T&>;
            { x++ } -> Same<T>;
        };

        template<typename T>
        concept Decrementable = requires(T x) {
            { --x } -> Same<T&>;
            { x-- } -> Same<T>;
        };

        template<typename T>
        concept EqualityComparable = requires(T a, T b) {
            { a == b } -> Same<bool>;
            { a != b } -> Same<bool>;
        };

        template<typename T>
        concept TotallyOrdered = requires(T a, T b) {
            { a <  b } -> Same<bool>;
            { a >  b } -> Same<bool>;
            { a <= b } -> Same<bool>;
            { a >= b } -> Same<bool>;
        };

        template<typename T>
        concept InputIterator =
            Readable<T> &&
            Incrementable<T> &&
            EqualityComparable<T>;

        template<typename T>
        concept BidirectionalIterator =
            InputIterator<T> &&
            Decrementable<T>;

        template<typename T>
        concept RandomAccessIterator =
            BidirectionalIterator<T> &&
            TotallyOrdered<T> &&
            requires(T it, usize n) {
                { it += n } -> Same<T&>;
                { it -= n } -> Same<T&>;
                { it +  n } -> Same<T>;
                { it -  n } -> Same<T>;
                { it - it } -> Same<isize>;
            };

        template<typename T>
        concept DefaultConstructable = requires {
            { T::Default() } -> Convertible<T>;
        };

        template<typename T>
        concept Maker = requires(T x) {
            { x.make() };
        };
    } // namespace concepts
} // export namespace nacl
