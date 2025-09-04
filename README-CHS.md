# bf

语言：（[English](./README.md) | **中文简体**）

我的一个编程练习：简单的 Brainfuck 语言（以下简称 BF）模拟器，
使用 ANSI C（C89）编写。

（BF 的百科页面：[维基百科](https://zh.wikipedia.org/wiki/Brainfuck)
 | [百度百科](https://baike.baidu.com/item/Brainfuck)）


## 接口

接口和部分文档模仿了开源库 [`benhoyt/inih`](https://github.com/benhoyt/inih)。

接口如下：

```C
typedef unsigned char bf_byte_t;
typedef int (*bf_reader_t)(void *stream);
typedef int (*bf_handler_t)(int b, void *stream);
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

`bf_s` 是类， `bf_run_*()` 和 `bf_show_nearby_memory()`
函数是成员函数（类方法）。

其中 `bf_run_*()` 函数都解释执行 BF 代码。BF 代码以字符串形式传入。

* 函数 `bf_run_filename()` 和 `bf_run_file()` 从文件获取输入流。
* 函数 `bf_run_memory()` 从内存中的数组获取输入流。
* 函数 `bf_run_stream()` 可以自定义输入方式，是最通用的，前三者都由它来实现。

每执行一个 `.` 命令（BF 中的输出指令），就调用 `(*handler)()` 函数，
参数就是数据指针当前所指的字节和 `out_stream`。

`bf_show_nearby_memory()` 函数用于可视化，显示当前数据指针附近的数据。

详细文档请见头文件中各个函数原型和 `typedef` 处的注释。


## 编译

库:
- `bf.c`

解释器:
- 库
- `bf_repl.c`

若只将 `bf.c` 和 `bf_repl.c` 一起编译，可以得到一个解释器。

也可以忽略 `bf_repl.c` ，而在自己的代码中包含 `bf.h`
，则可以将这里的代码作为库使用。

## 历史

这个解释器是我刚学会 C 语言半年后 (2022-12-05) 自己做的一个编程练习，
当时还拍成视频发在了 B 站上：

（视频）[（源码见简介）一个过于简单的 Brainfuck 解释器](https://www.bilibili.com/video/BV1T24y1e78o)

现在 (2024-12-18) 模仿 [`benhoyt/inih`](https://github.com/benhoyt/inih)
做了较大的改动，然后发布到了这里（GitHub 上）。


## 其他信息

代码风格基本遵循
[Nginx 风格](https://nginx.org/en/docs/dev/development_guide.html#code_style)。

开源协议：`BSD-3-Clause`。
