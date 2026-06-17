#ifndef SAMPLES_DELEGATE_H
#define SAMPLES_DELEGATE_H
#include <algorithm>
#include <cstring>

namespace cwr {

    template<typename R, typename ...Args>
    class delegate{

        template<typename AnyR, typename ...AnyArgs>
        friend class delegate;

        using ContextFunc_t = R(*)(void*, Args...);

        ContextFunc_t m_Function = nullptr;
        void* m_Context = nullptr;
        alignas(void*) unsigned char* m_MemRef = nullptr;

        delegate(const ContextFunc_t function, void* context, unsigned char* memRef) : m_Function(function), m_Context(context), m_MemRef(memRef) {}

    public:
        delegate() = default;

        delegate(const delegate& other) {
            m_Function = other.m_Function;
            m_Context = other.m_Context;
            m_MemRef = new unsigned char[sizeof(other.m_MemRef)];
            std::memcpy(m_MemRef, other.m_MemRef, sizeof(other.m_MemRef));
        }

        delegate(delegate&& other) noexcept {
            m_Function = std::move(other.m_Function);
            m_Context = other.m_Context;
            other.m_Context = nullptr;
            m_MemRef = other.m_MemRef;
            other.m_MemRef = nullptr;
        }

        ~delegate(){
            m_Function = nullptr;
            m_Context = nullptr;
            m_MemRef = nullptr;
        }

        template<R(*Func)(Args...)>
        void set() {
            m_Context = nullptr;
            m_Function = [](void*, Args... args) {return Func(std::forward<Args>(args)...); };
            delete[] m_MemRef;
            auto funcPtr = Func;
            m_MemRef = new unsigned char[sizeof(funcPtr)];
            std::memcpy(m_MemRef, &funcPtr, sizeof(funcPtr));
        }

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

        R invoke(Args... args) const {
            return (*m_Function)(m_Context, args...);
        }

        R operator ()(Args... args) const {
            return invoke(std::forward<Args>(args)...);
        }

        constexpr delegate& operator =(const delegate& other) = default;

        constexpr delegate& operator =(delegate&& other) noexcept {
            m_Function = std::move(other.m_Function);
            m_Context = other.m_Context;
            other.m_Context = nullptr;
            m_MemRef = other.m_MemRef;
            other.m_MemRef = nullptr;
            return *this;
        }

        bool operator ==(const delegate& other) const {
            return m_Context == other.m_Context &&
                std::memcmp(m_MemRef, other.m_MemRef, sizeof(m_MemRef)) == 0;
        }

        template<typename OtherR, typename ...OtherArgs, typename Other = delegate<OtherR, OtherArgs...>>
        operator delegate<OtherR, OtherArgs...>() const {
            return Other(reinterpret_cast<Other::ContextFunc_t>(m_Function), m_Context, m_MemRef);
        }

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

    template<typename R>
    using supplier = delegate<R>;

    template<typename ...Args>
    using consumer = delegate<void, Args...>;

    template<typename ...Args>
    using predicate = delegate<bool, Args...>;

    template<typename V, typename K>
    using mapper = delegate<V, K>;
}


#endif //SAMPLES_DELEGATE_H
