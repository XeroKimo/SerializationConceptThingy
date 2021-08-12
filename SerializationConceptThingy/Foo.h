#pragma once
#include "JsonSerializer.h"

struct Foo
{
    int x;

    virtual void f() {}
};


template<>
struct SerializeConstruct<Foo, JsonSerializer>
{
    static void Serialize(JsonSerializer& serializer, const Foo& v)
    {
        serializer.Serialize("x", v.x);
    }

    static void Deserialize(JsonSerializer& serializer, Foo& v)
    {
        serializer.Deserialize("x", v.x);
    }
};