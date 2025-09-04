# bf

Language: (**English** | [中文简体](./README-CHS.md))

My programming exerise: a simple Brainfuck language (hereinafter referred to as
"BF") simulator, written in ANSI C (C89).

(BF's wiki page: [Wikipedia](https://en.wikipedia.org/wiki/Brainfuck)
 | [Baidu Baike](https://baike.baidu.com/item/Brainfuck) (in Chinese))


## Interface

Some interfaces and some documents are modeled after repository
[`benhoyt/inih`](https://github.com/benhoyt/inih).

Interface is as below:

```C
typedef struct {
    bf_byte_t        *current;
    const bf_byte_t  *begin;
    const bf_byte_t  *end;  /* past-the-end */
} bf_s;

int bf_init(bf_s *ths, bf_byte_t *begin, bf_byte_t *end);

void bf_show_nearby_memory(const bf_s *ths, size_t on_the_left,
    size_t on_the_right);

int bf_run_filename(bf_s *ths, const char *command,
    const char *filename, bf_handler_t handler, void *out_stream);

int bf_run_file(bf_s *ths, const char *command, FILE *in_file,
    bf_handler_t handler, void *out_stream);

int bf_run_memory(bf_s *ths, const char *command, const bf_byte_t *mem,
    size_t mem_size, bf_handler_t handler, void *out_stream);

int bf_run_stream(bf_s *ths, const char *command, bf_reader_t reader,
    void *in_stream, bf_handler_t handler, void *out_stream);
```

In short, `bf_s` is the class. Functions `bf_run_*()` and
`bf_show_nearby_memory()` are methods.

Funtions `bf_run_*()` interpret and execute BF code, which is passed as string:

* Functions `bf_run_filename()` and `bf_run_file()` get in-stream from file.
* Function `bf_run_memory()` get in-stream from arrays in memory.
* Function `bf_run_stream()` is the most general one. The first three are
implemented through it.

Each time a output command `.` is executed, call the callback `(*handler)()`,
with arguments: byte currently pointed to by the data pointer and `out_stream`.

This is the same as [`benhoyt/inih`](https://github.com/benhoyt/inih).

and `bf_show_nearby_memory()` function is used for visualization, showing
the data around the current data pointer.

See the comments in the function prototypes and structure `typedef`s in the
header file for detailed documentation.


## Compiling

I only kown command `gcc`. I don't know much about various compilating tools.

If you only compile the two files `bf_test.c` and `bf.c` together, you will get
a simple interactive BF interpreter.

Also you can ignore `bf_test.c`, but only compile `bf.c` with your code, and
include `bf.h` in your code, then you can use the types and functions in it.


## History

This interpreter is a programming exercise I made myself half a year after I
learned C language (which is, written in 2022-12-05). I also made a video and
posted it on Bilibili (a Chinese video website) that time:

(Video) [(for source code, see introduction) An overly simple Brainfuck interpreter](https://www.bilibili.com/video/BV1T24y1e78o) (in Chinese)

Now (2024-12-18), I imitated [`benhoyt/inih`](https://github.com/benhoyt/inih)
and made changes to my code, and then posted it here (GitHub).


## Other Information

The code style basically follows
[Nginx style](https://nginx.org/en/docs/dev/development_guide.html#code_style).

Open source licence: `BSD-3-Clause`.
