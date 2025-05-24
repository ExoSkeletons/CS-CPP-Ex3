//
// Created by Aviad Levine on 18/05/2025.
//

#ifndef TYPETOOLS_HPP
#define TYPETOOLS_HPP

#include <algorithm>

template<typename Base, typename T>
bool instanceof(const T *ptr) { return dynamic_cast<const Base *>(ptr) != nullptr; }

template<typename T>
bool removeValue(std::vector<T> &vec, const T value) {
    typename std::vector<T>::iterator position = std::find(vec.begin(), vec.end(), value);

    // == myVector.end() means the element was not found
    if (position == vec.end()) return false;

    vec.erase(position);
    return true;
}

#endif //TYPETOOLS_HPP
