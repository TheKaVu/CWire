#ifndef CWIRE_EVENT_H
#define CWIRE_EVENT_H

#include "multidelegate.hpp"

namespace cwr {

    /// Base class for an event.
    /// @tparam Args Argument types.
    template <typename ...Args>
    class event : public multidelegate<void, Args...>{

    protected:
        ~event() override{}

        void invoke(Args... args) const {
            multidelegate<void, Args...>::invoke(args...);
        }

        void operator()(Args... args) const {
            multidelegate<void, Args...>::operator()(args...);
        }
    };
}

/// Creates an event, special void-returning multidelegate callable only inside the class it's declared in.
/// @param name Name of the event.
/// @param parent Parent class.
/// @param args_t Argument types.
#define EVENT(name, parent, args_t...) protected:\
class event_##name : public event<args_t> {\
friend parent;\
~event_##name() {} \
};\
public:\
event_##name on##name;

#endif //CWIRE_EVENT_H
