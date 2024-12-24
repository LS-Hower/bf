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
 * Structure useful for formatting nearby bytes of the byte that data pointer
 * points.
 *
 * Firstly, calculate `b` to be `current` - (`current` mod 16), where `current`
 * is member of the `bf_s` object.
 *
 * then `base_ptr`:
 *   if `b` is less than `begin` member in the `bf_s` object (thus pointing an
 *   invalid address), then `base_ptr` is assigned `begin`. otherwise (`b`
 *   points a valid address), `base_ptr` is assigned `b`.
 * then `begin_offset`:
 *   assigned `base_ptr` mod 16.
 * then `current_offset`:
 *   assigned `current` mod 16.
 * then `end_offset`:
 *   (like the `begin_offset`) assigned 16 if the address there is valid.
 *   Otherwise it'll be less than 16.
 *
 * `begin_offset` is in [0, 15], most times 0, but higher than 0 if
 * `end_offset` is in [1, 16], most times 16. Also past-the-end style.
 * `current_offset` is in [0, 15].
 *
 * However, pointing to an invalid address is UB in C. So the actual code is
 * different from what is said above.
 */
typedef struct {
    const bf_byte_t  *base_ptr;
    int               begin_offset;
    int               end_offset;  /* past-the-end */
    int               current_offset;
} bf_nearby_16_s;


/*
 * Initialize a `bf_s` object. The `end` pointer is past-the-end, and it should
 * meet the condition: begin < end.
 * Return -1 if begin >= end. And (*ths) wouldn't be modified in this case.
 *
 * See document of structure `bf_s`.
 */
int bf_init(bf_s *ths, bf_byte_t *begin, bf_byte_t *end);

/*
 * Find the 16 bytes near the `current` pointer,
 * and store their address in *out.
 *
 * See document of structure `bf_nearby_16_s`.
 */
int bf_nearby_16(const bf_s *ths, bf_nearby_16_s *out);

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


#ifdef __cplusplus
} /* extern "C" */
#endif


#endif  /* BF_H_INCLUDED_ */
