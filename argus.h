#ifndef ARGUS_H
#define ARGUS_H

/*
    Arguś: A simple, single-header argument parser for C
    Author: Kamack38
    Original Author: Xander Gouws

    Provided under an MIT License. See end of file for details.

    See github.com/gouwsxander/easy-args for documentation and examples.
    See also the original repo github.com/gouwsxander/easy-args
*/

#include <errno.h>    // used for error handling in default parsers
#include <stdbool.h>  // used for bool impl
#include <stdio.h>    // used for IO
#include <stdlib.h>   // used for parsing (atoi, atof)
#include <string.h>   // used for strcmp

/**
 * @def REQUIRED_ARG(type, name, label, description, parser)
 * @brief Define required positional argument
 * @param type The type of the argument
 * @param name The name of the argument in the args_t struct
 * @param label The display name in help
 * @param description A description of the argument
 * @param parser A function for parsing the argument of type (int)(char* argv[i], type args->name,
 * char* next_char). This function should contain all parsing and error printing logic and return a
 * non-zero value accordingly. It should also print any errors that may occur in format "Error: %s".
 * next_char* will always be set to NULL in required arguments.
 */
#define REQUIRED_STRING_ARG(name, label, description) REQUIRED_ARG(char*, name, label, description, parse_str)
#define REQUIRED_CHAR_ARG(name, label, description) REQUIRED_ARG(char, name, label, description, parse_char)
#define REQUIRED_INT_ARG(name, label, description) REQUIRED_ARG(int, name, label, description, parse_int)
#define REQUIRED_UINT_ARG(name, label, description) REQUIRED_ARG(unsigned int, name, label, description, parse_uint)
#define REQUIRED_LONG_ARG(name, label, description) REQUIRED_ARG(long, name, label, description, parse_l)
#define REQUIRED_ULONG_ARG(name, label, description) REQUIRED_ARG(unsigned long, name, label, description, parse_ul)
#define REQUIRED_LONG_LONG_ARG(name, label, description) REQUIRED_ARG(long long, name, label, description, parse_ll)
#define REQUIRED_ULONG_LONG_ARG(name, label, description) \
    REQUIRED_ARG(unsigned long long, name, label, description, parse_ull)
#define REQUIRED_SIZE_ARG(name, label, description) REQUIRED_ARG(size_t, name, label, description, parse_ull)
#define REQUIRED_FLOAT_ARG(name, label, description) REQUIRED_ARG(float, name, label, description, parse_f)
#define REQUIRED_DOUBLE_ARG(name, label, description) REQUIRED_ARG(double, name, label, description, parse_d)
#define REQUIRED_LONG_DOUBLE_ARG(name, label, description) REQUIRED_ARG(long double, name, label, description, parse_ld)

/* clang-format off */
/**
 * @def OPTIONAL_ARG(type, name, shortopt, longopt, arg_label, default, description, formatter, parser)
 * @brief Define an optional argument
 * @param type The type of the argument
 * @param name The name of the argument in the args_t struct
 * @param shortopt A single character which will be used for a short flag (NOT a char literal)
 * @param longopt A string that will act as a long option (NOT a string literal)
 * @param arg_label The label of the argument in the help string (a string literal)
 * @param default The default value of the argument
 * @param description A description of the argument
 * @param formatter A formatter for printing the argument
 * @param parser A function for parsing the argument of type (int)(const char* const argv[i], type args->name,
 * const char* next_char). This function should contain all parsing and error printing logic and return a
 * non-zero value accordingly. It should also print any errors that may occur in format "Error: %s".
 * Setting next_char to NULL or '\0' means that the parser is finished reading and can return
 */
/* clang-format on */
#define OPTIONAL_STRING_ARG(name, shortopt, longopt, arg_label, default, description) \
    OPTIONAL_ARG(char*, name, shortopt, longopt, arg_label, default, description, "%s", parse_str)
#define OPTIONAL_CHAR_ARG(name, shortopt, longopt, arg_label, default, description) \
    OPTIONAL_ARG(char, name, shortopt, longopt, arg_label, default, description, "%c", parse_char)
#define OPTIONAL_INT_ARG(name, shortopt, longopt, arg_label, default, description) \
    OPTIONAL_ARG(int, name, shortopt, longopt, arg_label, default, description, "%d", parse_int)
#define OPTIONAL_UINT_ARG(name, shortopt, longopt, arg_label, default, description) \
    OPTIONAL_ARG(unsigned int, name, shortopt, longopt, arg_label, default, description, "%u", parse_uint)
#define OPTIONAL_LONG_ARG(name, shortopt, longopt, arg_label, default, description) \
    OPTIONAL_ARG(long, name, shortopt, longopt, arg_label, default, description, "%ld", parse_l)
#define OPTIONAL_ULONG_ARG(name, shortopt, longopt, arg_label, default, description) \
    OPTIONAL_ARG(unsigned long, name, shortopt, longopt, arg_label, default, description, "%lu", parse_ul)
#define OPTIONAL_LONG_LONG_ARG(name, shortopt, longopt, arg_label, default, description) \
    OPTIONAL_ARG(long long, name, shortopt, longopt, arg_label, default, description, "%lld", parse_ll)
#define OPTIONAL_ULONG_LONG_ARG(name, shortopt, longopt, arg_label, default, description) \
    OPTIONAL_ARG(unsigned long long, name, shortopt, longopt, arg_label, default, description, "%llu", parse_ull)
#define OPTIONAL_SIZE_ARG(name, shortopt, longopt, arg_label, default, description) \
    OPTIONAL_ARG(size_t, name, shortopt, longopt, arg_label, default, description, "%zu", parse_ull)
#define OPTIONAL_FLOAT_ARG(name, shortopt, longopt, arg_label, default, description, precision) \
    OPTIONAL_ARG(float, name, shortopt, longopt, arg_label, default, description, "%." #precision "g", parse_f)
#define OPTIONAL_DOUBLE_ARG(name, shortopt, longopt, arg_label, default, description, precision) \
    OPTIONAL_ARG(double, name, shortopt, longopt, arg_label, default, description, "%." #precision "g", parse_d)
#define OPTIONAL_LONG_DOUBLE_ARG(name, shortopt, longopt, arg_label, default, description, precision) \
    OPTIONAL_ARG(long double, name, shortopt, longopt, arg_label, default, description, "%." #precision "g", parse_ld)

/**
 * @def BOOLEAN_ARG(name, shortopt, longopt, description)
 * @brief Define an boolean argument (true/false)
 * @param name The name of the argument in the args_t struct and for the long argument.
 * @param shortopt A single character which will be used for a short flag (NOT a char literal)
 * @param longopt A string that will act as a long option (NOT a string literal)
 * @param description A description of the argument
 */

// PARSERS
static inline int parse_str(const char* const text, char** out, const char** advance) {
    *out = (char*)text;
    if (advance != NULL) *advance = NULL;
    return 0;
}

static inline int parse_char(const char* const text, char* out, const char** advance) {
    *out = text[0];
    if (advance != NULL) *advance = (char*)(text + 1);
    return 0;
}

#define NUMBER_PARSER(type, shorthand, func)                                                       \
    static inline int parse_##shorthand(const char* const text, type* out, const char** advance) { \
        errno = 0;                                                                                 \
        *out = (type)func(text, (char**)advance, 10);                                              \
        if (errno != 0) {                                                                          \
            fprintf(stderr, "Error: failed to parse '%s' as " #type, text);                        \
            return 1;                                                                              \
        }                                                                                          \
        return 0;                                                                                  \
    }

#define FLOAT_PARSER(type, shorthand, func)                                                        \
    static inline int parse_##shorthand(const char* const text, type* out, const char** advance) { \
        errno = 0;                                                                                 \
        *out = (type)func(text, (char**)advance);                                                  \
        if (errno != 0) {                                                                          \
            fprintf(stderr, "Error: failed to parse '%s' as " #type, text);                        \
            return 1;                                                                              \
        }                                                                                          \
        return 0;                                                                                  \
    }

NUMBER_PARSER(unsigned long long, ull, strtoull)
NUMBER_PARSER(unsigned long, ul, strtoul)
NUMBER_PARSER(long long, ll, strtoll)
NUMBER_PARSER(long, l, strtol)
NUMBER_PARSER(unsigned int, uint, strtoul)
NUMBER_PARSER(int, int, strtol)
FLOAT_PARSER(float, f, strtof)
FLOAT_PARSER(double, d, strtod)
FLOAT_PARSER(long double, ld, strtold)

// COUNT ARGUMENTS
#ifdef REQUIRED_ARGS
#define REQUIRED_ARG(...) +1
static const int REQUIRED_ARG_COUNT = 0 REQUIRED_ARGS;
#undef REQUIRED_ARG
#else
static const int REQUIRED_ARG_COUNT = 0;
#endif

#ifdef OPTIONAL_ARGS
#define OPTIONAL_ARG(...) +1
static const int OPTIONAL_ARG_COUNT = 0 OPTIONAL_ARGS;
#undef OPTIONAL_ARG
#else
static const int OPTIONAL_ARG_COUNT = 0;
#endif

#ifdef BOOLEAN_ARGS
#define BOOLEAN_ARG(...) +1
static const int BOOLEAN_ARG_COUNT = 0 BOOLEAN_ARGS;
#undef BOOLEAN_ARG
#else
static const int BOOLEAN_ARG_COUNT = 0;
#endif

// ARG_T STRUCT
#define REQUIRED_ARG(type, name, ...) type name;
#define OPTIONAL_ARG(type, name, ...) type name;
#define BOOLEAN_ARG(name, ...) bool name;
// Stores argument values
typedef struct {
#ifdef REQUIRED_ARGS
    REQUIRED_ARGS
#endif
#ifdef OPTIONAL_ARGS
    OPTIONAL_ARGS
#endif
#ifdef BOOLEAN_ARGS
    BOOLEAN_ARGS
#endif
} args_t;
#undef REQUIRED_ARG
#undef OPTIONAL_ARG
#undef BOOLEAN_ARG

// Build an args_t struct with assigned default values
static inline args_t make_default_args() {
    args_t args = {
#define REQUIRED_ARG(type, name, ...) .name = (type)0,
#define OPTIONAL_ARG(type, name, shortopt, longopt, arg_label, default, ...) .name = default,
#define BOOLEAN_ARG(name, ...) .name = 0,

#ifdef REQUIRED_ARGS
        REQUIRED_ARGS
#endif

#ifdef OPTIONAL_ARGS
            OPTIONAL_ARGS
#endif

#ifdef BOOLEAN_ARGS
                BOOLEAN_ARGS
#endif

#undef REQUIRED_ARG
#undef OPTIONAL_ARG
#undef BOOLEAN_ARG
    };

    return args;
}

// Conditional flag generation
#define PROBE_NONE ,

// This MUST be two steps to force the preprocessor to resolve the comma
#define SELECT_3RD(a1, a2, a3, ...) a3
#define EVAL_SELECT_3RD(args) SELECT_3RD args

// Logic Selector
#define DO_NOTHING(...)

// Insert macro if value is not NONE
#define NOT_NONE(val, MACRO) EVAL_SELECT_3RD((PROBE_##val, DO_NOTHING, MACRO))

/**
 * @brief Parse arguments
 *
 * @param[in]  argc  Number of command-line arguments (standard main() argc).
 * @param[in]  argv  Array of argument strings (standard main() argv).
 * @param[in]  args  Pointer to an default args_t struct.
 *
 * @retval 1 Error
 * @retval 0 OK
 */
static inline int parse_args(const int argc, const char* const argv[], args_t* args) {
    if (!argc || !argv) {
        fprintf(stderr, "Internal error: null args or argv.\n");
        return 1;
    }

    // If not enough required arguments
    if (argc < 1 + REQUIRED_ARG_COUNT) {
        fprintf(stderr, "Not all required arguments included.\n");
        return 1;
    }

// Get required arguments
#ifdef REQUIRED_ARGS
#define REQUIRED_ARG(type, name, label, description, parser) \
    do {                                                     \
        int error = parser(argv[i++], &args->name, NULL);    \
        if (error != 0) return 1;                            \
    } while (0);
    int i = 1;
    REQUIRED_ARGS
#undef REQUIRED_ARG
#endif

    // Get optional and boolean arguments
    for (int i = 1 + REQUIRED_ARG_COUNT; i < argc; i++) {
#define GENERATE_LONG_OPT(name, longopt, parser)                                                               \
    if (!strcmp(argv[i], "--" #longopt)) {                                                                     \
        if (i + 1 >= argc) {                                                                                   \
            fprintf(stderr, "Error: option '%s' requires a value.\n", "--" #longopt);                          \
            return 1;                                                                                          \
        }                                                                                                      \
        const char* next_char = NULL;                                                                          \
        int error = parser(argv[++i], &args->name, &next_char);                                                \
        if (error != 0) {                                                                                      \
            return 1; /* The parser should notify the user of an error */                                      \
        }                                                                                                      \
        /* We don't allow parsing only part of an option */                                                    \
        if (next_char != NULL && *next_char != '\0') {                                                         \
            fprintf(stderr, "Error: couldn't parse argument '%s' for option '%s'.\n", argv[i], "--" #longopt); \
            return 1;                                                                                          \
        }                                                                                                      \
        continue;                                                                                              \
    }

// This generates the long opt parsing for an optional argument if it's not NONE
#define OPTIONAL_ARG(type, name, shortopt, longopt, arg_label, default, description, formatter, parser) \
    NOT_NONE(longopt, GENERATE_LONG_OPT)(name, longopt, parser)

#define GENERATE_LONG_BOOL(name, longopt)  \
    if (!strcmp(argv[i], "--" #longopt)) { \
        args->name = true;                 \
        continue;                          \
    }

#define BOOLEAN_ARG(name, shortopt, longopt, description) NOT_NONE(longopt, GENERATE_LONG_BOOL)(name, longopt)

#ifdef OPTIONAL_ARGS
        OPTIONAL_ARGS
#endif

#ifdef BOOLEAN_ARGS
        BOOLEAN_ARGS
#endif

#undef OPTIONAL_ARG
#undef BOOLEAN_ARG

        // Parse flags
        if (argv[i][0] == '-') {
            const char* curr_flag = argv[i] + 1;
#define GENERATE_SHORT_OPT(name, shortopt, parser)                                    \
    if (*curr_flag == #shortopt[0]) {                                                 \
        if (curr_flag[1] == '\0') {                                                   \
            fprintf(stderr, "Error: option '%s' requires a value.\n", "-" #shortopt); \
            return 1;                                                                 \
        }                                                                             \
        int error = parser(curr_flag + 1, &args->name, &curr_flag);                   \
        if (error != 0) {                                                             \
            return 1; /* The parser should notify the user of an error */             \
        }                                                                             \
        /* We set the flag to NULL when we read all of the remaning string  */        \
        /* We treat any left string as the next flag */                               \
        continue;                                                                     \
    }

#define OPTIONAL_ARG(type, name, shortopt, longopt, arg_label, default, description, formatter, parser) \
    NOT_NONE(shortopt, GENERATE_SHORT_OPT)(name, shortopt, parser)

#define GENERATE_SHORT_BOOL(name, shortopt) \
    if (*curr_flag == #shortopt[0]) {       \
        args->name = true;                  \
        curr_flag++;                        \
        continue;                           \
    }

#define BOOLEAN_ARG(name, shortopt, longopt, description) NOT_NONE(shortopt, GENERATE_SHORT_BOOL)(name, shortopt)

            while (curr_flag != NULL && *curr_flag != '\0') {
#ifdef OPTIONAL_ARGS
                OPTIONAL_ARGS
#endif

#ifdef BOOLEAN_ARGS
                BOOLEAN_ARGS
#endif

                fprintf(stderr, "Error: Invalid flag '-%s'\n", curr_flag);
                return 1;
            }
#undef OPTIONAL_ARG
#undef BOOLEAN_ARG

            continue;
        }

        fprintf(stderr, "Error: Invalid argument '%s'\n", argv[i]);
        return 1;
    }

    return 0;
}

// Display help string, given command used to launch program, e.g., argv[0]
static inline void print_help(const char* exec_alias) {
    // USAGE SECTION
    printf("USAGE:\n");
    printf("    %s ", exec_alias);

#define REQUIRED_ARG(type, name, label, ...) "<" label "> "

#ifdef REQUIRED_ARGS
    if (REQUIRED_ARG_COUNT > 0 && REQUIRED_ARG_COUNT <= 3) {
        printf(REQUIRED_ARGS);
    } else {
        printf("<ARGUMENTS> ");
    }
#endif
#undef REQUIRED_ARG

#define SHORT_OPT_QUICK_HELP(shortopt, arg_label) \
    "[-" #shortopt "<" arg_label                  \
    ">"                                           \
    "] "
#define OPTIONAL_ARG(type, name, shortopt, longopt, arg_label, ...) \
    NOT_NONE(shortopt, SHORT_OPT_QUICK_HELP)(shortopt, arg_label)

#define BOOLEAN_ARG_QUICK_HELP(shortopt) "[-" #shortopt "] "
#define BOOLEAN_ARG(name, shortopt, longopt, ...) NOT_NONE(shortopt, BOOLEAN_ARG_QUICK_HELP)(shortopt)

#if (OPTIONAL_ARG_COUNT + BOOLEAN_ARG_COUNT <= 3)
#ifdef OPTIONAL_ARGS
    printf(OPTIONAL_ARGS);
#endif
#undef OPTIONAL_ARG

#ifdef BOOLEAN_ARGS
    printf(BOOLEAN_ARGS);
#endif
#undef BOOLEAN_ARG

#else
    printf("[OPTIONS]");
#endif

    printf("\n\n");

    // Get maximum width of labels for spacing
    int max_width = 0;
    (void)max_width;  // suppress unused variable warning

#define REQUIRED_ARG(type, name, label, ...)  \
    {                                         \
        int len = strlen(label) + 2;          \
        if (len > max_width) max_width = len; \
    }
#define OPTIONAL_ARG(type, name, shortopt, longopt, arg_label, ...) \
    {                                                               \
        int len = 6 + strlen(#longopt) + strlen(arg_label) + 2;     \
        if (len > max_width) max_width = len;                       \
    }
#define BOOLEAN_ARG(name, shortopt, longopt, ...) \
    {                                             \
        int len = 4 + strlen(#longopt);           \
        if (len > max_width) max_width = len;     \
    }

#ifdef REQUIRED_ARGS
    REQUIRED_ARGS
#endif
#undef REQUIRED_ARG
#ifdef OPTIONAL_ARGS
    OPTIONAL_ARGS
#endif
#undef OPTIONAL_ARG
#ifdef BOOLEAN_ARGS
    BOOLEAN_ARGS
#endif
#undef BOOLEAN_ARG

// ARGUMENTS SECTION
#define REQUIRED_ARG(type, name, label, description, ...) \
    printf("    <" label ">%*s  " description "\n", max_width - (int)strlen(label) - 1, "");

#ifdef REQUIRED_ARGS
    printf("ARGUMENTS:\n");
    REQUIRED_ARGS
    printf("\n");
#endif
#undef REQUIRED_ARG

#if defined(OPTIONAL_ARGS) || defined(BOOLEAN_ARGS)
    printf("OPTIONS:\n");
#endif

#define SHORT_HELP(shortopt) "-" #shortopt
#define LONG_HELP(longopt) "--" #longopt
#define WIDTH(keyword) (int)strlen(#keyword)

#define SECOND_SET(second, MACRO) NOT_NONE(second, MACRO)
#define BOTH_SET(first, second, MACRO) NOT_NONE(first, SECOND_SET)(second, MACRO)

    // Calculate the width of a boolean argument
#define CALC_WIDTH(shortopt, longopt)                                                         \
    max_width NOT_NONE(shortopt, -1 - WIDTH)(shortopt) NOT_NONE(longopt, -2 - WIDTH)(longopt) \
            BOTH_SET(shortopt, longopt, -2) +                                                 \
        1

    // Calculate the width of the optional argument
#define CALC_OPT_WIDTH(shortopt, longopt, arg_label) CALC_WIDTH(shortopt, longopt) - (int)strlen(arg_label) - 3

#define OPTIONAL_ARG(type, name, shortopt, longopt, arg_label, default, description, formatter, ...) \
    printf("    "                                   /* line break */                                 \
           NOT_NONE(shortopt, SHORT_HELP)(shortopt) /* line break */                                 \
           BOTH_SET(shortopt, longopt, ", ")        /* line break */                                 \
           NOT_NONE(longopt, LONG_HELP)(longopt)    /* line break */                                 \
           " <" arg_label                                                                            \
           ">"                                                                                       \
           "%*s  " description " (default: " formatter ")\n",                                        \
           CALC_OPT_WIDTH(shortopt, longopt, arg_label), "", default);

#define BOOLEAN_ARG(name, shortopt, longopt, description)            \
    printf("    "                                   /* line break */ \
           NOT_NONE(shortopt, SHORT_HELP)(shortopt) /* line break */ \
           BOTH_SET(shortopt, longopt, ", ")        /* line break */ \
           NOT_NONE(longopt, LONG_HELP)(longopt)    /* line break */ \
           "%*s  " description "\n",                                 \
           CALC_WIDTH(shortopt, longopt), "");

#ifdef OPTIONAL_ARGS
    OPTIONAL_ARGS
#endif
#undef OPTIONAL_ARG

#ifdef BOOLEAN_ARGS
    BOOLEAN_ARGS
#endif
#undef BOOLEAN_ARG
}

#endif

/*
    MIT License

    Copyright (c) 2025 Kamack38, Xander Gouws

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in all
    copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
    SOFTWARE.
*/
