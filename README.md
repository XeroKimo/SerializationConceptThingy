# SerializationConceptThingy
A wrapper around serialization code that would provide a common interface regardless of format you're using... hopefully. 

Inspired by std::allocator and std::hash, this library attempts to provide a concept that will allow you to serialize / de-serialize objects without too much worry about what the underlying code is. Serialization / Deserialization does not need to modify any existing code, just simply specialize a template class This library by itself does not provide the ability to serialize / de-serialize, it is merely just a concept which one must fill out.

I'm still working out on what the final interface looks like, and what the concept requires, for example, I'm thinking of removing opening / closing file requirements and just provide a dump function instead that would dump it's current contents.

As I'm lacking expeirence and knowledge on serialization, this concept is now frozen. I will only be making updates if there are bugs required to fix.

# Requirements
- C++17
- RTTI enabled (Optional used for being able to polymoprhic serialization / deserialization which requires type_info and std::any to work)

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
    //Required aliases   
    using value_type = Type;
    using pointer = Type*;
    using reference = Type&;

    using const_pointer = const Type*;
    using const_reference = const Type&;

    using serializer_type = SerializerT;

    static void Serialize(serializer_type& serializer, const_reference v)
    {
        //To serialize members, just simply do the following
        //
        //  Funadmental Values / Pointers, Objects, Non-Polymorphic Object Pointers -> serializer.Serialize("x", v.x);
        //  Polymorphic Object Pointers                                             -> serializer.PolySerialize("foo", v.foo);
        //  For base classes                                                        -> SerializeConstruct<Base, SerializerT>::Serialize(serializer, v);
    }

    static void Deserialize(serializer_type& serializer, reference v)
    {
        //To deserialize members, just simply do the following
        //
        //  Funadmental Values / Pointers, Objects, Non-Polymorphic Object Pointers -> serializer.Deserialize("x", v.x);
        //  Polymorphic Object Pointers                                             -> serializer.PolyDeserialize("foo", v.foo);
        //  For base classes                                                        -> SerializeConstruct<Base, SerializerT>::Deserialize(serializer, v);
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
    //Required type aliases    
    using serializer_type = Serializer;

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

        //For char* assume they are a pointer to a single character
    }

    template<class T, std::enable_if_t<std::is_arithmetic_v<T>, bool> = true>
    void Deserialize(std::string_view name, T*& value)
    {
        //For built in value pointer types
        //value can be a nullptr, so that case must be accounted for

        //For char* assume they are a pointer to a single character
    }

    template<class T, std::enable_if_t<std::is_class_v<T>, bool> = true>
    void Serialize(std::string_view name, const T& value)
    {
        //For object value types

        //Do not do any serialization in this function.  Only setup if needed
        //Object serialization will be handled by the SerializeConstruct. 
        //At the minimum this function must do the following

        //SerializeConstruct<T, serializer_type>::Serialize(*this, value);
    }

    template<class T, std::enable_if_t<std::is_class_v<T>, bool> = true>
    void Deserialize(std::string_view name, T& value)
    {
        //For object value types

        //Do not do any serialization in this function.  Only setup if needed
        //Object serialization will be handled by the SerializeConstruct. 
        //At the minimum this function must do the following

        //SerializeConstruct<T, serializer_type>::Deserialize(*this, value);
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
            SerializeConstruct<T, serializer_type>::Deserialize(*this, *value);
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
            SerializeConstruct<T, serializer_type>::Deserialize(*this, *value);
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
            //PolymorphicSerializeConstruct assumes that the value is not a nullptr
            PolymorphicSerializeConstruct<Base, Derived, serializer_type>::Serialize(*this, value);
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
            //PolymorphicSerializeConstruct assumes that the value is not a nullptr
            PolymorphicSerializeConstruct<Base, Derived, serializer_type>::Deserialize(*this, value);
        }
    }

    const T& Data() const
    {
        //T is the underlying serializing object in case we want to read the raw data
    }

    void Merge(const serializer_type& other)
    {
        //Merges the contents of the 2 serializers into 1
    }

    std::string Dump() const
    {
        //Return all the serialized data as a string
    }
};
```
# Sample
Once the concepts and classes are filled out you should be able to do something like the following
```c++

Serializer s;

Foo f;
Foo f2;

s.Serialize("f", f);
s.Deserialize("f", f2);

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
s.PolySerialize<Foo>("b", f);
s.PolyDesrialize<Foo>("b", f2);

//Passes all 3 asserts
assert(typeid(*b) == typeid(*f));
assert(typeid(*b) == typeid(*f2));
assert(typeid(*f2) == typeid(*f));
```

# TODO
- ~~Pointers: I've never really tested them in a way where I'd want to serialize / deserilize them, but can end up being null. Everything right now just assumes that an object exists if you want to serialize them, or at least, that's what I assume **(Done)**~~
- ~~Using aliases: STL classes tend to have some using alias in it to enable meta-programming and for tagging a class, I intend to figure out what aliases are required and at least make one tag for the serializer so that you only have to specialize the SerializeConstruct once and instead just check the tags ~~
- Containers: Figure out a way so that all containers can be serialized nicely, and whether they're containers of objects, object pointers, or fundamental types
- Strings: Figure out where strings should be serialized, should they be special and be part of the serializer? Or have speical privledges as a SerializeConstruct?
