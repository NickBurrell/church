#pragma once

namespace church {

    template<typename InputIter, typename OutputIter>
    constexpr OutputIter copy(InputIter first, InputIter last, OutputIter output_first) {
        while (first != last) {
            *output_first++ = *first++;
        }
        return output_first;
    }

    template<typename InputIter, typename UnaryPredicate>
    constexpr InputIter findIf(InputIter first, InputIter last, UnaryPredicate p) {
        for (; first != last; ++first) {
            if(p(*first)) {
                return first;
            }
        }
        return last;
    }

    template<typename InputIter, typename T>
    constexpr InputIter find(InputIter first, InputIter last, const T& value) {
        for (; first != last; ++first) {
            if(*first == value) {
                return first;
            }
        }
        return last;
    }
}
