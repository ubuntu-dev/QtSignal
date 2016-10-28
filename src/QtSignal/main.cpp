
#include <stdio.h>
#include <stdlib.h>

#include <cstdio>
#include <iostream>
#include <string>
#include <atomic>
#include <mutex>
#include <thread>
#include <chrono>
#include <functional>
#include <tuple>
#include <type_traits>

#include "QtSignal.h"
#include "Functional.h"

enum signal_slots {
    OnValueChange,
    OnScrollChange
};

class FooA {
public:
    void onValueChange(int index) {
        std::cout << "FooA::onValueChange(int index): index = " << index << std::endl;
    }

    void onValueChange2(int index, int x, int y) {
        std::cout << "FooA::onValueChange2(int index, int x, int y): index = " << index
            << ", x = " << x << ", y = " << y << std::endl;
    }
};

class FooB {
public:
    void onValueChange(int index) {
        std::cout << "FooB::onValueChange(int index): index = " << index << std::endl;
    }

    void onValueChange2(int index, int x, int y) {
        std::cout << "FooB::onValueChange2(int index, int x, int y): index = " << index
            << ", x = " << x << ", y = " << y << std::endl;
    }
};

template <typename R, typename ...Args>
inline bool operator == (std::function<R(Args...)> const & func1, std::function<R(Args...)> const & func2) _NOEXCEPT {
    R (* const * ptr1)(Args...) = func1.template target<R(*)(Args...)>();
    R (* const * ptr2)(Args...) = func2.template target<R(*)(Args...)>();
    return ((ptr1 != nullptr) && (ptr2 != nullptr) && (ptr1 == ptr2));
}

template <typename R, typename ...Args>
inline bool operator == (std::function<R(Args...)> && func1, std::function<R(Args...)> && func2) _NOEXCEPT {
    return true;
}

void test_signal()
{
    using namespace std::placeholders;
    typedef jimi::signal<> signal_0;
    signal_0 & signal_0_inst = signal_0::get();

    signal_0_inst.connect(signal_slots::OnValueChange, []() { std::cout << "lambda::onValueChange()." << std::endl; });
    signal_0_inst.emit(signal_slots::OnValueChange);
    signal_0_inst.disconnect(signal_slots::OnValueChange);

    typedef jimi::signal<int> signal_int;
    signal_int & signal_int_inst = signal_int::get();

    FooA a; FooB b;
    std::function<void(int)> memfunc_a1 = std::bind(&FooA::onValueChange, &a, _1);
    std::function<void(int)> memfunc_a2 = std::bind(&FooA::onValueChange, &a, _1);
    std::function<void(int)> memfunc_b = std::bind(&FooB::onValueChange, &b, _1);
    printf("memfunc_a1.target_type().name() = %s\n\n", memfunc_a1.target_type().name());
    printf("jimi::getAddressOf(memfunc_a1) = 0x%p\n", (void *)jimi::getAddressOf(memfunc_a1));
    printf("jimi::getAddressOf(memfunc_a2) = 0x%p\n", (void *)jimi::getAddressOf(memfunc_a2));
    printf("\n");
    //std::function<void(FooA *, int)> memfunc_a(FooA::onValueChange, &a, _1);
    printf("(memfunc_a1 == memfunc_a2) ? %d\n", (int)(memfunc_a1 == memfunc_a2));
    printf("(memfunc_a1 == memfunc_a2) ? %d\n", (int)jimi::template function_equal<FooA>(memfunc_a1, memfunc_a2));
    signal_int_inst.connect(signal_slots::OnValueChange, memfunc_a1);
    signal_int_inst.connect(signal_slots::OnValueChange, memfunc_b);
    signal_int_inst.emit(signal_slots::OnValueChange, 100);
    signal_int_inst.disconnect(signal_slots::OnValueChange);

    std::function<void(int, int, int)> memfunc_a1_ = std::bind(&FooA::onValueChange2, &a, _1, _2, _3);
    std::function<void(int, int, int)> memfunc_a2_ = std::bind(&FooA::onValueChange2, &a, _1, _2, _3);

    printf("memfunc_a1_.target_type().name() = %s\n\n", memfunc_a1_.target_type().name());
    printf("(memfunc_a1_ == memfunc_a2_) ? %d\n", (int)jimi::template function_equal<FooA>(memfunc_a1_, memfunc_a2_));
}

void run_unittest()
{
    //
}

int main(int argn, char * argv[])
{
#if !defined(NDEBUG)
    run_unittest();
#endif

    test_signal();

#if defined(_WIN32) && (defined(NDEBUG) || !defined(NDEBUG))
    system("pause");
#endif
    return 0;
}