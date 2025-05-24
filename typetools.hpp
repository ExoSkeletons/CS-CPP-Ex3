//
// Created by Aviad Levine on 18/05/2025.
//

#ifndef TYPETOOLS_HPP
#define TYPETOOLS_HPP

template<typename Base, typename T>
bool instanceof(const T *ptr) {
    return dynamic_cast<const Base*>(ptr) != nullptr;
}

#endif //TYPETOOLS_HPP
