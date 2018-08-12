#include "errors.hpp"

namespace church::parser {

    virtual const char * ParseError::what() const noexcept {
        return "";
    }
}
