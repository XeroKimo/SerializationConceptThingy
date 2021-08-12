#include "Foo.h"
#include "JsonSerializer.h"

REGISTER_POLYMORPHIC_SERIALIZE_FUNCTIONS(Foo, Foo, JsonSerializer);