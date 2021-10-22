// https://stackoverflow.com/questions/11491505/python-with-statement-in-c
#include <functional>

template <typename T>
void with(T *t, std::function<void ()> fn) {
    t->bind();
    fn();
    t->unbind();
}
