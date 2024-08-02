#pragma once

#include <vector>
#include <functional>
#include <ostream>

template <typename T>
class List : public std::vector<T> {
public:

    List() : std::vector<T>() { }

    List(const List& other) : std::vector<T>(other) { }

    List(const std::initializer_list<T> l) : std::vector<T>(l) { }

    explicit List(size_t size) : std::vector<T>(size) { }

    List(size_t size, T initialValue) : std::vector<T>(size, initialValue) { }

    List(typename std::vector<T>::iterator first, typename std::vector<T>::iterator last) : std::vector<T>(first, last) { }

    List& operator=(const List& other) {
        if (this != &other) {
            std::vector<T>::operator=(other);
        }
        return *this;
    }

    size_t append(const T& element) {
        this->push_back(element);
        return this->size() - 1;
    }

    void remove(const T& element) {
        auto it = std::find(this->begin(), this->end(), element);
        this->erase(it);
    }

    void remove(size_t index, size_t number = 1) {
        if (number == 0) return;
        jassert(index >= 0 && index < this->size() && number > 0);
        jassert(index + number <= this->size()); // Remove more elements than there are in the List
        this->erase(this->begin() + static_cast<long>(index), this->begin() + static_cast<long>(index + number));
    }

    void append(const std::vector<T> &other) {
        this->insert(this->end(), other.begin(), other.end());
    }

    T sum() {
        if (this->empty())
            return T();
        T res = this->at(0);
        for (size_t i = 1; i < this->size(); ++i) {
            res += this->at(i);
        }
        return res;
    }

    bool all(const std::function<bool(T)> condition) {
        for (T& element : this) {
            if (!condition(element)) return false;
        }
        return true;
    }

    bool any(const std::function<bool(T)> condition) {
        for (T& element : this) {
            if (condition(element)) return true;
        }
        return false;
    }

    template<class U>
    List<U> map(std::function<U(T)> mapper) const {
        List<U> copiedList(this->size());

        for (size_t i = 0; i < this->size(); ++i) {
            copiedList[i] = mapper(this->at(i));
        }

        return copiedList;
    }

    void mapMutable(std::function<T(T)> mapper) {
        for (size_t i = 0; i < this->size(); ++i) {
            this->at(i) = mapper(this->at(i));
        }
    }

    // Returns size if element isn't contained
    size_t indexOf(const T &element) {
        auto iterator = find(this->begin(), this->end(), element);
        return static_cast<size_t>(iterator - this->begin());
    }

    void forEach(std::function<void(T)> consumer) {
        for (size_t i = 0; i < this->size(); ++i) {
            consumer(this->at(i));
        }
    }

    void forEachIndexed(std::function<void(size_t, T)> consumer) {
        for (size_t i = 0; i < this->size(); ++i) {
            consumer(i, this->at(i));
        }
    }


    juce::StringArray toStringArray() {
        juce::StringArray stringArray;
        for (size_t i = 0; i < this->size(); i++) {
            stringArray.add(juce::String(this->at(i)));
        }
        return stringArray;
    }


    // cout overload
    friend std::ostream& operator<<(std::ostream& os, const List& list) {
        os << "[";
        for (size_t i = 0; i < list.size(); i++) {
            os << list[i];
            if (i < list.size() - 1)
                os << ", ";
        }
        os << "]";
        return os;
    }

};
