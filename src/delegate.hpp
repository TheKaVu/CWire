#ifndef SAMPLES_DELEGATE_H
#define SAMPLES_DELEGATE_H
#include <algorithm>

namespace cwr {

    template<typename R, typename ...Args>
    class delegate{

        using ContextFunc_t = R(*)(void*, Args...);

        ContextFunc_t m_Function = nullptr;
        void* m_Context = nullptr;
        uintptr_t m_MemRef = 0;

        delegate(const ContextFunc_t function, void* context, const uintptr_t memRef) : m_Function(function), m_Context(context), m_MemRef(memRef) {}

    public:
        delegate() = default;

        delegate(const delegate& other) {
            m_Function = other.m_Function;
            m_Context = other.m_Context;
            m_MemRef = other.m_MemRef;
        }

        delegate(delegate&& other) noexcept {
            m_Function = std::move(other.m_Function);
            m_Context = other.m_Context;
            other.m_Context = nullptr;
            m_MemRef = other.m_MemRef;
            other.m_MemRef = 0;
        }

        virtual ~delegate(){
            m_Function = nullptr;
            m_Context = nullptr;
            m_MemRef = 0;
        }

        template<R(*Func)(Args...)>
        void set() {
            m_Context = nullptr;
            m_Function = [](void*, Args... args) {return Func(std::forward<Args>(args)...); };
            m_MemRef = reinterpret_cast<uintptr_t>(Func);
        }

        template<typename T, R(T::*Func)(Args...)>
        void set(const T& context) {
            m_Context = context;
            m_Function = [](void* ctx, Args... args) {
                return (static_cast<T*>(ctx)->*Func)(std::forward<Args>(args)...);
            };
            m_MemRef = reinterpret_cast<uintptr_t>(Func);
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
            other.m_MemRef = 0;
            return *this;
        }

        bool operator ==(const delegate& other) const {
            return m_Context == other.m_Context && m_MemRef == other.m_MemRef;
        }

        template<typename OtherR, typename ...OtherArgs, typename Other = delegate<OtherR, OtherArgs...>>
        operator delegate<OtherR, OtherArgs...>() const {
            return Other(reinterpret_cast<Other::ContextFunc_t>(m_Function), m_Context, m_MemRef);
        }

        template<R(*Func)(Args...)>
        static delegate of() {
            return delegate([](void*, Args... args) {return Func(std::forward<Args>(args)...); }, nullptr, reinterpret_cast<uintptr_t>(Func));
        }

        template<typename T, R(T::*Func)(Args...)>
        static delegate of(const T& context) {
            return delegate([](void* ctx, Args... args) {
                return (static_cast<T*>(ctx)->*Func)(std::forward<Args>(args)...);
            }, &context, reinterpret_cast<uintptr_t>(Func));
        }
    };

    template<typename R>
    using supplier = delegate<R>;

    template<typename ...Args>
    using consumer = delegate<void, Args...>;

    template<typename ...Args>
    using predicate = delegate<bool, Args...>;

    template<typename K, typename V>
    using mapper = delegate<K, V>;
}


#endif //SAMPLES_DELEGATE_H
