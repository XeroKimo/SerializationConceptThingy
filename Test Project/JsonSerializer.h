/*
MIT License

Copyright (c) 2021 Renzy Alarcon

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

*/
#pragma once
#include "../Single Include/Serializer.h"
#include "json.hpp"
#include <fstream>

template<class T>
inline constexpr bool is_arithmetic_value = std::is_arithmetic_v<T> && !std::is_pointer_v<T>;

template<class T>
inline constexpr bool is_arithmetic_pointer = std::is_arithmetic_v<std::remove_pointer_t<T>> && std::is_pointer_v<T>;

template<class T>
inline constexpr bool is_class_value = std::is_class_v<T> && !std::is_pointer_v<T>;

template<class T>
inline constexpr bool is_class_pointer = std::is_class_v<std::remove_pointer_t<T>> && std::is_pointer_v<T>;


//Serializer concept
class JsonSerializer
{
private:
    nlohmann::json json{};
    std::vector<std::string_view> tree;


public:
    //The following functions must exist unless specified

    template<class T, std::enable_if_t<is_arithmetic_value<T>, bool> = true>
    void Serialize(std::string_view name, const T value)
    {
        //For built in types
        JsonReference(name) = value;
    }

    template<class T, std::enable_if_t<is_arithmetic_value<T>, bool> = true>
    T Deserialize(std::string_view name)
    {
        //For built in types
        return JsonReference(name);
    }

    template<class T, std::enable_if_t<is_arithmetic_pointer<T>, bool> = true>
    void Serialize(std::string_view name, const T value)
    {
        if(value == nullptr)
        {
            JsonReference(name) = nullptr;
        }
        else
        {
            JsonReference(name) = *value;
        }
    }

    template<class T, std::enable_if_t<is_arithmetic_pointer<T>, bool> = true>
    T Deserialize(std::string_view name)
    {
        using value_type = std::remove_pointer_t<T>;
        auto ref = JsonReference(name);

        if(ref.is_null())
            return nullptr;
        //For built in types
        return new value_type(ref);
    }

    template<class T, std::enable_if_t<is_class_value<T>, bool> = true>
    void Serialize(std::string_view name, const T& value)
    {
        tree.push_back(name);

        SerializeConstruct<T, JsonSerializer>::Serialize(*this, value);

        tree.pop_back();
    }

    template<class T, std::enable_if_t<is_class_value<T>, bool> = true>
    T Deserialize(std::string_view name)
    {
        tree.push_back(name);

        T v;
        SerializeConstruct<T, JsonSerializer>::Deserialize(*this, v);

        tree.pop_back();

        return v;
    }

    template<class T, std::enable_if_t<is_class_pointer<T>, bool> = true>
    void Serialize(std::string_view name, const T& value)
    {
        using value_type = std::remove_pointer_t<T>;
        if(value == nullptr)
        {
            JsonReference(name);
        }
        else
        {
            tree.push_back(name);

            SerializeConstruct<value_type, JsonSerializer>::Serialize(*this, *value);

            tree.pop_back();
        }
    }

    template<class T, std::enable_if_t<is_class_pointer<T>, bool> = true>
    T Deserialize(std::string_view name)
    {
        using value_type = std::remove_pointer_t<T>;
        auto ref = JsonReference(name);

        if(ref.is_null())
        {
            return nullptr;
        }
        else
        {
            tree.push_back(name);

            T v = new value_type();
            SerializeConstruct<value_type, JsonSerializer>::Deserialize(*this, *v);

            tree.pop_back();

            return v;
        }
    }

    std::string Dump()
    {
        return json.dump();
    }

private:
    nlohmann::json::reference JsonReference(std::string_view name)
    {
        tree.push_back(name);

        nlohmann::json::pointer j = &json[tree[0].data()];
        for(auto n = tree.begin() + 1; n != tree.end(); n++)
        {
            j = &((*j)[n->data()]);
        }

        tree.pop_back();

        return *j;
    }
};


