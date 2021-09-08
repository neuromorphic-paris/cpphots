/**
 * @file clonable.h
 * @brief Interface for clonable objects
 */
#ifndef CPPHOTS_INTERFACES_CLONABLE_H
#define CPPHOTS_INTERFACES_CLONABLE_H


namespace cpphots{

namespace interfaces {

/**
 * @brief Class declaring the clonable interface
 * 
 * Other interface should subclass from this with CRTP.
 * 
 * @tparam Base class to which the interface should be added
 */
template <class Base>
struct ClonableBase {

    /**
     * @brief Clone the object
     * 
     * @return pointer to the new object
     */
    virtual Base* clone() const = 0;

};

/**
 * @brief Class implementing the clonable interface
 * 
 * Concrete classes should derive from this.
 * 
 * @tparam Derived concrete class
 * @tparam Base interface class
 */
template <class Derived, class Base>
struct Clonable : public virtual Base {

    /**
     * @brief Clone the object
     * 
     * @return pointer to the new object
     */
    Base* clone() const override {
        return new Derived(static_cast<Derived const&>(*this));
    }

    using Base::Base;

};

}

}

# endif