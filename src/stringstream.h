#ifndef STRINGSTREAM_H
#define STRINGSTREAM_H

#include "stream.h"
#include <bstrlib/bstrlib.h>
#include <stdio.h>

typedef struct StringStream StringStream;

struct StringStream
{
    Stream base;
    bstring buffer;
    int64_t pos;
};

int StringStreamInit(StringStream *stream);

int StringStreamInitFromBstring(StringStream *stream, bstring buffer);

#endif
