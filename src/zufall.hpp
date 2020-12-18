#pragma once

#include <random>
#include <chrono>
#include <unordered_map>

/// Methoden zum Liefern von Zufallszahlen.
namespace Zufall {

    template<typename T>
    T get(T a, T b) {
        thread_local std::random_device rd;
        thread_local std::mt19937 mt(rd());
        std::uniform_int_distribution<T> uid(a, b);
        return uid(mt);
    }

};
