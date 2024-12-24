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
typedef struct {
    bf_byte_t        *current;
    const bf_byte_t  *begin;
    const bf_byte_t  *end;  /* past-the-end */
} bf_s;

typedef struct {
    const bf_byte_t  *base_ptr;
    int               begin_offset;
    int               end_offset;  /* past-the-end */
    int               current_offset;
} bf_nearby_16_s;

int bf_init(bf_s *ths, bf_byte_t *begin, bf_byte_t *end);

int bf_nearby_16(const bf_s *ths, bf_nearby_16_s *out);

int bf_run_filename(bf_s *ths, const char *command,
    const char *filename, bf_handler_t handler, void *out_stream);

int bf_run_file(bf_s *ths, const char *command, FILE *in_file,
    bf_handler_t handler, void *out_stream);

int bf_run_memory(bf_s *ths, const char *command, const bf_byte_t *mem,
    size_t mem_size, bf_handler_t handler, void *out_stream);

int bf_run_stream(bf_s *ths, const char *command, bf_reader_t reader,
    void *in_stream, bf_handler_t handler, void *out_stream);
```

简单地讲，`bf_s` 是类，而 `bf_run_*()` 和 `bf_nearby_16()` 函数都是它的方法。

其中 `bf_run_*()` 函数都解释执行 BF 代码。BF 代码都是字符串形式传入的。

* 函数 `bf_run_filename()` 和 `bf_run_file()` 从文件获取输入流。
* 函数 `bf_run_memory()` 从内存中的数组获取输入流。
* 函数 `bf_run_stream()` 是最通用的，前三者都由它来表示。

每执行一个 `.` 命令（BF 中的输出指令），就调用 `(*handler)()` 函数，
参数就是数据指针当前所指的字节和 `out_stream`。

这和 [`benhoyt/inih`](https://github.com/benhoyt/inih) 的做法是一样的。

而 `bf_nearby_16()` 函数使用了另一个 `bf_nearby_16_s` 类来表示结果，
这个是方便用户对数据指针附近的 16 个字节做格式化，从而看到内存布局的。

详细文档请见头文件中的函数原型和结构体 `typedef` 处的注释。


## 编译

楼主是原始人，还不懂各种编译工具，只会敲 `gcc` 命令。

如果只将 `bf_test.c` 与 `bf.c` 两个文件一起编译，
你将得到一个简单的交互式 BF 解释器。

也可以不要 `bf_test.c`，而只将 `bf.c` 与你的代码一起编译，
你的代码里再包含 `bf.h`，你便能够使用里面的类型和函数。


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
