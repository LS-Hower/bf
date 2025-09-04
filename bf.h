/*
 * Copyright (C) 2024, LS Hower.
 * This project is licensed under the BSD 3-Clause License.
 * Project homepage: https://github.com/LS-Hower/bf
 */


/* 2022-12-05 bfsimu.h */
/* 2024-12-18 bf.h  */


#ifndef BF_H_INCLUDED_
#define BF_H_INCLUDED_


/* Make this header file easier to include in C++ code. */
#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>


#ifndef BF_MAX_BRACKET_DEPTH
#define BF_MAX_BRACKET_DEPTH  300
#endif


/* Data type of the memory of the virtual machine. */
typedef unsigned char bf_byte_t;

/* Typedef for fgetc-style reader function. */
typedef int (*bf_reader_t)(void *stream);

/* Typedef for fputc-style handler function. */
typedef int (*bf_handler_t)(int b, void *stream);

/*
 * The `end` pointer is past-the-end, like an iterator in C++ STL.
 * [begin, end) is the BF array (the buffer).
 * Pointer `current` is the BF data pointer.
 */
typedef struct {
    bf_byte_t        *current;
    const bf_byte_t  *begin;
    const bf_byte_t  *end;  /* past-the-end */
} bf_s;


/*
 * Initialize a `bf_s` object. The `end` pointer is past-the-end, and it should
 * meet the condition: begin < end.
 * Return -1 if begin >= end. And (*ths) wouldn't be modified in this case.
 *
 * See document of structure `bf_s`.
 */
int bf_init(bf_s *ths, bf_byte_t *begin, bf_byte_t *end);


/*
 * Run command. The virtual machine takes the file content (binary mode) as
 * input stream.
 * Each time a output command `.` is executed, call the callback
 * (*handler)(b, out_stream), where b is the byte that the `current` pointer
 * points.
 * An EOF read will be treated as a byte ((bf_byte_t) EOF) and is stored as if
 * nothing had happened.
 * Return -1 if bracket in command is nested incorrectly.
 * Return -2 if failed to open file with filename.
 * No BF command would be executed in either case;
 */
int bf_run_filename(bf_s *ths, const char *command,
    const char *filename, bf_handler_t handler, void *out_stream);

/*
 * Same as bf_run_filename(), but takes a standard file pointer instead of
 * filename.
 * This doesn't close the file when it's finished -- the caller must do that.
 */
int bf_run_file(bf_s *ths, const char *command, FILE *in_file,
    bf_handler_t handler, void *out_stream);

/*
 * Same as bf_run_filename(), but takes a zero-terminated string as the virtual
 * machine input, instead of a file.
 * End of memory will be treated as a byte ((bf_byte_t) EOF) and stored as if
 * nothing had happened.
 */
int bf_run_memory(bf_s *ths, const char *command, const bf_byte_t *mem,
    size_t mem_size, bf_handler_t handler, void *out_stream);

/*
 * Same as bf_run_filename(), but takes a function pointer instead of filename.
 * Each time a input command `,` is executed, call the callback
 * (*reader)(in_stream), and store it in where the `current` pointer points.
 * This way caller could implement custom I/O.
 * Still, end of memory will be treated as a byte ((bf_byte_t) EOF) and stored
 * as if nothing had happened.
 */
int bf_run_stream(bf_s *ths, const char *command, bf_reader_t reader,
    void *in_stream, bf_handler_t handler, void *out_stream);

/*
 * Show the memory around the current pointer.
 */
void bf_show_nearby_memory(const bf_s *ths, size_t on_the_left,
    size_t on_the_right);

#ifdef __cplusplus
} /* extern "C" */
#endif


#endif  /* BF_H_INCLUDED_ */
