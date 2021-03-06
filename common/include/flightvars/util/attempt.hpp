/*
 * FlightVars
 * Copyright (c) 2014 Alvaro Polo
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef FLIGHTVARS_UTIL_ATTEMPT_H
#define FLIGHTVARS_UTIL_ATTEMPT_H

#include <exception>
#include <memory>

#include <flightvars/util/exception.hpp>
#include <flightvars/util/either.hpp>
#include <flightvars/util/option.hpp>

namespace flightvars { namespace util {

FV_DECL_EXCEPTION(attempt_error);

/** 
 * An attempt to produce a value of class T. 
 * 
 * This class wraps either a value of class T, or an error encapsulated in a
 * std::exception_ptr value. It may be used to represent an attempt to perform
 * a computation.
 */
template <class T>
class attempt {
public:

    /** Create a successful attempt. */
    attempt(const T& value) : _result(value) {}

    /** Create a successful attempt. */
    attempt(T&& value) : _result(std::move(value)) {}

    /** Create a failure attempt. */
    attempt(const std::exception_ptr& error) : _result(error) {}

    /** Create a failure attempt. */
    template <class E>
    attempt(const E& error) : _result(std::make_exception_ptr(error)) {}

    attempt(const attempt& other) = default;
    attempt(attempt&& other) = default;

    attempt& operator = (const attempt& other) = default;
    attempt& operator = (attempt&& other) = default;

    /** True if this attempt has a state, false otherwise. */
    bool valid() const { return _result.valid(); }

    /** Return true if the attempt was successful, false otherwise. */
    bool is_success() const { return _result.has_left(); }

    /** Return true of the attempt has failed, false otherwise. */
    bool is_failure() const { return _result.has_right(); }

    /** Return the computed value if success, or throw the error otherwise. */
    const T& get() const {
        throw_if_not_success();
        return _result.left();
    }

    /** Return the computed value as an option. */
    option<T> get_opt() const {
        return is_success() ? make_some(_result.left()) : make_none<T>();
    }

private:

    either<T, std::exception_ptr> _result;

    void throw_if_not_success() const {
        if (_result.has_right()) {
            std::rethrow_exception(_result.right());
        } else if (!_result.has_left()) {
            throw attempt_error("this attempt is not defined");
        }
    }
};

template <>
class attempt<void> {
public:

    /** Create a successful attempt. */
    attempt(bool is_success = true) {
        if (is_success) { _result.reset(nullptr); }
    }

    /** Create a failure attempt. */
    attempt(const std::exception_ptr& error) : _result(error) {}

    /** Create a failure attempt. */
    template <class E>
    attempt(const E& error) : _result(std::make_exception_ptr(error)) {}

    attempt(const attempt& other) = default;
    attempt(attempt&& other) = default;

    attempt& operator = (const attempt& other) = default;
    attempt& operator = (attempt&& other) = default;

    /** Return true if the attempt was successful, false otherwise. */
    bool is_success() const { return _result.has_left(); }

    /** Return true of the attempt has failed, false otherwise. */
    bool is_failure() const { return _result.has_right(); }

    /** Do nothing if success, or throw the error otherwise. */
    void get() const {
        throw_if_not_success();
    }

    /** Return the computed value as an option. */
    option<void> get_opt() const {
        return is_success() ? make_some() : make_none<void>();
    }

private:

    either<std::nullptr_t, std::exception_ptr> _result;

    void throw_if_not_success() const {
        if (_result.has_right()) {
            std::rethrow_exception(_result.right());
        } else if (!_result.has_left()) {
            throw attempt_error("this attempt is not defined");
        }
    }
};

template <class T>
typename std::enable_if<!std::is_void<T>::value, attempt<T>>::type
make_success(const T& value) {
    return attempt<T>(value); 
}

template <class T>
typename std::enable_if<std::is_void<T>::value, attempt<void>>::type
make_success() {
    return attempt<void>(true); 
}

template <class T>
attempt<T> make_failure(const std::exception_ptr& error) {
    return attempt<T>(error);
}

template <class T, class E>
attempt<T> make_failure(const E& error) {
    return attempt<T>(error);
}

}}

#endif
