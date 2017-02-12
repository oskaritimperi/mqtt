#ifndef STRINGBUF_H
#define STRINGBUF_H

#include <stdlib.h>

typedef struct StringBuf StringBuf;

struct StringBuf
{
    char *data;
    int size;
    int len;
};

int StringBufInit(StringBuf *buf, size_t size);

int StringBufInitFromCString(StringBuf *buf, const char *s, int len);

int StringBufInitFromData(StringBuf *buf, const void *ptr, size_t size);

void StringBufDeinit(StringBuf *buf);

size_t StringBufAvailable(StringBuf *buf);

int StringBufGrow(StringBuf *buf, size_t size);

int StringBufAppendData(StringBuf *buf, const void *ptr, size_t size);

#define StaticStringBuf(S) { "" S, -1, sizeof(S)-1 }

#endif
