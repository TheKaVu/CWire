#ifndef SAMPLES_DELEGATE_H
#define SAMPLES_DELEGATE_H
#include <algorithm>

namespace cwr {

    template<typename R, typename ...Args>
    class delegate{

        using ContextFunc_t = R(*)(void*, Args...);

        ContextFunc_t m_Function = nullptr;
        void* m_Context = nullptr;

        delegate(const ContextFunc_t function, void* context) : m_Function(function), m_Context(context) {}

    public:
        delegate() = default;

        delegate(const delegate& other) {
            m_Function = other.m_Function;
        }

        delegate(const delegate&& other) noexcept {
            m_Function = std::move(other.m_Function);
        }

        virtual ~delegate(){
            m_Function = nullptr;
            m_Context = nullptr;
        }

        template<R(*Func)(Args...)>
        void set() {
            m_Context = nullptr;
            m_Function = [](void*, Args... args) {return Func(std::forward<Args>(args)...); };
        }

        template<typename T, R(T::*Func)(Args...)>
        void set(T& context) {
            m_Context = context;
            m_Function = [](void* ctx, Args... args) {
                return (static_cast<T*>(ctx)->*Func)(std::forward<Args>(args)...);
            };
        }

        R invoke(Args... args) {
            return (*m_Function)(m_Context, args...);
        }

        R operator ()(Args... args) {
            return invoke(std::forward<Args>(args)...);
        }

        constexpr delegate& operator =(const delegate& other) = default;

        constexpr delegate& operator =(delegate&& other) noexcept {
            m_Function = std::move(other.m_Function);
            return *this;
        }

        bool operator ==(const delegate& handler) const {
            return m_Function == handler.m_Function;
        }

        template<typename OtherR, typename ...OtherArgs, typename Other = delegate<OtherR, OtherArgs...>>
        operator delegate<OtherR, OtherArgs...>() const {
            return Other(reinterpret_cast<typename Other::ContextFunc_t>(m_Function));
        }

        template<R(*Func)(Args...)>
        static delegate of() {
            return delegate([](void*, Args... args) {return Func(std::forward<Args>(args)...); }, nullptr);
        }

        template<typename T, R(T::*Func)(Args...)>
        static delegate of(T& context) {
            return delegate([](void* ctx, Args... args) {
                return (static_cast<T*>(ctx)->*Func)(std::forward<Args>(args)...);
            }, &context);
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
