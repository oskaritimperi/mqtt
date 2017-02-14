#ifndef STREAM_H
#define STREAM_H

#include <stdlib.h>
#include <stdint.h>

#ifndef SEEK_SET
#define SEEK_SET (-1)
#endif

#ifndef SEEK_CUR
#define SEEK_CUR (-2)
#endif

#ifndef SEEK_END
#define SEEK_END (-3)
#endif

typedef struct Stream Stream;
typedef struct StreamOps StreamOps;

struct Stream
{
    const StreamOps *ops;
};

struct StreamOps
{
    int64_t (*read)(void *ptr, size_t size, Stream *stream);
    int64_t (*write)(const void *ptr, size_t size, Stream *stream);
    int (*close)(Stream *stream);
    int (*seek)(Stream *stream, int64_t offset, int whence);
    int64_t (*tell)(Stream *stream);
};

int StreamClose(Stream *stream);

int64_t StreamRead(void *ptr, size_t size, Stream *stream);
int64_t StreamReadUint16Be(uint16_t *v, Stream *stream);

int64_t StreamWrite(const void *ptr, size_t size, Stream *stream);
int64_t StreamWriteUint16Be(uint16_t v, Stream *stream);

int StreamSeek(Stream *stream, int64_t offset, int whence);

int64_t StreamTell(Stream *stream);

#endif
