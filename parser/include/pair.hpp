#pragma once

namespace church {
    template<typename First, typename Second>
    struct Pair {
        First first;
        Second second;
        using first_type = First;
        using second_type = Second;
    };

    template<typename First, typename Second>
    constexpr auto make_pair(First f, Second s) {
        return Pair<First, Second>{f, s};
    }
}
