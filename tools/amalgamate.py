import io
import os
import sys

this_dir = os.path.dirname(os.path.abspath(__file__))

src_dir = os.path.join(this_dir, '..', 'src')

sources = (
    'config.h',
    'queue.h',
    'log.h',
    'misc.c',
    'lib/bstrlib/bstrlib.c',
    'stream.c',
    'socketstream.c',
    'stream_mqtt.c',
    'socket.c',
    'packet.c',
    'serialize.c',
    'deserialize.c',
    'client.c'
)


def is_header(filename):
    return os.path.splitext(filename)[1] == '.h'


def get_header(src):
    root, ext = os.path.splitext(src)
    return root + '.h'


def read_file(filename):
    def tounicode(s):
        if sys.version_info[0] == 2:
            return s.decode('utf-8')
        else:
            return s
    with open(filename, 'r') as fp:
        buf = io.StringIO()
        for line in fp:
            if line.startswith('#include "'):
                if line[10:].startswith('mqtt.h'):
                    pass
                else:
                    continue
            elif line.startswith('#include <'):
                header = line[10:]
                idx = header.find('>')
                if idx > 0:
                    header = header[:idx]
                    header = os.path.join(src_dir, 'lib', header)
                    if os.path.isfile(header):
                        continue
            buf.write(tounicode(line))
        return buf.getvalue()


def file_header(filename):
    filename = os.path.basename(filename)
    # how long lines we create
    linelen = 72
    # how much space left after the necessary comment markup
    chars = linelen - 4
    # how much padding in total for filename
    padding = chars - len(filename)
    padding_l = padding // 2
    padding_r = padding - padding_l
    lines = (
        '',
        '/*' + '*'*chars + '*/',
        '/*' + ' '*padding_l + filename + ' '*padding_r + '*/',
        '/*' + '*'*chars + '*/',
        '\n',
    )
    return '\n'.join(lines)


def write_file(output, srcfilename):
    output.write(file_header(srcfilename))
    output.write(read_file(srcfilename))


output_filename = sys.argv[1]

with open(output_filename, 'w') as out:
    for source in sources:
        path = os.path.join(src_dir, source)

        if is_header(path):
            write_file(out, path)
        else:
            header = get_header(path)
            if os.path.isfile(header):
                write_file(out, header)
            write_file(out, path)
