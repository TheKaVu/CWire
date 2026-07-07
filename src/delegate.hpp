#ifndef SAMPLES_DELEGATE_H
#define SAMPLES_DELEGATE_H
#include <algorithm>
#include <cstring>

namespace cwr {

    /// A sophisticated polymorphic function wrapper.
    /// @tparam R Return type.
    /// @tparam Args Argument types.
    template<typename R, typename ...Args>
    class delegate{

        template<typename AnyR, typename ...AnyArgs>
        friend class delegate;

        using ContextFunc_t = R(*)(void*, Args...);

        ContextFunc_t m_Function = nullptr;
        void* m_Context = nullptr;
        alignas(void*) unsigned char* m_MemRef = nullptr;

        /// Creates new delegate with initialization values. Callable only inside the class.
        /// @param function Wrapped function call.
        /// @param context Object to call function on.
        /// @param memRef Raw memory data of wrapped function.
        delegate(const ContextFunc_t function, void* context, unsigned char* memRef) : m_Function(function), m_Context(context), m_MemRef(memRef) {}

    public:
        /// Creates empty delegate.
        delegate() = default;

        /// Creates new delegate by copying other one.
        /// @param other Delegate to copy.
        delegate(const delegate& other) {
            m_Function = other.m_Function;
            m_Context = other.m_Context;
            m_MemRef = new unsigned char[sizeof(other.m_MemRef)];
            std::memcpy(m_MemRef, other.m_MemRef, sizeof(other.m_MemRef));
        }

        /// Moves given delegate.
        /// @param other Delegate to move.
        delegate(delegate&& other) noexcept {
            m_Function = std::move(other.m_Function);
            m_Context = other.m_Context;
            other.m_Context = nullptr;
            m_MemRef = other.m_MemRef;
            other.m_MemRef = nullptr;
        }

        /// Destroys this delegate object.
        ~delegate(){
            m_Function = nullptr;
            m_Context = nullptr;
            delete[] m_MemRef;
            m_MemRef = nullptr;
        }

        /// Assigns global or static function to this delegate.
        /// @tparam Func Function to assign.
        template<R(*Func)(Args...)>
        void set() {
            m_Context = nullptr;
            m_Function = [](void*, Args... args) {return Func(std::forward<Args>(args)...); };
            delete[] m_MemRef;
            auto funcPtr = Func;
            m_MemRef = new unsigned char[sizeof(funcPtr)];
            std::memcpy(m_MemRef, &funcPtr, sizeof(funcPtr));
        }

        /// Assigns member function to this delegate.
        /// @tparam T Parent type the function is declared in.
        /// @tparam Func Function to assign.
        /// @param context Parent object to call the function on.
        template<typename T, R(T::*Func)(Args...)>
        void set(T& context) {
            m_Context = &context;
            m_Function = [](void* ctx, Args... args) {
                return (static_cast<T*>(ctx)->*Func)(std::forward<Args>(args)...);
            };
            delete[] m_MemRef;
            auto funcPtr = Func;
            m_MemRef = new unsigned char[sizeof(funcPtr)];
            std::memcpy(m_MemRef, &funcPtr, sizeof(funcPtr));
        }

        /// Invokes wrapped function with given arguments.
        /// @param args Arguments to pass to the function.
        /// @return Result of the function call.
        R invoke(Args... args) const {
            return (*m_Function)(m_Context, args...);
        }

        /// Invokes wrapped function with given arguments.
        /// @param args Arguments to pass to the function.
        /// @return Result of the function call.
        R operator ()(Args... args) const {
            return invoke(std::forward<Args>(args)...);
        }

        /// Copies given delegate to this one.
        /// @param other Delegate to copy.
        /// @return This delegate.
        constexpr delegate& operator =(const delegate& other) {
            if (&other == this) return *this;
            m_Function = other.m_Function;
            m_Context = other.m_Context;
            m_MemRef = new unsigned char[sizeof(other.m_MemRef)];
            std::memcpy(m_MemRef, other.m_MemRef, sizeof(other.m_MemRef));
            return *this;
        }

        /// Moves given delegate to this one.
        /// @param other Delegate to move.
        /// @return This delegate.
        constexpr delegate& operator =(delegate&& other) noexcept {
            m_Function = std::move(other.m_Function);
            m_Context = other.m_Context;
            other.m_Context = nullptr;
            m_MemRef = other.m_MemRef;
            other.m_MemRef = nullptr;
            return *this;
        }

        /// Checks equality to given delegate.
        /// @param other Delegate to compare.
        /// @return @c true if delegates are equal, @c false otherwise.
        bool operator ==(const delegate& other) const {
            return this == &other || (
                m_Context == other.m_Context && std::memcmp(m_MemRef, other.m_MemRef, sizeof(m_MemRef)) == 0);
        }

        /// Checks inequality to given delegate.
        /// @param other Delegate to compare.
        /// @return @c true if delegates are not equal, @c false otherwise.
        bool operator!=(const delegate &other) const {
            return !(*this == other);
        }

        template<typename OtherR, typename ...OtherArgs, typename Other = delegate<OtherR, OtherArgs...>>
        operator delegate<OtherR, OtherArgs...>() const {
            return Other(reinterpret_cast<Other::ContextFunc_t>(m_Function), m_Context, m_MemRef);
        }

        /// Creates new delegate wrapping the given global or static function.
        /// @tparam Func Function to wrap.
        /// @return A delegate wrapping the given function.
        template<R(*Func)(Args...)>
        static delegate of() {
            auto funcPtr = Func;
            auto buffer = new unsigned char[sizeof(funcPtr)];
            std::memcpy(buffer, &funcPtr, sizeof(funcPtr));
            return delegate(
                [](void*, Args... args) {return Func(std::forward<Args>(args)...); },
                nullptr,
                buffer
                );
        }

        /// Creates new delegate wrapping the given member function.
        /// @tparam T Parent type the function is declared in.
        /// @tparam Func Function to wrap.
        /// @param context Parent object to call the function on.
        /// @return A delegate wrapping the given function.
        template<typename T, R(T::*Func)(Args...)>
        static delegate of(T& context) {
            auto funcPtr = Func;
            auto buffer = new unsigned char[sizeof(funcPtr)];
            std::memcpy(buffer, &funcPtr, sizeof(funcPtr));
            return delegate(
                [](void* ctx, Args... args) {return (static_cast<T*>(ctx)->*Func)(std::forward<Args>(args)...);},
                &context,
                buffer);
        }
    };

    /// Accepts no arguments and returns a value.
    /// @tparam R Return type.
    template<typename R>
    using supplier = delegate<R>;

    /// Accepts arguments and returns nothing.
    /// @tparam Args Argument types.
    template<typename ...Args>
    using consumer = delegate<void, Args...>;

    /// Accepts arguments and returns @code bool@endcode.
    /// @tparam Args Argument types.
    template<typename ...Args>
    using predicate = delegate<bool, Args...>;

    /// Accepts one argument and returns a value.
    /// @tparam K Argument type.
    /// @tparam V Return type.
    template<typename V, typename K>
    using mapper = delegate<V, K>;
}


#endif //SAMPLES_DELEGATE_H
