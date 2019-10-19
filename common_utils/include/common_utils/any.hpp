#ifndef CARPI_COMMON_UTILS_ANY_HPP
#define CARPI_COMMON_UTILS_ANY_HPP

#include <typeinfo>
#include <algorithm>
#include <sstream>

#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>

namespace carpi::utils {
    class Any {
        class Placeholder {
        public:
            virtual ~Placeholder() = default;

            [[nodiscard]] virtual const std::type_info &type_info() const = 0;

            [[nodiscard]] virtual Placeholder *clone() const = 0;

            [[nodiscard]] virtual std::string to_string() const = 0;
        };

        template<typename ValueType>
        class Holder : public Placeholder {
            const ValueType _held;
        public:
            explicit Holder(ValueType value) : _held(std::move(value)) {}

            [[nodiscard]] const std::type_info &type_info() const override {
                return typeid(ValueType);
            }

            [[nodiscard]] Placeholder *clone() const override {
                return new Holder(_held);
            }

            [[nodiscard]] std::string to_string() const override {
                std::stringstream stream;
                stream << _held;
                return stream.str();
            }
        };

        Placeholder *_content = nullptr;

    public:
        Any() = default;

        Any(const Any &other) : _content(other._content ? other._content->clone() : nullptr) {

        }

        template<typename ValueType>
        explicit Any(ValueType value) : _content(new Holder(std::move(value))) {

        }

        ~Any() {
            delete _content;
        }

        [[nodiscard]] const std::type_info &type_info() const {
            return _content ? _content->type_info() : typeid(void);
        }

        Any &swap(Any &rhs) {
            std::swap(_content, rhs._content);
            return *this;
        }

        Any &operator=(const Any &rhs) {
            delete _content;
            _content = rhs._content->clone();
            return *this;
        }

        template<typename ValueType>
        Any &operator=(const ValueType &rhs) {
            delete _content;
            _content = new Holder(rhs);
            return *this;
        }

        explicit operator const void *() const {
            return _content;
        }

        template<typename ValueType>
        bool copy_to(ValueType &value) const {
            const ValueType* copyable = to_ptr<ValueType>();
            if(copyable) {
                value = *copyable;
            }

            return copyable != nullptr;
        }

        template<typename ValueType>
        const ValueType *to_ptr() const {
            return type_info() == typeid(ValueType) ?
                   &static_cast<Holder<ValueType> *>(_content)->_held :
                   nullptr;
        }

        [[nodiscard]] std::string to_string() const {
            if(_content == nullptr) {
                return "(null)";
            } else {
                return _content->to_string();
            }
        }

        template<typename OStream>
        friend OStream& operator << (OStream& os, const Any& value) {
            return os << value.to_string();
        }
    };

    template<typename ValueType>
    inline ValueType any_cast(const Any& source) {
        const ValueType* result = source.to_ptr<ValueType>();
        return result ? *result : throw std::bad_cast();
    }

    template<typename result_type, typename arg_type>
    inline result_type lexical_cast(const arg_type& arg) {
        std::stringstream interpreter;
        interpreter << arg;
        result_type result{};
        if(!(interpreter >> result)) {
            throw std::bad_cast{};
        }

        return result;
    }
}

#endif //CARPI_COMMON_UTILS_ANY_HPP
