#pragma once

#include "pair.hpp"
#include "maybe.hpp"

#include <string_view>

namespace church::parser {
    using ParserInput = std::string_view;
    template <typename T>
    using ParserResult = Maybe<Pair<T, std::string_view>>;
    template <typename Parser>
    using MaybeParserReturn = typename std::result_of<Parser(ParserInput)>;
    template <typename Parser>
    using ParserReturn = typename MaybeParserReturn<Parser>::value_type;
    template <typename Parser>
    using ParserType = typename ParserReturn<Parser>::first_type;

    namespace detail {
        template<typename Parser, typename T, typename Function>
        constexpr Pair<T, ParserInput> accumulateParse(ParserInput input, Parser&& parser, T init, Function&& f) {
            while (!input.empty()) {
                const auto result = parser(input);
                if (!result)
                    return make_pair(init, input);
                init = function(init, result->first);
                input = result->second;
            }
            return make_pair(init, input);
        }
        template<typename Parser, typename T, typename Function>
        constexpr Pair<T, ParserInput> accumulateN(ParserInput input, Parser&& parser, std::size_t count, T init, Function&& function) {
            while(count != 0) {
                const auto result = parser(input);
                if (!result)
                    return make_pair(init, input);
                init = function(init, result->first);
                input = result->second;
                --count;
            }
        }
    }

    template<typename Function, typename Parser>
    constexpr auto fmap(Function&& function, Parser&& parser) {
        using Return = ParserResult<std::result_of_t<Function(ParserType<Parser>)>>;
        return [function = std::forward<Function>(function),
                parser = std::forward<Parser>(parser)] (ParserInput input) -> Return {
                   const auto result = parser(input);
                   if (!result)
                       return std::nullopt;
                   return Return(make_pair(f(result->first), result->second));
               };
    }

    template<typename Parser, typename Function>
    constexpr auto bind(Parser&& parser, Function&& function) {
        using Return = std::result_of_t<Function(ParserType<Parser>, ParserInput)>;
        return [=] (ParserInput input) -> Return {
                   const auto result = parser(input);
                   if (!result)
                       return std::nullopt;
                   return function(result->first, result->second);
               };
    }

    template<typename T>
    constexpr auto liftParser(T&& t) {
        return [t = std::forward<T>(t)] (ParserInput input) {
                   return ParserResult<T> (make_pair(std::move(t), input));
               };
    }

    template<typename T>
    constexpr auto fail(T) {
        return [=] (ParserInput) -> ParserResult<T> {
            return std::nullopt;
        };
    }

    template<typename T, typename ErrorFunction>
    constexpr auto fail(T, ErrorFunction f) {
        return [=] (ParserInput) -> ParserResult<T> {
            f();
            return std::nullopt;
        };
    }
    template<typename Parser1, typename Parser2,
             typename = std::enable_if_t<std::is_same_v<ParserType<Parser1>, ParserType<Parser2>>>>
    constexpr auto operator|(Parser1&& parser1, Parser2&& parser2) {
        return [=] (ParserInput input) {
                   const auto result1 = parser1(input);
                   if (result1)
                       return result1;
                   return parser2(input);
               };
    }

    template<typename Parser1, typename Parser2, typename Function,
             typename Return = std::result_of_t<Function(ParserType<Parser1>, ParserType<Parser2>)>>
    constexpr auto combine(Parser1&& parser1, Parser2&& parser2, Function&& function) {
        return [=] (ParserInput input) -> ParserResult<Return> {
            const auto result1 = parser1(input);
            if (!result1)
                return std::nullopt;
            const auto result2 = parser2(result1->second);
            if (!result2)
                return std::nullopt;
            return ParserResult<Return>(function(result1->first, result2->first), result2->second);
        };
    }

    template<typename Parser1, typename Parser2,
             typename = ParserReturn<Parser1>, typename = ParserReturn<Parser2>>
    constexpr auto operator<(Parser1&& parser1, Parser2&& parser2) {
        return combine(std::forward<Parser1>(parser1),
                       std::forward<Parser2>(parser2),
                       [] (auto, const auto& result){return result;});
    }

    template<typename Parser1, typename Parser2,
             typename = ParserReturn<Parser1>, typename = ParserReturn<Parser2>>
    constexpr auto operator>(Parser1&& parser1, Parser2&& parser2) {
        return combine(std::forward<Parser1>(parser1),
                       std::forward<Parser2>(parser2),
                       [] (const auto& result, auto){return result;});
    }


    template <typename Parser, typename T, typename Function>
    constexpr auto many(Parser&& parser, T&& init, Function&& function) {
        return  [parser = std::forward<Parser>(parser), init = std::forward<T>(init),
                 function = std::forward<Function>(function)] (ParserInput input) {
                    return ParserResult<T>(detail::accumulateParse(input, parser, init, function));
                };
    }

    template <typename Parser, typename T, typename Function>
    constexpr auto many1(Parser&& parser, T&& init, Function&& function) {
        return [parser = std::forward<Parser>(parser), init = std::forward<T>(init),
                function = std::forward<Function>(function)] (ParserInput input) -> ParserResult<T> {
            const auto result = parser(input);
            if(!result)
                return std::nullopt;
            return ParserResult<T>(detail::accumulateParse(result->second, parser, function(init, result->first), function));
        };
    }

    template <typename Parser, typename T, typename Function>
    constexpr auto exactlyN(Parser&& parser, std::size_t count, T&&  init, Function&& function) {
        return [parser = std::forward<Parser>(parser), count, init = std::forward<T>(init),
                function = std::forward<Function>(function)] (ParserInput input) -> ParserResult<T> {
            return ParserResult<T>(detail::accumulateN(input, parser, count, init, function));
        };
    }

    template <typename Parser, typename T = ParserReturn<Parser>>
    constexpr auto option(T&& def, Parser&& parser) {
        return [parser = std::forward<Parser>(parser),
                def = std::forward<T>(def)] (ParserInput input) {
                   const auto result = parser(input);
                   if (result)
                       return result;
                   return ParserResult<T>(make_pair(def, input));
               };
    }

    template <typename Parser1, typename Parser2, typename Function>
    constexpr auto seperatedBy(Parser1&& parser1, Parser2&& parser2, Function&& function) {
        using T = ParserReturn<Parser1>;
        return [parser1 = std::forward<Parser1>(parser1), parser2 = std::forward<Parser2>(parser2),
                function = std::forward<Function>(function)] (ParserInput input) -> ParserResult<T> {
                   const auto result = parser1(input);
                   if (!result)
                       return std::nullopt;
                   const auto parser = parser2 < parser1;
                   return ParserResult<T>(detail::accumulateParse(result->second, parser, result->first, function));
               };
    }

    template <typename Parser1, typename Parser2, typename Function0, typename Function>
    constexpr auto seperatedBy(Parser1&& parser1, Parser2&& parser2, Function0&& init, Function&& function) {
        using Return = ParserResult<std::result_of_t<Function0>>;
        return [parser1 = std::forward<Parser1>(parser1), parser2 = std::forward<Parser2>(parser2),
                init = std::forward<Function0>(init), function = std::forward<Function>(function)] (ParserInput input) -> Return {
                   const auto result = parser1(input);
                   if (!result)
                       return Return(make_pair(init(), input));
                   const auto parser = parser2 < parser1;
                   return R(detail::accumulateParse(result->second, parser, function(init(), result->first), function));
               };
    }

    template <typename Parser1, typename Parser2, typename T, typename Function>
    constexpr auto seperatedByValue(Parser1&& parser1, Parser2 parser2, T&& init, Function&& function) {
        using Return = ParserResult<std::remove_reference_t<T>>;
        return [parser1 = std::forward<Parser1>(parser1), parser2 = std::forward<Parser2>(parser2),
                init = std::forward<T>(init), function = std::forward<Function>(function)] (ParserInput input) -> Return {
                   const auto result = parser1(input);
                   if (!result)
                       Return(make_pair(init, input));
                   const auto parser = parser2 < parser1;
                   return Return(detail::accumulateParse(result->second, parser, function(init, result->first), function));
               };
    }
}
