// from https://stackoverflow.com/questions/19808054/convert-c-function-pointer-to-c-function-pointer/19809787
#include <functional>

/*
template <typename T>
struct Callback;

template <typename Ret, typename... Params>
struct Callback<Ret(Params...)> {
    template <typename... Args>
    static Ret callback(Args... args) { return func(args...); }
    static std::function<Ret(Params...)> func;
};

// Initialize the static member.
template <typename Ret, typename... Params>
std::function<Ret(Params...)> Callback<Ret(Params...)>::func;
*/

/*
 * Example of usage:
 * 
 * // Store member function and the instance using std::bind.
 * Callback<void(int*)>::func = std::bind(&Foo::print, foo, std::placeholders::_1);
 * // Convert callback-function to c-pointer.
 * void (*c_func)(int*) = static_cast<decltype(c_func)>(Callback<void(int*)>::callback);
 */

// Alternative for callbacks without arguments
typedef void (*voidCCallback)();
template<typename T>
voidCCallback makeCCallback(void (T::*method)(),T* r){
    Callback<void()>::func = std::bind(method, r);
    void (*c_function_pointer)() = static_cast<decltype(c_function_pointer)>(Callback<void()>::callback);
    return c_function_pointer;
}

// Used like e.g. callback = makeCCallback(&InterruptRotaryEncoder::onChange, this);
