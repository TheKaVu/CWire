#ifndef CWIRE_SEQUENCE_CALL_H
#define CWIRE_SEQUENCE_CALL_H

#include <algorithm>
#include <memory>
#include <vector>

#include "delegate.hpp"

namespace cwr {
    template<typename R, typename ...Args>
    class multidelegate {

    public:
        using Delegate = delegate<R, Args...>;

    protected:
        std::vector<Delegate> m_Delegates;

    public:
        multidelegate() : m_Delegates() {}

        multidelegate(Delegate& delegate) {
            m_Delegates = {delegate};
        }

        multidelegate(const multidelegate &other) : m_Delegates(other.m_Delegates) {}

        multidelegate(const multidelegate &&other) noexcept : m_Delegates(std::move(other.m_Delegates)) {}

        virtual ~multidelegate() {
            m_Delegates.clear();
        }

        std::vector<Delegate> delegates() const {
            return m_Delegates;
        }

        void add(const Delegate &delegate) {
            m_Delegates.push_back(delegate);
        }

        void add(const Delegate &&delegate) {
            m_Delegates.push_back(std::move(delegate));
        }

        void remove(const Delegate &delegate) {
            m_Delegates.erase(std::remove(m_Delegates.begin(), m_Delegates.end(), delegate));
        }

        void remove(const Delegate &&delegate) {
            m_Delegates.erase(std::remove(m_Delegates.begin(), m_Delegates.end(), std::move(delegate)));
        }

        bool operator==(const multidelegate &other) const {
            return this == &other || m_Delegates == other.m_Delegates;
        }

        bool operator!=(const multidelegate &other) const {
            return !(*this == other);
        }

        void operator +=(const Delegate &delegate) {
            add(delegate);
        }

        void operator +=(const Delegate &&delegate) {
            add(std::move(delegate));
        }

        void operator -=(const Delegate &delegate) {
            remove(delegate);
        }

        void operator -=(const Delegate &&delegate) {
            remove(std::move(delegate));
        }

        multidelegate& operator=(const multidelegate &other) {
            if (this == &other) return *this;
            m_Delegates.clear();
            m_Delegates = std::vector<Delegate>(other.m_Delegates);
            return *this;
        }

        virtual void invoke(Args... args, std::vector<R> &resultOut) const {
            for (Delegate delegate : m_Delegates) {
                resultOut.push_back(delegate(args...));
            }
        }

        virtual void invoke(Args... args) const {
            for (Delegate delegate : m_Delegates) {
                delegate(args...);
            }
        }

        virtual void operator()(Args... args, std::vector<R> &resultOut) const {
            invoke(args..., resultOut);
        }

        virtual void operator()(Args... args) const {
            invoke(args...);
        }
    };
};

#endif //CWIRE_SEQUENCE_CALL_H
