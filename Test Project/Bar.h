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

        v.y = serializer.Deserialize<int>("y");
    }

};