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
#include "Serializer.h"
#include "json.hpp"
#include <fstream>


//Serializer concept
class JsonSerializer
{
private:
    nlohmann::json json{};
    std::fstream stream;
    std::ios_base::openmode mode;

    std::vector<std::string_view> tree;

public:
    JsonSerializer(std::wstring_view fileName, std::ios_base::openmode mode) :
        stream(fileName.data(), mode),
        mode(mode)
    {
        if(mode & stream.in)
            stream >> json;
    }

    ~JsonSerializer()
    {
        Close();
    }

public:
    //The following functions must exist unless specified

    template<class T, std::enable_if_t<std::is_arithmetic_v<T>, bool> = true>
    void Serialize(std::string_view name, const T& value)
    {
        //For built in types
        JsonReference(name) = value;
    }

    template<class T>
    void Serialize(std::string_view name, const std::vector<T>& value)
    {
        //For arrays
        JsonReference(name) = value;
    }

    void Serialize(std::string_view name, std::string_view value)
    {
        //For strings
        JsonReference(name) = value;
    }

    template<class T, std::enable_if_t<std::is_class_v<T>, bool> = true>
    void SerializeObject(std::string_view name, const T& value)
    {
        //Do not parse any member variables in here
        //Member variables should be handled by the SerializeConstruct
        //Only do any set up if required
        // 
        //For objects, at the minimum, the following must be called
        tree.push_back(name);

        SerializeConstruct<T, JsonSerializer>::Serialize(*this, value);

        tree.pop_back();
    }

    //This is not required if you don't plan to support polymorphic serialization
    template<class Base, class Derived, std::enable_if_t<std::is_base_of_v<Base, Derived>, bool> = true>
    void PolymorphicSerializeObject(std::string_view name, const Derived& value)
    {
        //Do not parse any member variables in here
        //Member variables should be handled by the SerializeConstruct
        //Only do any set up if required
        //For objects, at the minimum, the following must be called

        tree.push_back(name);

        Serialize("Type", typeid(value).name());
        PolymorphicSerializeConstruct<Base, Derived, JsonSerializer>::Serialize(*this, value);

        tree.pop_back();
    }

    template<class T, std::enable_if_t<std::is_arithmetic_v<T>, bool> = true>
    void Deserialize(std::string_view name, T& value)
    {
        //For built in types

        value = JsonReference(name);
    }

    template<class T>
    void Deserialize(std::string_view name, std::vector<T>& value)
    {
        //For arrays
        auto ref = JsonReference(name);
        value.insert(value.end(), ref.begin(), ref.end());
    }

    void Deserialize(std::string_view name, std::string& value)
    {
        //For strings
        value = JsonReference(name);
    }

    template<class T, std::enable_if_t<std::is_class_v<T>, bool> = true>
    void DeserializeObject(std::string_view name, T& value)
    {
        //Do not parse any member variables in here
        //Member variables should be handled by the SerializeConstruct
        //Only do any set up if required
        //For objects, at the minimum, the following must be called
        tree.push_back(name);

        SerializeConstruct<T, JsonSerializer>::Deserialize(*this, value);

        tree.pop_back();
    }

    //This is not required if you don't plan to support polymorphic serialization
    template<class Base, class Derived, std::enable_if_t<std::is_class_v<Base>, bool> = true>
    void PolymorphicDeserializeObject(std::string_view name, Derived*& value)
    {
        tree.push_back(name);

        //Do not parse any member variables in here
        //Member variables should be handled by the SerializeConstruct
        //Only do any set up if required
        // 
        //For objects, at the minimum, the following must be called
        std::string type;
        Deserialize("Type", type);

        PolymorphicSerializeConstruct<Base, Derived, JsonSerializer>::Deserialize(*this, value, type);

        tree.pop_back();
    }

    void Flush()
    {
        //Writes data to the file
        if(mode & stream.out)
            stream << json;
    }

    void Close()
    {
        if(stream.is_open())
        {
            //Writes data to the file then closes it
            Flush();
            stream.close();
        }
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


