# SerializationConceptThingy
A wrapper around serialization code that would provide a common interface regardless of format you're using... hopefully. 

Inspired by std::allocator and std::hash, this library attempts to provide a concept that will allow you to serialize / de-serialize objects without too much worry about what the underlying code is. Serialization / Deserialization does not need to modify any existing code, just simply specialize a template class This library by itself does not provide the ability to serialize / de-serialize, it is merely just a concept which one must fill out.

# Requirements
C++17
RTTI enabled (Optional used for being able to polymoprhic serialization / deserialization which requires type_info and std::any to work)

# Main Idea
Projects only need to include 1 file, Serializer.h then must fill out the following:
- SerializeConstruct (Class, fill out at least once per object and serializer)
- Serializer (Concept, fill out once per class that satisfies this concept)

## SerializeConstruct
Inspired by std::hash, all one must do is specialize this template class. The signature appears as follows

```c++
template<class Type, class Serializer>
struct SerializeConstruct
{
  static void Serialize(Serializer&, const Type&)
  {
    //Call serialize functions here for each member variable you wish to serialize
  }
  
  static void Deserialize(Serializer&, Type&)
  {
    //Call deserialize functions here for each member variable you wish to deserialize
  }
};
```

## Serializer
Inspired by std::allocator, one must simply satisfy the given concept of a Serializer and everything will work.
The following must be satisfied:

```c++
class Serializer
{
public:
  //Must open a stream to read and / or write to
  Serializer();
  
  //Flushes the contents to the stream and closes the stream
  ~Serializer();
  
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
```
# Sample
Once the concepts and classes are filled out you should be able to do something like the following
```c++

Serializer s;

Foo f;
Foo f2;

s.SerializeObject("f", f);
s.DeserializeObject("f", f2);

assert(f == f2); //Passes the assert

```
The project contains a full example using nlohmann json as the underlying JSON library.

# Miscellaneous
The concept supports polymorphic serialization, so that we don't require knowing the concrete type of a variable. To do so you must use the macro

```c++
REGISTER_POLYMORPHIC_SERIALIZE_FUNCTIONS(baseType, derivedType, serializerType);
```
The baseType should be the most parent class it can be. Currently only supports single inheritance.
This required typeid to work, but if you choose to support it, the following will be possible

```c++
struct Foo;
struct Bar : Foo;

Serializer s;
Bar* b = new Bar();
Foo* f = b;
Foo* f2;

//Must provide the base class
s.PolymorphicSerializeObject<Foo>("b", f);
s.PolymorphicDeserializeObject<Foo>("b", f2);

//Passes all 3 asserts
assert(typeid(*b) == typeid(*f));
assert(typeid(*b) == typeid(*f2));
assert(typeid(*f2) == typeid(*f));
```

# TODO
- Pointers: I've never really tested them in a way where I'd want to serialize / deserilize them, but can end up being null. Everything right now just assumes that an object exists if you want to serialize them, or at least, that's what I assume

