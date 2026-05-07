#include <type_traits>
#include <cstddef> 

template <typename... Ts>
struct TypeList;

namespace detail 
{
    template <std::size_t Index, typename TList>
    struct GetImpl;

    template <typename Head, typename... Tail>
    struct GetImpl<0, TypeList<Head, Tail...>> { using type = Head; };

    template <std::size_t Index, typename Head, typename... Tail>
    struct GetImpl<Index, TypeList<Head, Tail...>> 
    {
        using type = typename GetImpl<Index - 1, TypeList<Tail...>>::type;
    };

    template <typename T, typename TList>
    struct ContainsImpl;

    template <typename T>
    struct ContainsImpl<T, TypeList<>> { static constexpr bool value = false; };

    template <typename T, typename... Tail>
    struct ContainsImpl<T, TypeList<T, Tail...>> { static constexpr bool value = true; };

    template <typename T, typename Head, typename... Tail>
    struct ContainsImpl<T, TypeList<Head, Tail...>> 
    {
        static constexpr bool value = ContainsImpl<T, TypeList<Tail...>>::value;
    };

    template <typename T, typename TList>
    struct IndexOfImpl;

    template <typename T, typename... Tail>
    struct IndexOfImpl<T, TypeList<T, Tail...>> { static constexpr std::size_t value = 0; };

    template <typename T, typename Head, typename... Tail>
    struct IndexOfImpl<T, TypeList<Head, Tail...>> 
    {
        static constexpr std::size_t value = 1 + IndexOfImpl<T, TypeList<Tail...>>::value;
    };

    template <typename TList, typename T>
    struct PushBackImpl;

    template <typename... Ts, typename T>
    struct PushBackImpl<TypeList<Ts...>, T> { using type = TypeList<Ts..., T>; };

    template <typename TList, typename T>
    struct PushFrontImpl;

    template <typename... Ts, typename T>
    struct PushFrontImpl<TypeList<Ts...>, T> { using type = TypeList<T, Ts...>; };
}

template <typename... Ts>
struct TypeList 
{
    static constexpr std::size_t Size = sizeof...(Ts);

    template <std::size_t I>
    using Get = typename detail::GetImpl<I, TypeList<Ts...>>::type;

    template <typename T>
    static constexpr bool Contains = detail::ContainsImpl<T, TypeList<Ts...>>::value;

    template <typename T>
    static constexpr std::size_t IndexOf = detail::IndexOfImpl<T, TypeList<Ts...>>::value;

    template <typename T>
    using PushBack = typename detail::PushBackImpl<TypeList<Ts...>, T>::type;

    template <typename T>
    using PushFront = typename detail::PushFrontImpl<TypeList<Ts...>, T>::type;
};


int main() 
{
    using MyList = TypeList<int, double, char>;

    static_assert(MyList::Size == 3, "Size must be 3");

    static_assert(std::is_same_v<MyList::Get<0>, int>, "index 0 must be int");
    static_assert(std::is_same_v<MyList::Get<1>, double>, "index 1 must be double");
    static_assert(std::is_same_v<MyList::Get<2>, char>, "index 2 must be char");

    static_assert(MyList::Contains<double> == true, "list contains double");
    static_assert(MyList::Contains<float> == false, "list does not contain float");

    static_assert(MyList::IndexOf<double> == 1, "index of double is 1");
    static_assert(MyList::IndexOf<char> == 2, "index of char is 2");

    using AppendedList = MyList::PushBack<float>;
    static_assert(std::is_same_v<AppendedList, TypeList<int, double, char, float>>, "PushBack failed");
    static_assert(AppendedList::Size == 4, "New size must be 4");

    using PrependedList = MyList::PushFront<short>;
    static_assert(std::is_same_v<PrependedList, TypeList<short, int, double, char>>, "PushFront failed");
    
    static_assert(std::is_same_v<PrependedList::Get<0>, short>, "first element must be short"); 

    return 0;
}