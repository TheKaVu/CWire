#ifndef CWIRE_SEQUENCE_CALL_H
#define CWIRE_SEQUENCE_CALL_H

#include <algorithm>
#include <memory>
#include <vector>

#include "../delegate.hpp"

namespace cwr {
    template<typename R, typename ...Args>
    class multidelegate {

    public:
        using Handler = delegate<R, Args...>;

    protected:
        std::vector<Handler> m_Handlers;

    public:
        multidelegate() : m_Handlers() {}

        multidelegate(Handler& handler) {
            m_Handlers = {handler};
        }

        multidelegate(const multidelegate &other) : m_Handlers(std::copy(other.m_Handlers.begin(), other.m_Handlers.end())) {}

        multidelegate(const multidelegate &&other) noexcept : m_Handlers(std::move(other.m_Handlers)) {}

        virtual ~multidelegate() {
            m_Handlers.clear();
        }

        std::vector<Handler> handlers() const {
            return m_Handlers;
        }

        void add(const Handler &handler) {
            m_Handlers.emplace_back(handler);
        }

        void remove(const Handler &handler) {
            std::remove(m_Handlers.begin(), m_Handlers.end(), handler);
        }

        void operator +=(const Handler &handler) {
            add(handler);
        }

        void operator +=(const Handler &&handler) {
            add(handler);
        }

        void operator -=(const Handler &handler) {
            remove(handler);
        }

        void operator -=(const Handler &&handler) {
            remove(handler);
        }

        void invoke(Args... args, std::vector<R> &resultOut) const {
            for (Handler handler : m_Handlers) {
                resultOut.push_back(handler(args...));
            }
        }

        virtual void invoke(Args... args) const {
            for (Handler handler : m_Handlers) {
                handler(args...);
            }
        }

        void operator()(Args... args, std::vector<R> &resultOut) const {
            invoke(args..., resultOut);
        }

        virtual void operator()(Args... args) const {
            invoke(args...);
        }
    };
};

#endif //CWIRE_SEQUENCE_CALL_H
