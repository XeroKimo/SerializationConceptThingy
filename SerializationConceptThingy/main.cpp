#include "JsonSerializer.h"
#include "Foo.h"
#include "Bar.h"
#include <assert.h>
#include <vector>


int main()
{

    JsonSerializer serializer(L"testJson.json", std::ios_base::out);

    int test = 6;
    serializer.Serialize("test", test);

    Foo foo;
    foo.x = 100;
    serializer.SerializeObject("foo", foo);


    std::vector<int> values{ 5, 3, 7, 10 };
    serializer.Serialize("values", values);

    Bar bar;
    bar.x = 200;
    bar.y = 300;
    serializer.PolymorphicSerializeObject<Foo>("bar", bar);

    Foo* fBar = new Bar();
    fBar->x = 100;
    static_cast<Bar*>(fBar)->y = 600;
    serializer.PolymorphicSerializeObject<Foo>("fBar", *fBar);

    serializer.Close();

    {
        JsonSerializer serializer(L"testJson.json", std::ios_base::in);

        int test2;
        Foo foo2{};
        std::vector<int> values2;
        Bar* bar2;
        Foo* fBar2;

        serializer.Deserialize("test", test2);
        serializer.DeserializeObject("foo", foo2);
        serializer.Deserialize("values", values2);
        serializer.PolymorphicDeserializeObject<Foo>("bar", bar2);
        serializer.PolymorphicDeserializeObject<Foo>("fBar", fBar2);


        assert(test == test2);
        assert(foo.x == foo2.x);

        for(int i = 0; i < values.size(); i++)
        {
            assert(values[i] == values2[i]);
        }
        assert(bar.x == bar2->x && bar.y == static_cast<Bar*>(bar2)->y);
        assert(typeid(*fBar) == typeid(*fBar2));
    }

}