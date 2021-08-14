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
    friend struct SerializeConstruct<std::string, JsonSerializer>;
private:
    nlohmann::json json{};
    std::vector<std::string_view> tree;


public:
    //The following functions must exist unless specified

    template<class T, std::enable_if_t<std::is_arithmetic_v<T>, bool> = true>
    void Serialize(std::string_view name, const T value)
    {
        //For built in types
        JsonReference(name) = value;
    }

    template<class T, std::enable_if_t<std::is_arithmetic_v<T>, bool> = true>
    void Deserialize(std::string_view name, T& value)
    {
        //For built in types
        value = JsonReference(name);
    }

    template<class T, std::enable_if_t<std::is_arithmetic_v<T>, bool> = true>
    void Serialize(std::string_view name, const T* value)
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

    template<class T, std::enable_if_t<std::is_arithmetic_v<T>, bool> = true>
    void Deserialize(std::string_view name, T*& value)
    {
        using value_type = std::remove_pointer_t<T>;
        auto ref = JsonReference(name);

        value = (ref.is_null()) ? nullptr : new T(ref);
    }

    template<class T, std::enable_if_t<std::is_class_v<T>, bool> = true>
    void Serialize(std::string_view name, const T& value)
    {
        tree.push_back(name);

        SerializeConstruct<T, JsonSerializer>::Serialize(*this, value);

        tree.pop_back();
    }

    template<class T, std::enable_if_t<std::is_class_v<T>, bool> = true>
    void Deserialize(std::string_view name, T& value)
    {
        tree.push_back(name);

        SerializeConstruct<T, JsonSerializer>::Deserialize(*this, value);

        tree.pop_back();
    }

    template<class T, std::enable_if_t<std::is_class_v<T>, bool> = true>
    void Serialize(std::string_view name, const T* value)
    {
        if(value == nullptr)
        {
            JsonReference(name);
        }
        else
        {
            tree.push_back(name);

            SerializeConstruct<T, JsonSerializer>::Serialize(*this, *value);

            tree.pop_back();
        }
    }

    template<class T, std::enable_if_t<std::is_class_v<T>, bool> = true>
    void Deserialize(std::string_view name, T*& value)
    {
        auto ref = JsonReference(name);

        if(ref.is_null())
        {
            value = nullptr;
        }
        else
        {
            tree.push_back(name);

            value = new T();
            SerializeConstruct<T, JsonSerializer>::Deserialize(*this, *value);

            tree.pop_back();
        }
    }

    template<class Base, class Derived, std::enable_if_t<std::is_base_of_v<Base, Derived>, bool> = true>
    void PolySerialize(std::string_view name, const Derived* value)
    {
        if(value == nullptr)
        {
            JsonReference(name);
        }
        else
        {
            tree.push_back(name);

            PolymorphicSerializeConstruct<Base, Derived, JsonSerializer>::Serialize(*this, value);

            tree.pop_back();
        }
    }

    template<class Base, class Derived, std::enable_if_t<std::is_base_of_v<Base, Derived>, bool> = true>
    void PolyDeserialize(std::string_view name, Derived*& value)
    {
        auto ref = JsonReference(name);

        if(ref.is_null())
        {
            value = nullptr;
        }
        else
        {
            tree.push_back(name);

            PolymorphicSerializeConstruct<Base, Derived, JsonSerializer>::Deserialize(*this, value);

            tree.pop_back();
        }
    }

    std::string Dump()
    {
        return json.dump();
    }

private:
    nlohmann::json::reference JsonReference(std::string_view name)
    {
        return JsonReference()[name.data()];
    }

    nlohmann::json::reference JsonReference()
    {
        if(tree.size() == 0)
            return json;

        nlohmann::json::pointer j = &json[tree[0].data()];
        for(auto n = tree.begin() + 1; n != tree.end(); n++)
        {
            j = &((*j)[n->data()]);
        }

        return *j;
    }
};


//TODO: Figure out an interface to support containers for any kind of format without having to touch
//The class to basically do what is done below.
template<>
struct SerializeConstruct<std::string, JsonSerializer>
{
    static void Serialize(JsonSerializer& serializer, const std::string& v)
    {
        serializer.JsonReference() = v;
    }

    static void Deserialize(JsonSerializer& serializer, std::string& v)
    {
        v = serializer.JsonReference();
    }
};
