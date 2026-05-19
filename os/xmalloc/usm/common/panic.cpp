#pragma once

#ifndef BAREMETAL
#include <exception>
#include <stdexcept>

#endif

namespace xmalloc {
    enum ErrorType {
        success,
        out_of_memory,
        function_is_empty,
    };

    struct Error {
        ErrorType type;
        const char* comment;
    };

#ifndef BAREMETAL
    struct exception_base : public std::exception {
        const char* message;

        const char* what() const noexcept override {
            return message;
        }

        explicit exception_base(const char * comment) : message(comment){}
    };
#else
    struct exception_base {
        const char* message;

        const char* what() const {
            return message;
        }

        explicit exception_base(const char * comment) : message(comment){}
    };
#endif

    typedef exception_base out_of_memory_exception;
    typedef exception_base function_is_empty_exception;
    typedef exception_base unknown_error_exception;

    namespace detail {
        namespace handlers {
            inline void _h_out_of_memory(const char*);
            inline void _h_function_is_empty(const char*);
            inline void _h_unknown_error();

#ifndef BAREMETAL
            inline void _h_out_of_memory(const char* comment) {
                throw out_of_memory_exception(comment);
            }
            inline void _h_function_is_empty(const char* comment) {
                throw function_is_empty_exception(comment);
            }
            inline void _h_unknown_error() {
                throw unknown_error_exception("");
            }
#endif
        }

        inline void default_accept(Error err) {
            switch (err.type) {
                case success: break;
                case out_of_memory:
                    handlers::_h_out_of_memory(err.comment);
                case function_is_empty:
                    handlers::_h_function_is_empty(err.comment);
                default: handlers::_h_unknown_error();
            }
        }

        inline void (*accept)(Error err) = default_accept;
    }

    class ErrorContext {
        static const ErrorContext* context;
        const ErrorContext* last;

    public:
        ErrorContext() {
            last = context;
            context = this;
        }

        ~ErrorContext() {
            context = last;
        }

        void panic(Error err);
    };

    namespace detail {
        inline ErrorContext global_context = {};
        inline const ErrorContext firstctx = {global_context};

    }

    inline void ErrorContext::panic(Error err) {
        detail::accept(err);
    }

    inline void panic(ErrorType err, const char* comment) {
        detail::global_context.panic(Error{err, comment});
    }
}
