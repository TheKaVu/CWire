#ifndef CWIRE_SEQUENCE_CALL_H
#define CWIRE_SEQUENCE_CALL_H

#include <algorithm>
#include <memory>
#include <vector>

#include "delegate.hpp"

namespace cwr {

    /// An ordered delegate container.
    /// @tparam R Return type.
    /// @tparam Args Argument types.
    template<typename R, typename ...Args>
    class multidelegate {

    public:

        /// Type of stored delegate
        using Delegate = delegate<R, Args...>;

    protected:
        std::vector<Delegate> m_Delegates;

    public:
        /// Creates empty multidelegate.
        multidelegate() : m_Delegates() {}

        /// Creates new multidelegate with a single delegate.
        /// @param delegate Single delegate to store.
        multidelegate(Delegate& delegate) {
            m_Delegates = {delegate};
        }

        /// Creates new multidelegate by copying other one.
        /// @param other Multidelegate to copy.
        multidelegate(const multidelegate &other) : m_Delegates(other.m_Delegates) {}

        /// Creates new multidelegate by moving other one.
        /// @param other Multidelegate to move.
        multidelegate(const multidelegate &&other) noexcept : m_Delegates(std::move(other.m_Delegates)) {}

        /// Destroys this multidelegate object.
        virtual ~multidelegate() {
            m_Delegates.clear();
        }

        /// Returns the collection of delegates stored in this multidelegate.
        /// @return Collection of stored delegates.
        std::vector<Delegate> delegates() const {
            return m_Delegates;
        }

        /// Adds new delegate.
        /// @param delegate Delegate to add.
        void add(const Delegate &delegate) {
            m_Delegates.push_back(delegate);
        }

        /// Adds new delegate.
        /// @param delegate Delegate to add.
        void add(const Delegate &&delegate) {
            m_Delegates.push_back(std::move(delegate));
        }

        /// Removes given delegate.
        /// @param delegate Delegate to remove.
        void remove(const Delegate &delegate) {
            m_Delegates.erase(std::remove(m_Delegates.begin(), m_Delegates.end(), delegate));
        }

        /// Removes given delegate.
        /// @param delegate Delegate to remove.
        void remove(const Delegate &&delegate) {
            m_Delegates.erase(std::remove(m_Delegates.begin(), m_Delegates.end(), std::move(delegate)));
        }

        /// Checks equality to given multidelegate.
        /// @param other Multidelegate to compare.
        /// @return @c true if multidelegates are equal, @c false otherwise.
        bool operator==(const multidelegate &other) const {
            return this == &other || m_Delegates == other.m_Delegates;
        }

        /// Checks inequality to given multidelegate.
        /// @param other Multidelegate to compare.
        /// @return @c true if multidelegates are not equal, @c false otherwise.
        bool operator!=(const multidelegate &other) const {
            return !(*this == other);
        }

        /// Adds new delegate.
        /// @param delegate Delegate to add.
        void operator +=(const Delegate &delegate) {
            add(delegate);
        }

        /// Adds new delegate.
        /// @param delegate Delegate to add.
        void operator +=(const Delegate &&delegate) {
            add(std::move(delegate));
        }

        /// Removes given delegate.
        /// @param delegate Delegate to remove.
        void operator -=(const Delegate &delegate) {
            remove(delegate);
        }

        /// Removes given delegate.
        /// @param delegate Delegate to remove.
        void operator -=(const Delegate &&delegate) {
            remove(std::move(delegate));
        }

        /// Copies given multidelegate to this one.
        /// @param other Multidelegate to copy.
        /// @return This multidelegate.
        multidelegate& operator=(const multidelegate &other) {
            if (this == &other) return *this;
            m_Delegates.clear();
            m_Delegates = std::vector<Delegate>(other.m_Delegates);
            return *this;
        }

        /// Invokes stored delegates in order writing results to passed vector.
        /// @param args Arguments to pass to the function.
        /// @param resultOut Vector to save results to.
        virtual void invoke(Args... args, std::vector<R> &resultOut) const {
            for (Delegate delegate : m_Delegates) {
                resultOut.push_back(delegate(args...));
            }
        }

        /// Invokes stored delegates in order writing results to passed vector.
        /// @param args Arguments to pass to the function.
        virtual void invoke(Args... args) const {
            for (Delegate delegate : m_Delegates) {
                delegate(args...);
            }
        }

        /// Invokes stored delegates in order writing results to passed vector.
        /// @param args Arguments to pass to the function.
        /// @param resultOut Vector to save results to.
        virtual void operator()(Args... args, std::vector<R> &resultOut) const {
            invoke(args..., resultOut);
        }

        /// Invokes stored delegates in order writing results to passed vector.
        /// @param args Arguments to pass to the function.
        virtual void operator()(Args... args) const {
            invoke(args...);
        }
    };
};

#endif //CWIRE_SEQUENCE_CALL_H
