#ifndef CWIRE_PROPERTY_H
#define CWIRE_PROPERTY_H
#include <algorithm>

#include "delegate.hpp"
#include "event.hpp"

#endif //CWIRE_PROPERTY_H

namespace cwr {

    /// Single-value container watching its content change.
    /// @tparam T Type of stored value
    template <typename T>
    class property {

        T m_Value;
        EVENT(ValueChanged, class property, T, T);

    public:
        using ValueChangedEventHandler = delegate<void, T>;

        /// Creates new property with given value.
        /// @param value Value of the property.
        property(T value = {}) : m_Value(value) {
            m_Value = value;
        }

        /// Creates new property by copying other one.
        /// @param p Property to copy.
        property(const property &p) : m_Value(p.m_Value) {}

        /// Creates new property by moving other one.
        /// @param p Property to move.
        property(property &&p) noexcept {
            m_Value = std::move(p.m_Value);
        }

        /// Gets the value of this property.
        /// @return Property value.
        T get() const {
            return m_Value;
        }

        /// Sets the value of this property.
        /// @param value New value.
        void set(T value) {
            onValueChanged(m_Value, value);
            m_Value = value;
        }

        /// Sets the value of this property.
        /// @param value New value.
        property& operator=(T value) {
            set(value);
            return *this;
        }

        /// Copies given property to this one.
        /// @param p Property to copy.
        /// @return This property.
        property& operator=(const property &p) {
            set(p.m_Value);
            return *this;
        }

        /// Moves given property to this one.
        /// @param p Property to move.
        /// @return This property.
        property& operator=(property &&p) noexcept {
            set(std::move(p.m_Value));
            return *this;
        }

        /// Checks equality to given property.
        /// @param other Property to compare.
        /// @return @c true if properties are equal, @c false otherwise.
        bool operator==(const property &other) const {
            return m_Value == other.m_Value;
        }

        /// Checks inequality to given property.
        /// @param other Property to compare.
        /// @return @c true if properties are not equal, @c false otherwise.
        bool operator!=(const property &other) const {
            return m_Value != other.m_Value;
        }

        /// Compares this property to another one.
        /// @param p Property to compare.
        /// @return @c true if this property is "lesser", @c false otherwise.
        bool operator<(const property &p) const {
            return m_Value < p.m_Value;
        }

        /// Unpacks this property.
        operator T() const{
            return m_Value;
        }
    };
}
