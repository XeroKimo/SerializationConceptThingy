#include "Bar.h"
#include "JsonSerializer.h"

REGISTER_POLYMORPHIC_SERIALIZE_FUNCTIONS(Foo, Bar, JsonSerializer);