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
        //  Basic types, arrays, strings                    -> serializer.Serialize("x", v.x);
        //  Nested Classes                                  -> serializer.SerializeObject("foo", v.foo);
        //  Non Polymorphic Class Pointers                  -> serializer.SerializeObject("foo", (*v.foo));
        //  Nested Owning Polymorphic Pointers              -> serialize.PolymorphicSerializeObject("bar", v.bar);
    }
 
    static void Deserialize(SerializerT& serializer, T& v)
    {
        //To deserialize members, just simply do the following
        //
        //  Basic types, arrays, strings                    -> serializer.Deserialize<V>("x", v.x);
        //  Nested Classes, Non Polymorphic Class Pointers  -> serializer.DeserializeObject("foo", v.foo);
        //  Non Polymorphic Class Pointers                  -> serializer.DeserializeObject("foo", (*v.foo));
        //  Nested Owning Polymorphic Pointers              -> serialize.PolymorphicDeserializeObject("bar", v.bar);
    }
};
*/


/*
//Serializer concept
class Serializer
{

  //The constructor must open some file
  //The destructor must write the contents to the file and close it if the file has not been closed

public:

  template<class T, std::enable_if_t<std::is_arithmetic_v<T>, bool> = true>
  void Serialize(std::string_view name, const T& value)
  {
      //For built in types
  }

  template<class T>
  void Serialize(std::string_view name, const std::vector<T>& value)
  {
      //For arrays
  }

  void Serialize(std::string_view name, std::string_view value)
  {
      //For strings
  }

  template<class T, std::enable_if_t<std::is_class_v<T>, bool> = true>
  void SerializeObject(std::string_view name, const T& value)
  {
      //Do not parse any member variables in here
      //Member variables should be handled by the SerializeConstruct
      //Only do any set up if required

      //For objects, at the minimum the following must be called
      SerializeConstruct<T, Serializer>::Serialize(*this, value);
  }

  //This is not required if you don't plan to support polymorphic serialization
  template<class Base, class Derived, std::enable_if_t<std::is_base_of_v<Base, Derived>, bool> = true>
  void PolymorphicSerializeObject(std::string_view name, const Derived& value)
  {
      //Do not parse any member variables in here
      //Member variables should be handled by the SerializeConstruct
      //Only do any set up if required

      //For objects, at the minimum, the following must be called
      Serialize("Type", typeid(value).name());
      PolymorphicSerializeConstruct<Base, Derived, Serializer>::Serialize(*this, value);
  }

  template<class T, std::enable_if_t<std::is_arithmetic_v<T>, bool> = true>
  void Deserialize(std::string_view name, T& value)
  {
      //For built in types
  }

  template<class T>
  void Deserialize(std::string_view name, std::vector<T>& value)
  {
      //For arrays
  }

  void Deserialize(std::string_view name, std::string& value)
  {
      //For strings
  }

  template<class T, std::enable_if_t<std::is_class_v<T>, bool> = true>
  void DeserializeObject(std::string_view name, T& value)
  {
      //Do not parse any member variables in here
      //Member variables should be handled by the SerializeConstruct
      //Only do any set up if required

      //For objects, at the minimum, the following must be called
      SerializeConstruct<T, Serializer>::Deserialize(*this, value);
  }

  //This is not required if you don't plan to support polymorphic serialization
  template<class Base, class Derived, std::enable_if_t<std::is_class_v<Base>, bool> = true>
  void PolymorphicDeserializeObject(std::string_view name, Derived*& value)
  {
      //Do not parse any member variables in here
      //Member variables should be handled by the SerializeConstruct
      //Only do any set up if required

      //For objects, at the minimum, the following must be called

      std::string type;
      Deserialize("Type", type);

      PolymorphicSerializeConstruct<Base, Derived, Serializer>::Deserialize(*this, value, type);
  }

  void Flush()
  {
      //Writes data to the file
  }

  void Close()
  {
      //Writes data to the file then closes it
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
        GetPolymorphicSerializeFunctions<SerializerT>()[typeid(*v).name()](serializer, std::any(std::reference_wrapper<const Base*>(v)));
    }
    static void Deserialize(SerializerT& serializer, Derived*& v, std::string_view type)
    {
        Base* base;
        auto any = std::any(std::reference_wrapper<Base*>(base));

        GetPolymorphicDeserializeFunctions<SerializerT>()[type.data()](serializer, any);

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
