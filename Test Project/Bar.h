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
    using value_type = Bar;
    using pointer = Bar*;
    using reference = Bar&;

    using const_pointer = const Bar*;
    using const_reference = const Bar&;

    using serializer_type = JsonSerializer;

    static void Serialize(serializer_type& serializer, const const_reference& v)
    {
        SerializeConstruct<Foo, serializer_type>::Serialize(serializer, v);
        serializer.Serialize("y", v.y);

    }

    static void Deserialize(serializer_type& serializer, reference& v)
    {
        SerializeConstruct<Foo, serializer_type>::Deserialize(serializer, v);
        serializer.Deserialize("y", v.y);
    }

};