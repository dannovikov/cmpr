/* #libraryintro

The spanio library.
We use spanio methods when possible and only use null-terminated C strings at interface boundaries where there is no way around it, see the s() pattern.
When we say "print" we mean what prt() does, which is append output to the output span out; it has the printf interface, i.e. format string followed by varargs.
You must flush() before the output will be printed to stdout and be visible to the user.
A common cmpr pattern is prt, flush, getch.
To "complain and exit" means prt, flush, exit(n>0).
A span has a start pointer and an end pointer, called .buf and .end respectively.
(A thran has three pointers and can be addressed as two spans which share an endpoint; just an idea at this point.)

- empty(span): If a span is empty (start and end pointers are equal).
- len(span): The length of a span. Prefer this over less clear .end minus .buf.
- init_spans(): Init global spans and buffers; called only from main().
- prt(char *, ...): Formats and appends a string to the output span, i.e. prints it. Pronounced as prt.
- prs(char *, ...): Same as prt, but returns a span (allocated in cmp space).
- w_char(char): Writes a single character.
- wrs(span): Writes the contents of a span.
- bksp(): Backspace, shortens the output span by one.
- sp(): Appends a space character to the output span, i.e. prints a space.
- terpri(): Prints a newline (name courtesy Common Lisp).
- void* out2cmp(), out_rst(void*): redirect all output functions to cmp (instead of out) and then undo (reset)
- flush(), flush_err(): Flushes the output span to standard output or standard error.
- write_to_file_span(span content, span path, int clobber): Write a span to a file, optionally overwriting.
- write_to_file(span, const char*): Deprecated.
- read_file_into_span(char*, span): Reads the contents of a file into a span. Deprecated.
- read_file_S_into_span(span, span): Read the contents of a file $1 into a span $2. Used in new code. Returns a span prefix of $2.
- read_file_into_cmp(span): Filename as a span, returns contents as a span inside cmp space.
- read_file_into_inp(span): Filename as a span, returns contents as a span inside inp space.
- advance1(span*), advance(span*, int): Advances the start pointer of a span by one or a specified number of characters.
- shorten1(span*), shorten(span*, int): Shortens a span by one or by a given number of characters.
- find_char(span, char): Searches for a character in a span and returns its first index or -1.
- contains(span, span): Checks if one span TEXTUALLY contains another; abc b O(n) string search.
- contains_ptr(span, span): Checks if one span PHYSICALLY contains another; [[]] O(1) pointer comparisons.
- starts_with(span, span): Check if $2 is textual prefix of $1 (or equal) (mnemonic: a "starts with" b).
- ends_with(span, span): Check if $1 ends with $2.
- consume_prefix(span, span*): Shortens a span by a prefix if present, returning that prefix or nullspan().
- first_n(span, int): Returns n leading chars of a span.
- skip_n(span, int): Returns a new span skipping n initial chars.
- take_n(int, span*): Returns as a new span the first n characters from a span, mutating it; often used when parsing.
- skip_whitespace(span*): modifies a span, returning a prefix span of zero or more removed whitespace.
- next_line(span*): Extracts the next line (up to \n or .end) from a span and returns it as a new span.
- span_eq(span, span), span_cmp(span, span): Compares two spans for equality or lexicographical order.
- S(char*): Creates a span from a null-terminated string.
- char* s(span): Returns a null-terminated string (in cmp space) containing the given contents.
- char* s_buffer(char*,int,span): Copies $3 into $1 (of length $2) and null-terminates it, returning $1 for convenience.
- nullspan(): Returns the empty span at address 0.
- span_arena_alloc(int),span_arena_free(): top-level setup and free spans arena (typically once per program).
- span_arena_push(),span_arena_pop(): checkpoint arena highpoint and reset it, releasing memory.
- spans spans_alloc(int): alloc a spans with the given size (.n already set).
- is_one_of(span, spans): Checks if a span textually equals one of the spans in a spans.
- index_of(span,spans): Return first element of $2 which is span_eq $1, or -1 if none match.
- spanspan(span, span): Finds the first occurrence of a span within another span and returns a span into haystack.
- w_char_esc(char), w_char_esc_pad(char), w_char_esc_dq(char), w_char_esc_sq(char), wrs_esc(): Write characters (or for wrs_esc, spans) to the output span, applying various escape sequences.
- trim(span): Gives the possibly smaller span with any isspace(3) trimmed on both sides.
- split_commas_ws(span): splits a span into a spans on commas, stripping whitespace
- split_whitespace(span): split a span into tokens on whitespace
- concat(span,span): Returns a new span (in cmp space) containing a concatenation.

typedef struct { u8* buf; u8* end; } span; // the type of span

We have a generic array implementation using arena allocation.

- Call MAKE_ARENA(E,T,STACK_SIZE) to define array type T for elements of type E.
- T will have .a of type E*, and .n, and .cap of type size_t.
- Use T_arena_alloc(N) and T_arena_free(), typically in main() or similar.
- T_alloc(N) returns an array of type T, with .n = .cap = N.
- T_arena_push() and T_arena_pop() manage arena allocation stack; use them as directed.
- Use T_push(T,E) to push an element onto an array.

This is used to declare a spans type and the associated functions.

A common idiom is to iterate over something once to count the number of spans needed, then call spans_alloc and iterate again to fill the spans.

A common idiom in functions returning span: use a span ret declared near the top, with buf pointing to one thing and end to something else found later or separately, they may be set anywhere in the function body as convenient, and the ret value is returned from one or more places.

Note that we NEVER write const in C, as this feature doesn't pull its weight.
There's some existing contamination around library functions but try to minimize the spread.

Note that prt() has exactly the same function signature as printf, i.e. it takes a format string followed by varargs.
We never use printf, but always prt.
A common idiom when reporting errors is to call prt, flush, and exit.
We could also use flush, prt, flush_err, exit, but up to now we've been lazy about the distinction between stdout and stderr as we have mainly interactive use cases.

To prt a span x we use %.*s with len(x) and x.buf.

A common idiom is next_line() in a loop with !empty().

In main() or similar it is common to call init_spans and often also read_and_count_stdin.
*/

/* #s_pattern

Note that in general our spans are NOT null-terminated, so casting a span.buf to a char* and hoping for the best in calling C library functions would be very wrong.

When we need a null-terminated C string for talking to library functions, we can use s_buffer().
We use a local buffer of some suitably generous size, according to the use case.
For example, when used for a path name we should use PATH_MAX.

Here is an example using a size of 2048:

```
char buf[2048] = {0};
s_buffer(buf,2048,some_span);
... use 'buf' ...
```
*/
/* #jsonlib

JSON support in the spanio library.

- json_s(span): prt a JSON string (double-quoted and escaped appropriately).
- json_n(f64): prt a double in JSON format.
- json_b(int): prt a true or false (only 0 is false).
- json_0(): prt a json_null value ("null").
- json_o(): prt an empty json object.
- json_o_extend(json*,span,json): extends $1 with key $2 and value $3.
- json_a(): prt an empty json array.
- json_a_extend(json*,json): extends $1 with key $2.
- all the above json constructor functions return the json type (which they also prt, usually this is sent to cmp space) as in the _{s,n,b,0,o,a} constructors.
- json_{s,n,b,0,o,a}p: full list of json_?p predicate funcs, used to distinguish types of json values.
  - (for example) json_sp(json): 1 if $1 is a string, otherwise 0.
- json_key(span, json): lookup on json object.
- json_index(int, json): lookup on json array.
- above lookup functions return a "nullable json".
- mnemonic: the argument order was inspired by partial application.
- int json_is_null(json): returns 0 or 1.
- json_un_s(json): return a span containing the actual value of a json string value (e.g. from json_key or json_index).
- json_parse(span): parse a span into a json and return it; may be shorter only by trimmed whitespace; commonly used.
- make_json(span): return a json wrapper of the span in O(1); the span must be known to be valid json already; rarely used.
- json_s2s(json,span*,u8*): converts json string $1 into an unquoted string in $2 (not exceeding buffer end $3); returns a span.
- json_parse_prefix(span*): not usually called directly, but can be used to parse a json value off the front of a buffer, shortening it.

The json type is a wrapped span which actually contains a JSON-formatted string, allocated in cmp space.
Every json value wraps a span .s, which can be accessed directly whenever the string value of the json is needed, for example when sending as JSON over the wire.

There are constructor functions for all the primitive types, and for the collection types, array and object, there are constructors for the empty collections and extend functions to extend them.
These extend them in place, and are intended for relatively simple applications like building a message for an API call.

There are predicate functions for the json type that distinguish between numbers, arrays, and so on.
(Since the json type just wraps an actual JSON string, these work by looking at the first character of that string, which is definitive; this implies that the json type doesn't include leading or trailing whitespace.)

JSON defines a literal "null" value, but we define a separate "nulljson" distinguished signal value, testable by json_is_null, which indicates some kind of hard failure.
It is returned by all the json-returning functions that can fail, such as indexing an array or object, or parsing a JSON string.
It is simply the json type wrapping a nullspan (the span having .buf = .end = 0).

The function make_json is rarely used and is for "casting" a span to a json object in constant time.
It is normally only used internally in library methods, but can be used if you know you have a JSON string and don't want to parse it again.

The json indexing functions return the json type; that is, the contents are still valid JSON.
In particular, a JSON string will contain JSON string escaping.
If you want the actual string value, you can use json_un_s, which returns a new span in cmp space.
(This uses the lower-level json_s2s, which has a less convenient interface.)

(We should probably have a similar function for getting a number out, but it hasn't been added yet.)
*/
/* json library design notes

- all the json constructor functions trim whitespace, so that all the predicate functions follow a pointer and examine one byte.
- the json parser and constant-time wrapper functions are the low-trust and high-trust ways to make a json from a string.
*/
/* includes */

#define _GNU_SOURCE // for memmem
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdarg.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>
#include <ctype.h>
#include <limits.h>
#include <termios.h>
#include <errno.h>
#include <time.h>
#include <math.h>
/* convenient debugging macros */
#define dbgd(x) prt(#x ": %d\n", x),flush()
#define dbgx(x) prt(#x ": %x\n", x),flush()
#define dbgf(x) prt(#x ": %f\n", x),flush()
#define dbgp(x) prt(#x ": %p\n", x),flush()

typedef unsigned char u8;

/* #span

Our string type.

A span is two pointers.
Buf points to the first char included in the string.
End points to the first char excluded after the string's end.

If these two pointers are equal, the string is empty, but it still points to a location.

So two empty spans are not necessarily the same span, while two empty strings are.
Neither spans nor their contents are immutable, but everything depends on intended use.

These two pointers must point into some space that has been allocated somewhere.

The inp variable is the span which writes into input_space, and then is the immutable copy of stdin for the duration of the process.
The number of bytes of input is len(inp).
The output is stored in span out, which points to output_space.
Input processing is generally by reading out of the inp span or subspans of it.
The output spans are mostly written to with prt() and other IO functions.
The cmp_space and cmp span which points to it are used for analysis and model data, both reading and writing.
These are just the common conventions; your program may use them differently.

When writing output, we often see prt followed by flush.
Flush sends to stdout the contents of out (the output span) that have not already been sent.
Usually it is important to do this
- before any operation that blocks, when the user should see the output that we've already written,
- generally immediately after prt when debugging anything,
- after printing any error message and before exiting the program, and
- at the end of main.

If you want to write to stderr, you can use flush_err(), which also flushes from the output_space but to stderr instead of stdout.
(You may need to do a flush() before the call to prt() if you already have pending output that needs to go to stdout.)
*/

typedef struct {
  u8 *buf;
  u8 *end;
} span;

#define BUF_SZ (1 << 30)

u8 *input_space; // remains immutable once stdin has been read up to EOF.
u8 *output_space;
u8 *cmp_space;
span out, inp, cmp;
span* outp;

int empty(span);
int len(span);

void init_spans(); // main spanio init function

// basic spanio primitives

void prt(const char *, ...);
void w_char(char);
void wrs(span);
void bksp();
void sp();
void terpri();
void w_char_esc(char);
void w_char_esc_pad(char);
void w_char_esc_dq(char);
void w_char_esc_sq(char);
void wrs_esc(span);
void* out2cmp(); // redirect all output functions (prt, wrs, etc) to cmp instead of out
void out_rst(void*); // undo effect of out2cmp
void flush();
void flush_err();
void write_to_file(span content, const char* filename);
span read_file_into_span(char *filename, span buffer);
span read_file_S_into_span(span filename, span buffer);
span read_file_into_cmp(span filename);
//void redir(span);
//span reset();
//void save();
//void push(span);
//void pop(span*);
void advance1(span*);
void advance(span*,int);
//span pop_into_span();
int find_char(span s, char c);
int contains(span, span);
span take_n(int, span*);
span next_line(span*);
span first_n(span, int);
int span_eq(span, span);
int span_cmp(span, span);
span S(char*);
span nullspan();

 /*
typedef struct {
  span *s; // array of spans (points into span arena)
  int n;   // length of array
} spans;

#define SPAN_ARENA_STACK 256

span* span_arena;
int span_arenasz;
int span_arena_used;
int span_arena_stack[SPAN_ARENA_STACK];
int span_arena_stack_n;

void span_arena_alloc(int);
void span_arena_free();
void span_arena_push();
void span_arena_pop();
*/

span inp_compl();
span cmp_compl();
span out_compl();
/* input statistics on raw bytes; span basics */

int counts[256] = {0};

void read_and_count_stdin(); // populate inp and counts[]
int empty(span s) {
  return s.end == s.buf;
}


inline int len(span s) { return s.end - s.buf; }

u8 in(span s, u8* p) { return s.buf <= p && p < s.end; }

int out_WRITTEN = 0, cmp_WRITTEN = 0;

void init_spans() {
  input_space = malloc(BUF_SZ);
  output_space = malloc(BUF_SZ);
  cmp_space = malloc(BUF_SZ);
  out.buf = output_space;
  out.end = output_space;
  inp.buf = input_space;
  inp.end = input_space;
  cmp.buf = cmp_space;
  cmp.end = cmp_space;
  outp = &out;
}

void bksp() { (*outp).end--; }

void sp() { w_char(' '); }

// we might have the same kind of redir_i() as we have redir() already, where we redirect input to come from a span and then use standard functions like take() and get rid of these special cases for taking input from streams or spans.

span head_n(int n, span *io) {
  span ret;
  ret.buf = io->buf;
  ret.end = io->buf + n;
  io->buf += n;
  return ret;
}

int span_eq(span s1, span s2) {
  if (len(s1) != len(s2)) return 0;
  for (int i = 0; i < len(s1); ++i) if (s1.buf[i] != s2.buf[i]) return 0;
  return 1;
}

int span_cmp(span s1, span s2) {
  for (;;) {
    if (empty(s1) && !empty(s2)) return 1;
    if (empty(s2) && !empty(s1)) return -1;
    if (empty(s1)) return 0;
    int dif = *(s1.buf++) - *(s2.buf++);
    if (dif) return dif;
  }
}

span S(char *s) {
  span ret = {(u8*)s, (u8*)s + strlen(s) };
  return ret;
}

char* s_buffer(char* buf, int n, span s) {
  size_t l = (n - 1) < len(s) ? (n - 1) : len(s);
  memmove(buf, s.buf, l);
  buf[l] = '\0';
  return buf;
}

char* s(span s) {
  if (len(s) && s.end[-1] == '\0') return (char*)s.buf;
  char* ret = (char*)cmp.end;
  void* o = out2cmp();
  wrs(s);
  w_char('\0');
  out_rst(o);
  return ret;
}

void read_and_count_stdin() {
  int c;
  while ((c = getchar()) != EOF) {
    //if (c == ' ') continue;
    assert(c != 0);
    counts[c]++;
    *inp.buf = c;
    inp.buf++;
    if (len(inp) == BUF_SZ) { prt("input overflow\n"); flush_err(); exit(1); }
  }
  inp.end = inp.buf;
  inp.buf = input_space;
}

/*
span saved_out[16] = {0};
int saved_out_stack = 0;

void redir(span new_out) {
  assert(saved_out_stack < 15);
  saved_out[saved_out_stack++] = out;
  out = new_out;
}

span reset() {
  assert(saved_out_stack);
  span ret = out;
  out = saved_out[--saved_out_stack];
  return ret;
}
*/

// set if debugging some crash
const int ALWAYS_FLUSH = 0;

// Note: this doesn't swap output_space, which means manual comparisons with output_space + BUF_SZ will be broken?
// probably an argument for the "thran"
// actually we should just be using out.buf + BUF_SZ anyway I suppose
//void swapcmp() { span swap = cmp; cmp = out; out = swap; int swpn = cmp_WRITTEN; cmp_WRITTEN = out_WRITTEN; out_WRITTEN = swpn; }
//void prt2cmp() { if (out.buf == output_space) swapcmp(); }
//void prt2std() { if (out.buf == cmp_space) swapcmp(); }

//span prt_cmp_stack[1024] = {0};
//int prt_cmp_stack_n = 0;
//void prt_cmp() { assert(prt_cmp_stack_n < 1023); prt_cmp_stack[prt_cmp_stack_n++] = out; out = cmp; }
//void prt_pop() { assert(0 < prt_cmp_stack_n); out = prt_cmp_stack[--prt_cmp_stack_n]; }

void* out2cmp() { void* ret = outp; outp = &cmp; return ret; }
void out_rst(void* op) { outp = (span*) op; }

void prt(const char * fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  char *buffer;
  // we used to use vsprintf here, but that adds a null byte that we don't want
  int n = vasprintf(&buffer, fmt, ap);
  memcpy(outp->end, buffer, n);
  free(buffer);
  outp->end += n;
  if (outp->buf + BUF_SZ < outp->end) {
    printf("OUTPUT OVERFLOW (%ld)\n", outp->end - outp->buf);
    exit(7);
  }
  va_end(ap);
  if (ALWAYS_FLUSH) flush();
}

span prs(char * fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  span ret = { .buf = cmp.end };
  char *buffer;
  // we used to use vsprintf here, but that adds a null byte that we don't want
  int n = vasprintf(&buffer, fmt, ap);
  if (BUF_SZ < len(cmp) + n) {
    printf("CMP OVERFLOW (%d)\n", len(cmp) + n);
    exit(7);
  }
  memcpy(cmp.end, buffer, n);
  free(buffer);
  cmp.end += n;
  va_end(ap);
  if (ALWAYS_FLUSH) flush();
  ret.end = cmp.end;
  return ret;
}

void terpri() {
  *outp->end = '\n';
  outp->end++;
  if (ALWAYS_FLUSH) flush();
}

void w_char(char c) {
  *outp->end++ = c;
}

void w_char_esc(char c) {
  if (c < 0x20 || c == 127) {
    outp->end += sprintf((char*)outp->end, "\\%03o", (u8)c);
  } else {
    *outp->end++ = c;
  }
}

void w_char_esc_pad(char c) {
  if (c < 0x20 || c == 127) {
    outp->end += sprintf((char*)outp->end, "\\%03o", (u8)c);
  } else {
    sp();sp();sp();
    *outp->end++ = c;
  }
}

void w_char_esc_dq(char c) {
  if (c < 0x20 || c == 127) {
    outp->end += sprintf((char*)outp->end, "\\%03o", (u8)c);
  } else if (c == '"') {
    *outp->end++ = '\\';
    *outp->end++ = '"';
  } else if (c == '\\') {
    *outp->end++ = '\\';
    *outp->end++ = '\\';
  } else {
    *outp->end++ = c;
  }
}

void w_char_esc_sq(char c) {
  if (c < 0x20 || c == 127) {
    outp->end += sprintf((char*)outp->end, "\\%03o", (u8)c);
  } else if (c == '\'') {
    *outp->end++ = '\\';
    *outp->end++ = '\'';
  } else if (c == '\\') {
    *outp->end++ = '\\';
    *outp->end++ = '\\';
  } else {
    *outp->end++ = c;
  }
}

void wrs(span s) {
  for (u8 *c = s.buf; c < s.end; c++) w_char(*c);
}

void wrs_esc(span s) {
  for (u8 *c = s.buf; c < s.end; c++) w_char_esc(*c);
}

void flush() {
  int *WRITTEN = (output_space < outp->end && outp->end < output_space + BUF_SZ) ? &out_WRITTEN : &cmp_WRITTEN;
  if (*WRITTEN < len(*outp)) {
    printf("%.*s", len(*outp) - *WRITTEN, outp->buf + *WRITTEN);
    *WRITTEN = len(*outp);
    fflush(stdout);
  }
}

void flush_err() {
  int *WRITTEN = (output_space < outp->end && outp->end < output_space + BUF_SZ) ? &out_WRITTEN : &cmp_WRITTEN;
  if (*WRITTEN < len(out)) {
    fprintf(stderr, "%.*s", len(*outp) - *WRITTEN, outp->buf + *WRITTEN);
    *WRITTEN = len(*outp);
    fflush(stderr);
  }
}

   /*
In write_to_file we open a file, which must not exist, and write the contents of a span into it, and close it.
If the file exists or there is any other error, we prt(), flush(), and exit as per usual.

In write_to_file_span we simply take the same two arguments but the filename is a span.
We build a null-terminated string and call write_to_file.
*/

void write_to_file_2(span, const char*, int);

void write_to_file(span content, const char* filename) {
  write_to_file_2(content, filename, 0);
}

void write_to_file_2(span content, const char* filename, int clobber) {
  // Attempt to open the file with O_CREAT and O_EXCL to ensure it does not already exist
  /* clobber thing is a manual fixup */
  int flags = O_WRONLY | O_CREAT | O_TRUNC;
  if (!clobber) flags |= O_EXCL;
  int fd = open(filename, flags, 0644);
  if (fd == -1) {
    if (clobber) {
      prt("Error opening %s for writing: File cannot be created or opened.\n", filename);
    } else {
      prt("Error opening %s for writing: File already exists or cannot be created.\n", filename);
    }
    flush();
    exit(EXIT_FAILURE);
  }

  // Write the content of the span to the file
  ssize_t written = write(fd, content.buf, len(content));
  if (written != len(content)) {
    // Handle partial write or write error
    prt("Error writing to file %s.\n", filename);
    flush();
    close(fd); // Attempt to close the file before exiting
    exit(EXIT_FAILURE);
  }

  // Close the file
  if (close(fd) == -1) {
    prt("Error closing %s after writing.\n", filename);
    flush();
    exit(EXIT_FAILURE);
  }
}

void write_to_file_span(span content, span filename_span, int clobber) {
  char filename[filename_span.end - filename_span.buf + 1];
  memcpy(filename, filename_span.buf, filename_span.end - filename_span.buf);
  filename[filename_span.end - filename_span.buf] = '\0';
  write_to_file_2(content, filename, clobber);
}

// Function to print error messages and exit (never write "const" in C)
void exit_with_error(char *error_message) {
  perror(error_message);
  exit(EXIT_FAILURE);
}

/* not really any better for usability I think
span read_f_into_span(span filename, span* buffer) {
  span ret = read_file_S_into_span(filename, *buffer);
  buffer->buf = ret.end;
}
*/

span read_file_into_cmp(span filename) {
  span ret = read_file_S_into_span(filename, cmp_compl());
  cmp.end = ret.end;
  return ret;
}

span read_file_S_into_span(span filename, span buffer) {
  char path[2048];
  s_buffer(path,2048,filename);
  return read_file_into_span(path, buffer);
}

span read_file_into_span(char* filename, span buffer) {
  // Open the file
  int fd = open(filename, O_RDONLY);
  if (fd == -1) {
    prt("Failed to open %s\n", filename);
    flush();
    exit(1);
  }

  // Get the file size
  struct stat statbuf;
  if (fstat(fd, &statbuf) == -1) {
    close(fd);
    prt("Failed to get file size for %s\n", filename);
    flush();exit(1);
  }

  // Check if the file's size fits into the provided buffer
  size_t file_size = statbuf.st_size;
  if (file_size > len(buffer)) {
    close(fd);
    exit_with_error("File content does not fit into the provided buffer");
  }

  // Read file contents into the buffer
  ssize_t bytes_read = read(fd, buffer.buf, file_size);
  if (bytes_read == -1) {
    close(fd);
    exit_with_error("Failed to read file contents");
  }

  // Close the file
  if (close(fd) == -1) {
    exit_with_error("Failed to close file");
  }

  // Create and return a new span that reflects the read content
  span new_span = {buffer.buf, buffer.buf + bytes_read};
  return new_span;
}

/*
u8 *save_stack[16] = {0};
int save_count = 0;

void save() {
  push(out);
}

span pop_into_span() {
  span ret;
  ret.buf = save_stack[--save_count];
  ret.end = out.end;
  return ret;
}

void push(span s) {
  save_stack[save_count++] = s.buf;
}

void pop(span *s) {
  s->buf = save_stack[--save_count];
}
*/

   /*
take_n is a mutating function which takes the first n chars of the span into a new span, and also modifies the input span to remove this same prefix.
After a function call such as `span new = take_n(x, s)`, it will be the case that `new` contatenated with `s` is equivalent to `s` before the call.
*/

span take_n(int n, span *io) {
  span ret;
  ret.buf = io->buf;
  ret.end = io->buf + n;
  io->buf += n;
  return ret;
}

void advance1(span *s) {
  if (!empty(*s)) s->buf++;
}

void advance(span *s, int n) {
  if (len(*s) >= n) s->buf += n;
  else s->buf = s->end; // Move to the end if n exceeds span length
}

void shorten1(span *s) {
  if (!empty(*s)) s->end--;
}

void shorten(span *s, int n) {
  if (n <= len(*s)) s->end -= n;
  else s->end = s->buf;
}

int contains(span haystack, span needle) {
  /*
  prt("contains() haystack:\n");
  wrs(haystack);terpri();
  prt("needle:\n");
  wrs(needle);terpri();
  */
  if (len(haystack) < len(needle)) {
    return 0; // Needle is longer, so it cannot be contained
  }
  void *result = memmem(haystack.buf, haystack.end - haystack.buf, needle.buf, needle.end - needle.buf);
  return result != NULL ? 1 : 0;
}

int contains_ptr(span a, span b) {
  return a.buf <= b.buf && b.end <= a.end;
}

int starts_with(span a, span b) {
  return len(b) <= len(a) && 0 == memcmp(a.buf, b.buf, len(b));
}

int ends_with(span a, span b) {
  return len(b) <= len(a) && 0 == memcmp(a.end - len(b), b.buf, len(b));
}

span first_n(span s, int n) {
  span ret;
  if (len(s) < n) n = len(s); // Ensure we do not exceed the span's length
  ret.buf = s.buf;
  ret.end = s.buf + n;
  return ret;
}

span skip_n(span s, int n) {
  if (len(s) <= n) return (span){s.end, s.end};
  return (span){s.buf + n, s.end};
}

void skip_whitespace(span *s) {
  while (isspace(*s->buf)) s->buf++;
}

int find_char(span s, char c) {
  for (int i = 0; i < len(s); ++i) {
    if (s.buf[i] == c) return i;
  }
  return -1; // Character not found
}

span trim(span s) {
  while (len(s) && isspace((unsigned char)*s.buf)) s.buf++;
  while (len(s) && isspace((unsigned char)*(s.end - 1))) s.end--;
  return s;
}

span concat(span a, span b) {
  span ret = {cmp.end};
  void* o = out2cmp();
  wrs(a);
  wrs(b);
  out_rst(o);
  ret.end = cmp.end;
  return ret;
}
/* next_line(span*) shortens the input span and returns the first line as a new span.
The newline is consumed and is not part of either the returned span or the input span after the call.
I.e. the total len of the shortened input and the returned line is one less than the len of the original input.
If there is no newline found, then the entire input is returned.
In this case the input span is mutated such that buf now points to end.
This makes it an empty span and thus a null span in our nomenclature, but it is still an empty span at a particular location.
This convention of empty but localized spans allows us to perform comparisons without needing to handle them differently in the case of an empty span.
*/

span next_line(span *input) {
  if (empty(*input)) return nullspan();
  span line;
  line.buf = input->buf;
  while (input->buf < input->end && *input->buf != '\n') {
    input->buf++;
  }
  line.end = input->buf;
  if (input->buf < input->end) { // If '\n' found, move past it for next call
    input->buf++;
  }
  return line;
}

/* 
In consume_prefix(span*,span) we are given a span which is typically something being parsed and another span which is expected to be a prefix of it.
If the prefix is found, we return it and modify the span that is being parsed to remove the prefix.
Otherwise we leave that span unmodified and return nullspan().
Typical use is in an if statement to either identify and consume some prefix and then continue on to handle what follows it, or otherwise to skip the if and continue parsing the unmodified input.
We return the span that points into the input in case the caller has some use for it.
*/

span consume_prefix(span prefix, span *input) {
  if (len(*input) < len(prefix) || !span_eq(first_n(*input, len(prefix)), prefix)) {
    return nullspan();
  }
  span ret = {.buf = input->buf};
  input->buf += len(prefix);
  ret.end = input->buf;
  return ret;
}
/*
The spans arena implementation.
*/

//spans spans_alloc(int n);

/*void span_arena_alloc(int sz) {
  span_arena = malloc(sz * sizeof *span_arena);
  span_arenasz = sz;
  span_arena_used = 0;
  span_arena_stack_n = 0;
}
void span_arena_free() {
  free(span_arena);
}
void span_arena_push() {
  assert(span_arena_stack_n < SPAN_ARENA_STACK);
  span_arena_stack[span_arena_stack_n++] = span_arena_used;
}
void span_arena_pop() {
  assert(0 < span_arena_stack_n);
  span_arena_used = span_arena_stack[--span_arena_stack_n];
}
spans spans_alloc(int n) {
  assert(span_arena);
  spans ret = {0};
  ret.s = span_arena + span_arena_used;
  ret.n = n;
  span_arena_used += n;
  assert(span_arena_used < span_arenasz);
  return ret;
}
*/

/* Generic arrays.

Here we have a macro that we can call with two type names (i.e. typedefs) and a number.
One is an already existing typedef and another will be created by the macro, and the number is the size of a stack, described below.

For example, to create the spans type we might call this macro with span and spans as the names.
We call these the element type and array type names resp.
(We may use "E" and "T" as variables in documentation to refer to them.)

This macro will create a typedef struct with that given name that has a pointer to the element type called "a", a number of elements, which is always called "n", and a capacity "cap", which are size_t's.

We don't implement realloc for simplicity and improved memory layout.
We use an arena allocation pattern.
Because we don't realloc, a common pattern is to make an initial loop over something to count some required number of elements and then to call the alloc function to get an array of the precise size and then to have a second loop where we populate that array.

For every generic array type that we make, we will have:

- A setup function T_arena_alloc(N) for the arena, which takes a number (as int) and allocates (using malloc) enough memory for that many of the element type, where T is the array type name.
- A corresponding T_arena_free().
- A pair T_arena_push() and T_arena_pop().
- A function T_alloc(N) which returns a T, having cap of N.
- T_push(T*,E) which increments n, complains and exits if cap is reached, and stores the element provided.

The implementation makes a single global struct (both the typedef and the singleton instance) that holds the arena state for the array type.
This includes the arena pointer, the arena size in elements, the number of allocated elements, and a stack of such numbers.
The stack size is also an argument to the macro.
We do not support realloc on the entire arena, rather the programmer needs to choose a big enough value and if we exceed at runtime we will always crash.
The programmer has to call the T_arena_alloc(N) and _free methods themselves, usually in a main() function or similar, and if the function is not called the arena won't be initialized and T_alloc() will always complain and crash (using prt, flush, exit as usual).

The main entry point is the MAKE_ARENA(E,T) macro, which sets up everything and must be called before any references to T in the source code.
Then the arena alloc and free functions must be called somewhere, and everything is ready to use.
*/

#define MAKE_ARENA(E, T, STACK_SIZE) \
typedef struct { \
    E* a; \
    size_t n, cap; \
} T; \
\
static struct { \
    E* arena; \
    size_t arena_size, allocated, stack[STACK_SIZE], stack_top; \
} T##_arena = {0}; \
\
void T##_arena_alloc(int N) { \
    T##_arena.arena = (E*)malloc(N * sizeof(E)); \
    if (!T##_arena.arena) { \
        prt("Failed to allocate arena for " #T "\n", 0); \
        flush(); \
        exit(1); \
    } \
    T##_arena.arena_size = N; \
    T##_arena.allocated = 0; \
    T##_arena.stack_top = 0; \
} \
\
void T##_arena_free() { \
    free(T##_arena.arena); \
} \
\
void T##_arena_push() { \
    if (T##_arena.stack_top == STACK_SIZE) { \
        prt("Exceeded stack size for " #T "\n", 0); \
        flush(); \
        exit(1); \
    } \
    T##_arena.stack[T##_arena.stack_top++] = T##_arena.allocated; \
} \
\
void T##_arena_pop() { \
    if (T##_arena.stack_top == 0) { \
        prt("Stack underflow for " #T "\n", 0); \
        flush(); \
        exit(1); \
    } \
    T##_arena.allocated = T##_arena.stack[--T##_arena.stack_top]; \
} \
\
T T##_alloc(size_t N) { \
    if (T##_arena.allocated + N > T##_arena.arena_size) { \
        prt("Arena overflow for " #T "\n", 0); \
        flush(); \
        exit(1); \
    } \
    T result; \
    result.a = T##_arena.arena + T##_arena.allocated; \
    result.n = 0; \
    result.cap = N; \
    T##_arena.allocated += N; \
    return result; \
} \
\
void T##_push(T* t, E e) { \
    if (t->n == t->cap) { \
        prt("Capacity reached for " #T "\n", 0); \
        flush(); \
        exit(1); \
    } \
    t->a[t->n++] = e; \
}

/*
Our first generic array is spans, which has a stack depth of 256.

*/

MAKE_ARENA(span,spans,256);

/*
Other stuff.
*/

int bool_neq(int, int);
span spanspan(span haystack, span needle);
int is_one_of(span x, spans ys);

span nullspan() {
  return (span){0, 0};
}

int bool_neq(int a, int b) { return ( a || b ) && !( a && b); }

spans split_commas_ws(span s) {
  int n_commas = 0;
  for (int i=0;i<len(s);i++) {
    if (s.buf[i] == ',') n_commas++;
  }
  spans ret = spans_alloc(n_commas + 1);
  //int idx = 0;
  while (len(s)) {
    int comma = find_char(s,',');
    if (comma < 0) {
      //ret.a[idx++] = trim(s);
      spans_push(&ret,trim(s));
      break;
    } else {
      spans_push(&ret,trim(first_n(s,comma)));
      //ret.a[idx++] = trim(first_n(s,comma));
      s = skip_n(s, comma+1);
    }
  }
  return ret;
}

spans split_whitespace(span s) {
  int n_tokens = 0;
  for (int i=0;i<len(s);i++) {
    if (!isspace(s.buf[i]) && (i == 0 || isspace(s.buf[i-1]))) n_tokens++;
  }
  spans ret = spans_alloc(n_tokens);
  int idx = 0;
  while (len(s)) {
    while (len(s) && isspace(*s.buf)) s.buf++;
    if (!len(s)) break;
    span tok = {.buf = s.buf};
    while (len(s) && !isspace(*s.buf)) s.buf++;
    tok.end = s.buf;
    ret.a[idx++] = tok;
  }
  ret.n = idx;
  return ret;
}
/* #json

JSON library

*/

typedef struct {
  span s;
} json;

int json_is_null(json);

// constructors
json json_s(span);
json json_n(double);
json json_b(int);
json json_0();
json json_o();
json json_a();
json nulljson();

// extraction
span json_un_s(json);
span json_s2s(json,span*,u8*);

// extend
void json_o_extend(json*,span,json);
void json_a_extend(json*,json);

// predicates
int json_sp(json);
int json_np(json);
int json_bp(json);
int json_0p(json);
int json_op(json);
int json_ap(json);

// lookups
json json_key(span, json);
json json_index(int, json);

// from spans
json json_parse(span);
json make_json(span);
json json_parse_prefix(span*);
json json_parse_prefix_string(span*);
json json_parse_prefix_number(span*);
json json_parse_prefix_littok(span*);

// implementation

int json_is_null(json j) { return !j.s.buf; }

json json_s(span s) {
  void* out = out2cmp();
  json ret = {0};
  ret.s.buf = cmp.end;
  prt("\"");
  for (u8* p=s.buf;p<s.end;p++) {
    switch (*p) {
      case '\b':
        prt("\\b");
      case '\f':
        prt("\\f");
      case '\n':
        prt("\\n");
        break;
      case '\r':
        prt("\\r");
      case '\t':
        prt("\\t");
      case '"':
        prt("\\\"");
        break;
      case '\\':
        prt("\\\\");
        break;
      default:
        if (iscntrl(*p)) {
          prt("\\u%04X", *p);
        }
        w_char(*p);
    }
  }
  prt("\"");
  ret.s.end = cmp.end;
  out_rst(out);
  return ret;
}

json json_n(double n) {
  void *rst = out2cmp();
  json ret = {.s = {.buf = cmp.end }};
  prt("%F", n);
  ret.s.end = cmp.end;
  out_rst(rst);
  return ret;
}

json json_b(int b) {
  void *rst = out2cmp();
  json ret = {.s = {.buf = cmp.end }};
  if (b) prt("true"); else prt("false");
  ret.s.end = cmp.end;
  out_rst(rst);
  return ret;
}

json json_0() {
  void *rst = out2cmp();
  json ret = {.s = {.buf = cmp.end }};
  prt("null");
  ret.s.end = cmp.end;
  out_rst(rst);
  return ret;
}

json json_o() {
  void *rst = out2cmp();
  json ret = {.s = {.buf = cmp.end }};
  prt("{}");
  ret.s.end = cmp.end;
  out_rst(rst);
  return ret;
}

void json_o_extend(json *j, span key, json val) {
  void *rst = out2cmp();
  u8* keybuf = malloc(len(key));
  u8* valbuf = malloc(len(val.s));
  memcpy(keybuf, key.buf, len(key));
  memcpy(valbuf, val.s.buf, len(val.s));
  span key2 = {keybuf, keybuf + len(key)};
  span val2 = {valbuf, valbuf + len(val.s)};
  cmp.end = j->s.end;
  bksp();
  if (*(cmp.end - 1) != '{') prt(",");
  //wrs(key2);
  json_s(key2);
  prt(":");
  wrs(val2);
  prt("}");
  j->s.end = cmp.end;
  free(keybuf);
  free(valbuf);
  out_rst(rst);
}

json json_a() {
  void *rst = out2cmp();
  json ret = {.s = {.buf = cmp.end }};
  prt("[]");
  ret.s.end = cmp.end;
  out_rst(rst);
  return ret;
}

void json_a_extend(json *a, json val) {
  void *rst = out2cmp();
  cmp.end = a->s.end;
  bksp();
  if (*(cmp.end - 1) != '[') prt(",");
  wrs(val.s);
  prt("]");
  a->s.end = cmp.end;
  out_rst(rst);
}

json nulljson() { return (json) {nullspan()}; }

int json_sp(json j) { return j.s.buf && *j.s.buf == '"'; }
int json_np(json j) {
  if (!j.s.buf) return 0;
  switch(*j.s.buf) {
    case '-':
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
      return 1;
    default:
      return 0;
  }
}
int json_bp(json j) { return j.s.buf && (*j.s.buf == 't' || *j.s.buf == 'f'); }
int json_0p(json j) { return j.s.buf && *j.s.buf == 'n'; }
int json_op(json j) { return j.s.buf && *j.s.buf == '{'; }
int json_ap(json j) { return j.s.buf && *j.s.buf == '['; }

json json_index(int n, json a) {
  json ret = {0};
  a.s.buf++;
  while (*a.s.buf != ']') {
    skip_whitespace(&a.s);
    ret = json_parse_prefix(&a.s);
    if (json_is_null(ret)) return nulljson();
    if (!n--) return ret;
    skip_whitespace(&a.s);
    if (*a.s.buf != ',') return nulljson();
    a.s.buf++;
  };
  return nulljson();
}

json json_key(span s, json o) {
  o.s.buf++;
  while (*o.s.buf != '}') {
    skip_whitespace(&o.s);
    json key = json_parse_prefix(&o.s);
    if (json_is_null(key)) return key;
    skip_whitespace(&o.s);
    if (*(o.s.buf++) != ':') return nulljson();
    skip_whitespace(&o.s);
    json value = json_parse_prefix(&o.s);
    if (json_is_null(value)) return nulljson();
    span key_s = json_s2s(key, &cmp, cmp_space + BUF_SZ);
    if (span_eq(key_s, s)) return value;
    skip_whitespace(&o.s);
    if (*(o.s.buf++) != ',') return nulljson();
    skip_whitespace(&o.s);
  }
  return nulljson();
}

json make_json(span s) { return (json){s}; }

span json_un_s(json s) {
  return json_s2s(s, &cmp, cmp_space + BUF_SZ);
}
/*
The `json_parse` function takes a `span` representing the JSON data and returns a `json` type object, unless the parse failed or did not consume the entire input (excepting whitespace) in which case it returns nulljson().
*/

json json_parse(span s) {
  skip_whitespace(&s);
  json ret = json_parse_prefix(&s);
  skip_whitespace(&s);
  if (empty(s)) return ret;
  return nulljson();
}
/*
The json_parse_prefix function takes a span and parses as much of it as it can as a json, then leaves the rest, and returns a json which is null only if the parse failed.

In fact, the span is passed in by reference and we modify it, shortening it from the front.
If the parse fails the input span may be modified.

We declare a json return value `ret`.
This will include the first non-whitespace byte that we consume up to the end of the complete JSON value by the time we return it, or we will indicate failure and not return it.

First we strip any whitespace by calling skip_whitespace on the input span.
Then we set ret.s.buf from the input as if we succeed this is the only value it can have.
After this point, if we return successfully we will always set ret.s.end to be the same as .buf of the input span when we are returning.
I.e. the json that we return always covers the prefix that we have parsed up to what is left in the input.

Then we switch on the first non-ws char of the input, and then we either:

- call json_parse_prefix_string
- call json_parse_prefix_number
- directly parse a true/false/null
- directly parse an object or array

To directly parse an object, we first consume the "{", then call json_parse_prefix_string.
If this returns a null json it means we failed and we return the null json.
Otherwise we continue by skipping whitespace, consuming the ":" and then recursively calling json_parse_prefix to consume the value.
Once again, if the value is the null json then we failed and return the null json.
Otherwise, we consume (whitespace and) either another comma, going around the loop again, we exit the loop, and then outside that consume (any whitespace and) the final "}" and return successfully.

To directly parse an array we do something similar but without the keys, just handling the commas and values.

To directly parse true/false/null, we call consume_prefix with the appropriate string.
This returns a span, which we put in a variable; if it is the null span we return nulljson().
Otherwise, it will be a span pointing into the span that we are parsing, for this reason we return the same span returned from consume_prefix, just wrapping it with a call to make_json first.
*/

json json_parse_prefix(span *input) {
    json ret = {0};
    //skip_whitespace(input);
    ret.s.buf = input->buf;

    char first_char = *input->buf;
    switch (first_char) {
        case '\"':
            ret = json_parse_prefix_string(input);
            break;
        case '-':
        case '0' ... '9':
            ret = json_parse_prefix_number(input);
            break;
        case 't':
        case 'f':
        case 'n':
            ret = json_parse_prefix_littok(input);
            break;
        case '{':
            input->buf++; // consume '{'
            skip_whitespace(input);
            while (*input->buf != '}') {
                json key = json_parse_prefix_string(input);
                if (key.s.buf == NULL) return nulljson();
                skip_whitespace(input);
                if (*input->buf != ':') return nulljson();
                input->buf++; // consume ':'
                skip_whitespace(input);
                json value = json_parse_prefix(input);
                if (value.s.buf == NULL) return nulljson();
                skip_whitespace(input);
                if (*input->buf == ',') input->buf++; // consume ','
                skip_whitespace(input);
            }
            if (*input->buf == '}') input->buf++; // consume '}'
            else return nulljson();
            break;
        case '[':
            input->buf++; // consume '['
            skip_whitespace(input);
            while (*input->buf != ']') {
                json value = json_parse_prefix(input);
                if (value.s.buf == NULL) return nulljson();
                skip_whitespace(input);
                if (*input->buf == ',') input->buf++; // consume ','
                skip_whitespace(input);
            }
            if (*input->buf == ']') input->buf++; // consume ']'
            else return nulljson();
            break;
        default:
            return nulljson();
    }
    ret.s.end = input->buf;
    return ret;
}
/*

In json_s2s we get a json with .s being a JSON string, and a span pointer to a buffer area, which we will extend, and a max u8* giving the end of the buffer region.

First we assert that the string starts with the double quote, which we advance past.

Then we iterate over the input until it ends or we reach the closing quote.

We unescape the input into the buffer, advancing the end of the buffer.

We handle all the escaping in JSON strings.

Specifically, we can see a backslash followed by:

- b,f,n,r,t
- ",\,/
- u followed by four hex digits with either A-F or a-f

In every case we write the unescaped character into the buffer, advancing .end, and we also copy all non-escaped characters over directly.
In the case of \u we encode as UTF-8.

Finally we return a span covering the area that .end advanced over.
I.e. the length of the returned span is also the length that was added to the buffer span.

If we would ever advance the buf past the max we also crash the program (prt, flush, exit).
*/

// Utility to convert a hex digit to its integer value
int hex_to_int(char c) {
    if ('0' <= c && c <= '9') return c - '0';
    if ('a' <= c && c <= 'f') return 10 + c - 'a';
    if ('A' <= c && c <= 'F') return 10 + c - 'A';
    return -1; // Error case, should never happen if input is correct
}

// Function to parse unicode sequence and write as UTF-8
void write_utf8_from_hex(u8 **buf, char *hex) {
    int codepoint = (hex_to_int(hex[0]) << 12) | (hex_to_int(hex[1]) << 8) |
                    (hex_to_int(hex[2]) << 4) | hex_to_int(hex[3]);
    if (codepoint < 0x80) {
        *(*buf)++ = codepoint;
    } else if (codepoint < 0x800) {
        *(*buf)++ = 192 + (codepoint >> 6);
        *(*buf)++ = 128 + (codepoint & 63);
    } else if (codepoint < 0x10000) {
        *(*buf)++ = 224 + (codepoint >> 12);
        *(*buf)++ = 128 + ((codepoint >> 6) & 63);
        *(*buf)++ = 128 + (codepoint & 63);
    } else {
        *(*buf)++ = 240 + (codepoint >> 18);
        *(*buf)++ = 128 + ((codepoint >> 12) & 63);
        *(*buf)++ = 128 + ((codepoint >> 6) & 63);
        *(*buf)++ = 128 + (codepoint & 63);
    }
}

span json_s2s(json j, span *buffer, u8 *max) {
    u8 *buf = buffer->end;
    span ret = { buf, buf };

    if (*j.s.buf != '\"') {
        prt("Expected starting quote in JSON string\n");
        flush();
        exit(1);
    }

    for (u8 *s = j.s.buf + 1; s < j.s.end && *s != '\"'; s++) {
        if (buf >= max) {
            prt("Buffer overflow detected\n");
            flush();
            exit(1);
        }
        if (*s == '\\') {
            s++;
            switch (*s) {
                case 'b': *buf++ = '\b'; break;
                case 'f': *buf++ = '\f'; break;
                case 'n': *buf++ = '\n'; break;
                case 'r': *buf++ = '\r'; break;
                case 't': *buf++ = '\t'; break;
                case '\"': case '\\': case '/': *buf++ = *s; break;
                case 'u':
                    if (s + 4 >= j.s.end) {
                        prt("Incomplete unicode escape in JSON string\n");
                        flush();
                        exit(1);
                    }
                    write_utf8_from_hex(&buf, (char *)(s + 1));
                    s += 4;
                    break;
                default:
                    prt("Unknown escape sequence in JSON string\n");
                    flush();
                    exit(1);
            }
        } else {
            *buf++ = *s;
        }
    }
    ret.end = buf;
    buffer->end = buf;
    return ret;
}

/*
In json_parse_prefix_string we consume a JSON string from the input span and wrap it as a json.

We handle all the escaping in JSON strings.

Specifically, we can see a backslash followed by:

- b,f,n,r,t
- ",\,/
- u followed by four hex digits with either A-F or a-f

Here we just consume the initial and final double quotes, parse all the escaping to be sure it is a valid JSON string, and return a json with .s that points to the string (including the quotes) or nulljson() if there is any parsing error.
If we return successfully ret.s.end and input->buf will be equal at the end.
*/

json json_parse_prefix_string(span *input) {
    if (empty(*input) || *input->buf != '\"') return nulljson();
    advance1(input);
    span start = *input;
    while (!empty(*input) && *input->buf != '\"') {
        if (*input->buf == '\\') {
            advance1(input);
            if (empty(*input)) return nulljson();
            if (*input->buf == 'u') {
                for (int i = 0; i < 4; i++) {
                    advance1(input);
                    if (empty(*input) || !isxdigit(*input->buf)) return nulljson();
                }
            } else if (strchr("bfnrt\"\\/", *input->buf) == NULL) {
                return nulljson();
            }
        }
        advance1(input);
    }
    if (empty(*input)) return nulljson();
    advance1(input);
    return make_json((span){start.buf - 1, input->buf});
}

/*
In json_parse_prefix_number, we handle the JSON number format, namely:

We either return nulljson() if we could not parse the number for any reason or a json which includes the number that was parsed.

Manually written.
*/

json json_parse_prefix_number(span *input) {
  json ret = {0};
  ret.s.buf = input->buf;

  if (*input->buf == '-') advance1(input);

  if (!isdigit(*input->buf)) return nulljson();
  while (isdigit(*input->buf)) input->buf++;

  if (*input->buf == '.') {
    advance1(input);
    if (!isdigit(*input->buf)) return nulljson();
    while(isdigit(*input->buf)) input->buf++;
  }
  if (*input->buf == 'e' || *input->buf == 'E') {
    advance1(input);
    if (*input->buf == '+' || *input->buf == '-') {
      advance1(input);
    }
    if (!isdigit(*input->buf)) return nulljson();
    while (isdigit(*input->buf)) {
      advance1(input);
    }
  }

  ret.s.end = input->buf;
  return ret;
}
/*
Manually written for now.
*/

json json_parse_prefix_littok(span *input) {
  span inner;
  if (!empty(inner = consume_prefix(S("true"), input))) return (json){inner};
  if (!empty(inner = consume_prefix(S("false"), input))) return (json){inner};
  if (!empty(inner = consume_prefix(S("null"), input))) return (json){inner};
  return nulljson();
}

/* #sio

Spanio basics.

Instead of null-terminated strings, we use spans.
Functions that take string input or output should always use the span type, never char*.
(We only rarely use char* for talking to C library functions.)

To create a span from a char* use S(): `span s = S("hello world")`.

A span is a struct having a .buf and .end, both u8 pointers.

To get the length of a span, use `len(s)`, or to just check if it is empty use `empty(s)`.

To compare equality of strings, use span_eq(a,b).

for a lexicographic comparison use span_cmp(a,b), returning an int {-,0,+} according to whether {a<b, a==b, a>b}.

You can tell if a span `a` starts or ends with another `b` using starts_with(a,b) and ends_with(a,b).

To get a prefix of a span use first_n(span,int).
*/
/* #parserpattern

In parser functions, we are given a span pointer, usually called input.

We either return a value indicating success of some parsed result, and shorten the span (by advancing .buf), or we leave the span untouched and return some distinguished value indicating failure.

Thus the basic parser pattern is a function that takes a span pointer as input, and returns something.
The success or failure is indicated by the returned value.
The progress of the parse of the input is indicated by the modification, if any, made to the span that is passed in by reference.
The input span is gradually consumed by the various parser functions until it is empty, or until the parse fails.

We often use consume_prefix(span,span*) in parsers which consumes a literal prefix, if present, and returns it as span, and shortens the second argument accordingly, or if it is not present, returns an empty span and leaves the second argument unmodified.

We may also use take_n(int,span*), which is similar to first_n in the main spanio functions, but specialized for modifying a span as we do when parsing.
It takes a bite off the front of the string being parsed.
It returns a span of the given length pointing to the beginning of the input span, and advances the .buf of that span by the same amount.
*/

/* #jsonparser

The json parser (and indexing methods) returns a parsed json object that wraps a span containing the actual bytes of input.

This means that if the span parses cleanly as json, the .s of the returned value will be a subset of the input span (in the sense of contains_ptr), i.e. it will point to the same bytes.
This means that we don't allocate any new memory when parsing, and it also means that (if desired) the consuming function can index the JSON as it is being parsed.

This means that we only wrap spans of the given input, and manually construct json objects to return, rather than calling other constructor functions (e.g. json_b), which will typically point to static strings.
*/
/* #json_parse_prefix_littok @sio @jsonlib @parserpattern @jsonparser

Here we parse the three literal tokens, which are "true" "false" and "null".

Write the json json_parse_prefix_littok(span*) function.


json json_parse_prefix_littok(span* input) {
    if (consume_prefix(input, S("true"))) return json_b(1);
    if (consume_prefix(input, S("false"))) return json_b(0);
    if (consume_prefix(input, S("null"))) return json_0();
    return json_is_null(json_0()); // or some other error indication
}

*/
/* 
We do not use null-terminated strings but instead rely on the explicit end point of our span type.
Here we have spanspan(span,span) which is equivalent to strstr or memmem in the C library but for spans rather than C strings or void pointers respectively.
We implement spanspan with memmem under the hood so we get the same performance.
Like strstr or memmem, the arguments are in haystack, needle order, so remember to call spanspan with the thing you are looking for as the second arg.
We return either the empty span at the end of haystack or the span pointing to the first location of needle in haystack.
If needle is empty we return the empty span at the beginning of haystack.
(This allows you to distinguish between a non-found needle and an empty needle, as both give an empty match.)
Maybe we should actually return the nullspan here; considering the obvious extension to regexes or more powerful patterns, the difference between a matching empty span and a non-match would become significant.
Examples:

spanspan "abc" "b" -> "b"
spanspan "abc" "x" -> "" (located after "c")
spanspan "abc" ""  -> "" (located at "a")
*/

span spanspan(span haystack, span needle) {
  if (empty(needle)) return (span){haystack.buf, haystack.buf};

  if (len(needle) > len(haystack)) return nullspan();

  void *result = memmem(haystack.buf, len(haystack), needle.buf, len(needle));

  if (!result) return (span){haystack.end, haystack.end};

  return (span){result, result + len(needle)};
}

// Checks if a given span is contained in a spans.
// Returns 1 if found, 0 otherwise.
// Actually a more useful function would return an index or -1, so we don't need another function when we care where the thing is.
int is_one_of(span x, spans ys) {
  for (int i = 0; i < ys.n; ++i) {
    if (span_eq(x, ys.a[i])) {
      return 1; // Found
    }
  }
  return 0; // Not found
}

int index_of(span x, spans ys) {
  for (int i=0; i<ys.n; i++) {
    if (span_eq(ys.a[i], x)) return i;
  }
  return -1;
}

/*
Library function inp_compl() returns a span that is the complement of inp in the input_space.
The input space is defined by the pointer input_space and the constant BUF_SZ.
As the span inp always represents the content of the input (which has been written so far, for example by reading from stdin), the complement of inp represents the portion of the input space after inp.end which has not yet been written to.

We have cmp_compl() and out_compl() methods which do the analogous operation for the respective spaces.
*/

span inp_compl() {
  span compl;
  compl.buf = inp.end;
  compl.end = input_space + BUF_SZ;
  return compl;
}

span cmp_compl() {
  span compl;
  compl.buf = cmp.end;
  compl.end = cmp_space + BUF_SZ;
  return compl;
}

span out_compl() {
  span compl;
  compl.buf = out.end;
  compl.end = output_space + BUF_SZ;
  return compl;
}

