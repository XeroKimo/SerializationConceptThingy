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
#include<unordered_map>
#include<string_view>
#include<functional>
#include<any>




//This struct must be specialized for objects if they want to participate in serialization
template<class Type, class SerializerT>
struct SerializeConstruct;
/*
{
    static void Serialize(SerializerT& serializer, const T& v)
    {
        //To serialize members, just simply do the following
        //
        //  Funadmental Values / Pointers, Objects, Non-Polymorphic Object Pointers -> serializer.Serialize("x", v.x);
        //  Polymorphic Object Pointers                                             -> serializer.PolySerialize("foo", v.foo);
        //  For base classes                                                        -> SerializeConstruct<Base, SerializerT>::Serialize(serializer, v);
    }

    static void Deserialize(SerializerT& serializer, T& v)
    {
        //To deserialize members, just simply do the following
        //
        //  Funadmental Values / Pointers, Objects, Non-Polymorphic Object Pointers -> serializer.Deserialize("x", v.x);
        //  Polymorphic Object Pointers                                             -> serializer.PolyDeserialize("foo", v.foo);
        //  For base classes                                                        -> SerializeConstruct<Base, SerializerT>::Deserialize(serializer, v);
    }
};
*/


/*
//Serializer concept
class Serializer
{
public:
    //The following functions must exist unless specified

    template<class T, std::enable_if_t<std::is_arithmetic_v<T>, bool> = true>
    void Serialize(std::string_view name, const T value)
    {
        //For built in value types
    }

    template<class T, std::enable_if_t<std::is_arithmetic_v<T>, bool> = true>
    void Deserialize(std::string_view name, T& value)
    {
        //For built in value types
    }

    template<class T, std::enable_if_t<std::is_arithmetic_v<T>, bool> = true>
    void Serialize(std::string_view name, const T* value)
    {
        //For built in value pointer types
        //value can be a nullptr, so that case must be accounted for

        //Depending on the library used for serialization
        //c-style strings may not work, so for char*
        //One must specialize the template, or just assume it's pointing to a single char
    }

    template<class T, std::enable_if_t<std::is_arithmetic_v<T>, bool> = true>
    void Deserialize(std::string_view name, T*& value)
    {
        //For built in value pointer types
        //value can be a nullptr, so that case must be accounted for

        //Depending on the library used for serialization
        //c-style strings may not work, so for char*
        //One must specialize the template, or just assume it's pointing to a single char
    }

    template<class T, std::enable_if_t<std::is_class_v<T>, bool> = true>
    void Serialize(std::string_view name, const T& value)
    {
        //For object value types

        //Do not do any serialization in this function.  Only setup if needed
        //Object serialization will be handled by the SerializeConstruct. 
        //At the minimum this function must do the following

        //SerializeConstruct<T, Serializer>::Serialize(*this, value);
    }

    template<class T, std::enable_if_t<std::is_class_v<T>, bool> = true>
    void Deserialize(std::string_view name, T& value)
    {
        //For object value types

        //Do not do any serialization in this function.  Only setup if needed
        //Object serialization will be handled by the SerializeConstruct. 
        //At the minimum this function must do the following

        //SerializeConstruct<T, Serializer>::Deserialize(*this, value);
    }

    template<class T, std::enable_if_t<std::is_class_v<T>, bool> = true>
    void Serialize(std::string_view name, const T* value)
    {
        //For concrete object pointer types
        //This is when the pointer was made somewhere along the lines of
        //T* = new T();

        //Do not do any serialization in this function.  Only setup if needed
        //Object serialization will be handled by the SerializeConstruct. 
        //At the minimum this function must do the following

        if(value == nullptr)
        {
            //Handle nullptr case
        }
        else
        {
            SerializeConstruct<T, Serializer>::Deserialize(*this, *value);
        }
    }

    template<class T, std::enable_if_t<std::is_class_v<T>, bool> = true>
    void Deserialize(std::string_view name, T*& value)
    {
        //For concrete object pointer types
        //This is when the pointer was made somewhere along the lines of
        //T* = new T();

        //Do not do any serialization in this function.  Only setup if needed
        //Object serialization will be handled by the SerializeConstruct.
        //At the minimum this function must do the following

        //Check to see if the value we're tryin to deserialize is null
        //and handle that case
        if()
        {
            value = nullptr;
        }
        else
        {
            SerializeConstruct<T, Serializer>::Deserialize(*this, *value);
        }
    }

    template<class Base, class Derived, std::enable_if_t<std::is_base_of_v<Base, Derived>, bool> = true>
    void PolySerialize(std::string_view name, const Derived* value)
    {
        //This function is optional
        //Requires RTTI enabled to enable std::any and typeid, and be able to serialize strings
        //In order to be used REGISTER_POLYMORPHIC_SERIALIZE_FUNCTIONS(Base, Derived, Serializer) macro must be called

        //For polymorphic object pointer types
        //This is when the pointer was made somewhere along the lines of
        //Base* = new Derived();
        //This can work with Base* = new Base(), but will be slower

        //Do not do any serialization in this function.  Only setup if needed
        //Object serialization will be handled by the SerializeConstruct.
        //At the minimum this function must do the following

        if(value == nullptr)
        {
            //Handle nullptr case
        }
        else
        {
            PolymorphicSerializeConstruct<Base, Derived, Serializer>::Serialize(*this, value);
        }
    }

    template<class Base, class Derived, std::enable_if_t<std::is_base_of_v<Base, Derived>, bool> = true>
    void PolyDeserialize(std::string_view name, Derived*& value)
    {
        //This function is optional
        //Requires RTTI enabled to enable std::any and typeid, and be able to serialize strings
        //In order to be used REGISTER_POLYMORPHIC_SERIALIZE_FUNCTIONS(Base, Derived, Serializer) macro must be called

        //For polymorphic object pointer types
        //This is when the pointer was made somewhere along the lines of
        //Base* = new Derived();
        //This can work with Base* = new Base(), but will be slower

        //Do not do any serialization in this function.  Only setup if needed
        //Object serialization will be handled by the SerializeConstruct.
        //At the minimum this function must do the following

        //Check to see if the value we're tryin to deserialize is null
        //and handle that case
        if()
        {
            value = nullptr;
        }
        else
        {
            PolymorphicSerializeConstruct<Base, Derived, Serializer>::Deserialize(*this, value);
        }
    }

    std::string Dump()
    {
        //Return all the serialized data as a string
    }
};
 
 
*/


template<class SerializerT>
using PolymorphicSerializerFunctionMap = std::unordered_map<std::string, std::function<void(SerializerT&, const std::any&)>>;

template<class SerializerT>
using PolymorphicDeserializerFunctionMap = std::unordered_map<std::string, std::function<void(SerializerT&, std::any&)>>;

template<class SerializerT>
PolymorphicSerializerFunctionMap<SerializerT>& GetPolymorphicSerializeFunctions()
{
    static PolymorphicSerializerFunctionMap<SerializerT> map;
    return map;
}

template<class SerializerT>
PolymorphicDeserializerFunctionMap<SerializerT>& GetPolymorphicDeserializeFunctions()
{
    static PolymorphicDeserializerFunctionMap<SerializerT> map;
    return map;
}

template<class Base, class Derived, class SerializerT>
struct PolymorphicSerializeConstruct
{
    static void Serialize(SerializerT& serializer, const Derived*& v)
    {
        serializer.Serialize("Type", std::string(typeid(*v).name()));
        GetPolymorphicSerializeFunctions<SerializerT>()[typeid(*v).name()](serializer, std::any(std::reference_wrapper<const Base*>(v)));
    }
    static void Deserialize(SerializerT& serializer, Derived*& v)
    {
        Base* base;
        auto any = std::any(std::reference_wrapper<Base*>(base));
        std::string type;

        serializer.Deserialize("Type", type);
        GetPolymorphicDeserializeFunctions<SerializerT>()[type](serializer, any);

        v = static_cast<Derived*>(base);
    }
};

template<class BaseType, class DerivedType, class SerializerT>
class RegisterSerialization
{
private:
    static RegisterSerialization* instance;

private:
    RegisterSerialization()
    {
        GetPolymorphicSerializeFunctions<SerializerT>()[typeid(DerivedType).name()] = PolymorphicSerialize;
        GetPolymorphicDeserializeFunctions<SerializerT>()[typeid(DerivedType).name()] = PolymorphicDeserialize;
    }

    static void PolymorphicSerialize(SerializerT& serializer, const std::any& v)
    {
        //This function does nothing but casts the serialized type to it's concrete type and forwards it to the real serialization
        SerializeConstruct<DerivedType, SerializerT>::Serialize(serializer, *static_cast<const DerivedType*>(std::any_cast<std::reference_wrapper<const BaseType*>>(v).get()));
    }

    static void PolymorphicDeserialize(SerializerT& serializer, std::any& v)
    {
        std::reference_wrapper<BaseType*> value = std::any_cast<std::reference_wrapper<BaseType*>>(v);
        value.get() = new DerivedType();
        //This function does nothing but casts the deserialized type to it's concrete type and forwards it to the real serialization
        SerializeConstruct<DerivedType, SerializerT>::Deserialize(serializer, (*static_cast<DerivedType*>(value.get())));
    }
};

//Registers the polymorphic serialization functions
//Must be placed in a .cpp file
#define REGISTER_POLYMORPHIC_SERIALIZE_FUNCTIONS(baseType, derivedType, serializerType) RegisterSerialization<baseType, derivedType, serializerType>* RegisterSerialization<baseType, derivedType, serializerType>::instance = new RegisterSerialization<baseType, derivedType, serializerType>();
