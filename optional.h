#pragma once

#include <new>
#include <type_traits>
#include <utility>

template <typename T>
class Optional {
public:
    // ======================== Constructors ========================

    Optional();
    Optional(const T& value);
    Optional(T&& value);

    // =================== Copy and Move ===========================

    Optional(const Optional& other);
    Optional(Optional&& other);
    Optional& operator=(const Optional& other);
    Optional& operator=(Optional&& other);
    Optional& operator=(const T& value);
    Optional& operator=(T&& value);

    // =================== Destructor ==============================

    ~Optional();

    // =================== Observers ===============================

    bool has_value() const;
    explicit operator bool() const;

    T&       value();
    const T& value() const;

    T        value_or(const T& default_value) const;

    T&       operator*();
    const T& operator*() const;

    T*       operator->();
    const T* operator->() const;

    // =================== Modifiers ===============================

    template <typename... Args>
    T& emplace(Args&&... args);

    void reset();
    void swap(Optional& other);

    // =================== Comparison ==============================

    bool operator==(const Optional& rhs) const;
    bool operator!=(const Optional& rhs) const;

private:
    alignas(T) unsigned char storage_[sizeof(T)];
    bool has_value_ = false;
    T* ptr();
    const T* ptr() const;
};


template <typename T>
T* Optional<T>::ptr() {
    return reinterpret_cast<T*>(storage_);
}

template <typename T>
const T* Optional<T>::ptr() const {
    return reinterpret_cast<const T*>(storage_);
}



template <typename T>
Optional<T>::Optional() : has_value_(false) {}

template <typename T>
Optional<T>::Optional(const T& value) : has_value_(true) {
    new (ptr()) T(value);
}

template <typename T>
Optional<T>::Optional(T&& value) : has_value_(true) {
    new (ptr()) T(std::move(value));
}

template <typename T>
Optional<T>::Optional(const Optional& other) : has_value_(other.has_value_) {
    if (has_value_) {
        new (ptr()) T(*other.ptr());
    }
}

template <typename T>
Optional<T>::Optional(Optional&& other) : has_value_(other.has_value_) {
    if (has_value_) {
        new (ptr()) T(std::move(*other.ptr()));
        other.reset();
    }
}



template <typename T>
Optional<T>& Optional<T>::operator=(const Optional& other) {
    if (this != &other) {
        reset();
        has_value_ = other.has_value_;
        if (has_value_) {
            new (ptr()) T(*other.ptr());
        }
    }
    return *this;
}

template <typename T>
Optional<T>& Optional<T>::operator=(Optional&& other) {
    if (this != &other) {
        reset();
        has_value_ = other.has_value_;
        if (has_value_) {
            new (ptr()) T(std::move(*other.ptr()));
            other.reset();
        }
    }
    return *this;
}

template <typename T>
Optional<T>& Optional<T>::operator=(const T& value) {
    reset();
    has_value_ = true;
    new (ptr()) T(value);
    return *this;
}

template <typename T>
Optional<T>& Optional<T>::operator=(T&& value) {
    reset();
    has_value_ = true;
    new (ptr()) T(std::move(value));
    return *this;
}



template <typename T>
Optional<T>::~Optional() {
    reset();
}



template <typename T>
bool Optional<T>::has_value() const {
    return has_value_;
}

template <typename T>
Optional<T>::operator bool() const {
    return has_value_;
}

template <typename T>
T& Optional<T>::value() {
    return *ptr();
}

template <typename T>
const T& Optional<T>::value() const {
    return *ptr();
}

template <typename T>
T Optional<T>::value_or(const T& default_value) const {
    return has_value_ ? *ptr() : default_value;
}

template <typename T>
T& Optional<T>::operator*() {
    return *ptr();
}

template <typename T>
const T& Optional<T>::operator*() const {
    return *ptr();
}

template <typename T>
T* Optional<T>::operator->() {
    return ptr();
}

template <typename T>
const T* Optional<T>::operator->() const {
    return ptr();
}



template <typename T>
template <typename... Args>
T& Optional<T>::emplace(Args&&... args) {
    reset();
    has_value_ = true;
    new (ptr()) T(std::forward<Args>(args)...);
    return *ptr();
}

template <typename T>
void Optional<T>::reset() {
    if (has_value_) {
        ptr()->~T();
        has_value_ = false;
    }
}

template <typename T>
void Optional<T>::swap(Optional& other){
    if (has_value_ && other.has_value_){
        T tmp(std::move(*ptr()));
        *ptr() = std::move(*other.ptr());
        *other.ptr() = std::move(tmp);
    } else if (has_value_){
        new (other.storage_) T(std::move(*ptr()));
        other.has_value_ = true;
        reset();
    } else if (other.has_value_){
        new (storage_) T(std::move(*other.ptr()));
        has_value_ = true;
        other.reset();
    }
}



template <typename T>
bool Optional<T>::operator==(const Optional& rhs) const {
    if (has_value_ != rhs.has_value_) return false;
    if (!has_value_) return true;
    return *ptr() == *rhs.ptr();
}

template <typename T>
bool Optional<T>::operator!=(const Optional& rhs) const {
    return !(*this == rhs);
}
