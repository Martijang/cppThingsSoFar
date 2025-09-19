#include <iostream>
#include <concepts>

template<typename... Type>
concept Addable = requires (Type... a){
    (a + ...);
};

class Calculator{
    public:
    template<typename... T>
        constexpr auto add(T ...args) requires Addable<T...>{
            return (args + ...);
        }

    template<typename... T>
        constexpr auto sub(T ...args) requires Addable<T...>{
            return (args - ...);
        }

    template<typename... T>
        constexpr auto multiply(T ...args) requires Addable<T...>{
            return (args * ...);
        }

    template<typename... T>
        constexpr auto divide(T ...args) requires Addable<T...>{
            return (args / ...);
        }
};

//solved
//tempNconCal.cpp:18:33: error: no matching member function for call to 'add' : solved
int main(){
    Calculator cal;
    constexpr auto result = cal.add<int, double, int>(5, 1.1, 1);
    std::cout << result << "\n";

    constexpr auto result2 = cal.sub<int, int>(5,5);
    std::cout << result2 << "\n";

    constexpr auto result3 = cal.multiply<double, int, int, int>(1.7, 2, 2, 4);
    std::cout << result3 << "\n";

    constexpr auto result4 = cal.divide<int, int>(25, 5);
    std::cout << result4 << "\n";
    return 0;
}