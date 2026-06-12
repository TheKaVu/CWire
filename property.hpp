#ifndef CWIRE_PROPERTY_H
#define CWIRE_PROPERTY_H
#include <algorithm>

#include "event.hpp"

#endif //CWIRE_PROPERTY_H

namespace cwr {
    template <typename T>
    class property {

        T m_Value;
        EVENT(ValueChanged, class property, T, T);

    public:
        using ValueChangedEventHandler = delegate<void, T>;

        property() : m_Value() {}

        property(T value) : m_Value(value) {
            m_Value = value;
        }
        property(const property &p) : m_Value(p.m_Value) {}
        property(property &&p) noexcept {
            m_Value = std::move(p.m_Value);
        }

        T get() const {
            return m_Value;
        }

        void set(T value) {
            onValueChanged(m_Value, value);
            m_Value = value;
        }

        property& operator=(T value) {
            set(value);
            return *this;
        }

        property& operator=(const property &p) {
            set(p.m_Value);
            return *this;
        }

        property& operator=(property &&p) noexcept {
            set(std::move(p.m_Value));
            return *this;
        }

        bool operator==(const property &p) const {
            return m_Value == p.m_Value;
        }

        bool operator!=(const property &p) const {
            return m_Value != p.m_Value;
        }

        bool operator<(const property &p) const {
            return m_Value < p.m_Value;
        }

        operator T() const{
            return m_Value;
        }
    };
}
