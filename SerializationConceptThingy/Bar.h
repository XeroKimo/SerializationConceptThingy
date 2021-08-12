#pragma once
#include "Foo.h"


struct Bar : Foo
{
    int y;
};


template<>
struct SerializeConstruct<Bar, JsonSerializer>
{
    static void Serialize(JsonSerializer& serializer, const Bar& v)
    {
        SerializeConstruct<Foo, JsonSerializer>::Serialize(serializer, v);
        //To serialize members, just simply do the following
           //
           //  Basic types, arrays, strings                    -> serializer.Serialize("x", v.x);
           //  Nested Classes, Non Polymorphic Class Pointers  -> serializer.SerializeObject("foo", v.foo);
           //  Nested Owning Polymorphic Pointers              -> serialize.PolymorphicSerializeObject("bar", v.bar);

        serializer.Serialize("y", v.y);

    }

    static void Deserialize(JsonSerializer& serializer, Bar& v)
    {
        SerializeConstruct<Foo, JsonSerializer>::Deserialize(serializer, v);
        //To deserialize members, just simply do the following
            //
            //  Basic types, arrays, strings                    -> serializer.Deserialize<V>("x", v.x);
            //  Nested Classes, Non Polymorphic Class Pointers  -> serializer.DeserializeObject("foo", v.foo);
            //  Nested Owning Polymorphic Pointers              -> serialize.PolymorphicDeserializeObject("bar", v.bar);

        serializer.Deserialize("y", v.y);
    }

};