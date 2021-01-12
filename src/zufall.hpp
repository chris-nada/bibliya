#pragma once

#include <random>
#include <chrono>
#include <unordered_map>

namespace Zufall {

    /**
     * Thread-sichere Methode zum Liefern von Zufallszahlen beliebigen Typs.
     * @example Folgendes Beispiel liefert die Zahlen von 1 bis 6 wie auf einem Spielwürfel:
     *          `get<unsigned>(1, 6)`
     */
    template<typename T>
    T get(T a, T b) {
        thread_local std::random_device rd;
        thread_local std::mt19937 mt(rd());
        std::uniform_int_distribution<T> uid(a, b);
        return uid(mt);
    }

};
