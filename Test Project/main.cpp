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
#include "JsonSerializer.h"
#include "Foo.h"
#include "Bar.h"
#include <assert.h>
#include <vector>


int main()
{
    JsonSerializer serializer;
    int test = 20;

    serializer.Serialize("test", test);
    int test2 = serializer.Deserialize<int>("test");
    assert(test == test2);

    int* ip = new int(50);
    serializer.Serialize("ip", ip);
    int* ip2 = serializer.Deserialize<int*>("ip");
    
    assert(*ip == *ip2);

    Foo f;
    f.x = 300;
    serializer.Serialize("f", f);
    Foo f2 = serializer.Deserialize<Foo>("f");

    assert(f.x == f2.x);

    Bar b;
    b.x = 300;
    b.y = 600;
    serializer.Serialize("b", b);
    Bar b2 = serializer.Deserialize<Bar>("b");

    assert(b.x == b2.x && b.y == b2.y);

    Bar* bp = new Bar();
    bp->x = 300;
    bp->y = 700;
    serializer.Serialize("bp", bp);
    Bar* bp2 = serializer.Deserialize<Bar*>("bp");

    if(bp == nullptr)
        assert(bp == bp2);
    else
        assert(bp->x == bp2->x && bp->y == bp2->y);
}