/*
 * Copyright (c) 2018 Apple Inc. All rights reserved.
 *
 * @APPLE_LICENSE_HEADER_START@
 *
 * This file contains Original Code and/or Modifications of Original Code
 * as defined in and that are subject to the Apple Public Source License
 * Version 2.0 (the 'License'). You may not use this file except in
 * compliance with the License. Please obtain a copy of the License at
 * http://www.opensource.apple.com/apsl/ and read it before using this
 * file.
 *
 * The Original Code and all software distributed under the License are
 * distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER
 * EXPRESS OR IMPLIED, AND APPLE HEREBY DISCLAIMS ALL SUCH WARRANTIES,
 * INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR NON-INFRINGEMENT.
 * Please see the License for the specific language governing rights and
 * limitations under the License.
 *
 * @APPLE_LICENSE_HEADER_END@
 */

/*!
 * @header
 * Interfaces to implement subcommand-style command line utilities (e.g.
 * launchctl(1)) and automatically generate usage output. The usage generated by
 * these interfaces assumes a long option convention (cf. getopt_long(3)) and is
 * loosely based on the docopt convention described at
 *
 *     http://docopt.org
 *
 * The user may define each subcommand taken by the utility as:
 *
 *     static const struct option _template_opts[] = {
 *         [0] = {
 *             .name = "bar",
 *             .has_arg = required_argument,
 *             .flag = NULL,
 *             .val = 'f',
 *         }, {
 *             .name = "baz",
 *             .has_arg = optional_argument,
 *             .flag = NULL,
 *             .val = 'b',
 *         }, {
 *             .name = NULL,
 *             .has_arg = 0,
 *             .flag = NULL,
 *             .val = 0,
 *         },
 *     };
 *
 *     static const os_subcommand_option_t _template_required[] = {
 *         [0] = {
 *             .osco_template = OS_SUBCOMMAND_OPTION_VERSION,
 *             .osco_flags = 0,
 *             .osco_option = &_template_opts[0],
 *             .osco_argument_usage = "thing-to-bar",
 *             .osco_argument_human = "The thing to bar. May be specified as a "
 *             "bar that has a baz. This baz should have a shnaz.",
 *         },
 *         OS_SUBCOMMAND_OPTION_TERMINATOR,
 *     };
 *
 *     static const os_subcommand_option_t _template_optional[] = {
 *         [0] = {
 *             .osco_template = OS_SUBCOMMAND_OPTION_VERSION,
 *             .osco_flags = 0,
 *             .osco_option = &_template_opts[1],
 *             .osco_argument_usage = "thing-to-baz",
 *             .osco_argument_human = "The baz of which to propagate a foo.",
 *         },
 *         OS_SUBCOMMAND_OPTION_TERMINATOR,
 *     };
 *
 *     static const os_subcommand_option_t _template_positional[] = {
 *         [0] = {
 *             .osco_template = OS_SUBCOMMAND_OPTION_VERSION,
 *             .osco_flags = 0,
 *             .osco_option = NULL,
 *             .osco_argument_usage = "positional-baz",
 *             .osco_argument_human = "A baz specified by position.",
 *         },
 *         OS_SUBCOMMAND_OPTION_TERMINATOR,
 *     };
 *
 *     static const os_subcommand_t _template_cmd = {
 *         .osc_template = OS_SUBCOMMAND_VERSION,
 *         .osc_flags = 0,
 *         .osc_name = "foo",
 *         .osc_desc = "foo a bar or maybe baz",
 *         .osc_optstring = "f:b:",
 *         .osc_options = _foo_opts,
 *         .osc_required = _foo_required,
 *         .osc_optional = _foo_optional,
 *         .osc_positional = _template_positional,
 *         .osc_invoke = &_foo_invoke,
 *     }
 *     OS_SUBCOMMAND_REGISTER(_foo_cmd);
 * };
 *
 * When {@link os_subcommand_main} is called, the tool's "help" subcommand will
 * display approximately the following:
 *
 * $ tool help
 * usage: tool <subcommand>
 *
 * subcommands:
 *     foo             foo a bar or maybe baz
 *     help            Prints helpful information
 *
 * $ tool help foo
 * usage: tool foo [options] --bar=<thing-to-bar> <positional-baz>
 *
 * required options:
 *     --bar=<thing-to-bar>    The thing to bar. May be specified as a bar that
 *                             has a baz. This baz should have a shnaz.
 *
 *     positional-baz          A baz specified by position.
 *
 * optional options:
 *     --baz[=thing-to-baz]    The baz of which to propagate a foo.
 */
#ifndef __DARWIN_CTL_H
#define __DARWIN_CTL_H

#include <os/base.h>
#include <os/api.h>
#include <os/linker_set.h>
#include <sys/cdefs.h>
#include <stdio.h>
#include <getopt.h>

#if DARWIN_TAPI
#include "tapi.h"
#endif

__BEGIN_DECLS;

/*!
 * @define OS_SUBCOMMAND_REGISTER
 * Registers a {@link os_subcommand_t} with the runtime. Subcommands may only be
 * declared as part of the main executable image -- subcommands declared in
 * dynamic libraries or bundles will not be recognized.
 */
#define OS_SUBCOMMAND_REGISTER(_subcommand) \
		LINKER_SET_ENTRY(__subcommands, _subcommand)

/*!
 * @typedef os_subcommand_t
 * The formal type name for the _os_subcommand structure.
 */
DARWIN_API_AVAILABLE_20181020
typedef struct _os_subcommand os_subcommand_t;

/*!
 * @const OS_SUBCOMMAND_OPTION_VERSION
 * The maximum version of the {@link os_subcommand_option_t} structure supported
 * by the implementation.
 */
#define OS_SUBCOMMAND_OPTION_VERSION ((os_struct_version_t)0)

/*!
 * @typedef os_subcommand_option_flags_t
 * Flags describing an option for a subcommand.
 *
 * @const OS_SUBCOMMAND_OPTION_FLAG_INIT
 * No flags set. This value is suitable for initialization purposes.
 *
 * @const OS_SUBCOMMAND_OPTION_FLAG_TERMINATOR
 * The option terminates an array of {@link os_subcommand_option_t} structures
 * and does not contain any useful information.
 *
 * @const OS_SUBCOMMAND_OPTION_FLAG_OPTIONAL_POS
 * The option is a positional option (that is, not identified by a long or short
 * flag) and is not required for the subcommand to execute successfully.
 *
 * @const OS_SUBCOMMAND_OPTION_FLAG_ENUM
 * The option has an explicitly-defined list of valid inputs that are enumerated
 * in the option's {@link osco_argument_usage} field. When printing usage
 * information for this option, the implementation will not transform the string
 * in this field in any way.
 *
 * For example, an option named "--stream" might have three valid inputs:
 * "stdin", "stdout", and "stderr", and the usage string may be specified as
 *
 *     "stdin|stdout|stderr"
 *
 * Without this flag, the implementation would print this string as a parameter
 * name, i.e. in all caps:
 *
 *     "<STDIN|STDOUT|STDERR>"
 *
 * With this flag, the string will be printed as it is given.
 */
DARWIN_API_AVAILABLE_20181020
OS_CLOSED_ENUM(os_subcommand_option_flags, uint64_t,
	OS_SUBCOMMAND_OPTION_FLAG_INIT = 0,
	OS_SUBCOMMAND_OPTION_FLAG_TERMINATOR = (1 << 0),
	OS_SUBCOMMAND_OPTION_FLAG_OPTIONAL_POS = (1 << 1),
	OS_SUBCOMMAND_OPTION_FLAG_ENUM = (1 << 2),
);

/*!
 * @typedef os_subcommand_option_t
 * A structure describing human-readable information about a particular option
 * taken by a subcommand. This structure is to be returned when the
 * implementation invokes a {@link os_subcommand_option_info_t} function to
 * query about a command's options individually. This is done when the
 * implementation is synthesizing a usage string.
 *
 * @field osco_version
 * The version of the structure. Initialize to
 * {@link OS_SUBCOMMAND_OPTION_VERSION}.
 *
 * @field osco_flags
 * A set of flags describing information about the option.
 *
 * @field osco_option
 * A pointer to the option structure ingested by getopt_long(3) which
 * corresponds to this option.
 *
 * @field osco_argument_usage
 * The short-form name of the argument given to the option, appropriate for
 * display in a usage specifier. For example, if the subcommand takes a "--file"
 * option with a required argument, this might be the string "FILE-PATH", and
 * the resulting usage specifier would be
 *
 *     --file=<FILE-PATH>
 *
 * @field osco_argument_human
 * The long-form description of the argument given to the option. Extending the
 * above example, this might be the string "The path to a file to take as input.
 * This path must be absolute; relative paths are not supported." and the
 * resulting usage specifier would be
 *
 *     --file=<FILE-PATH> The path to a file to take as input. This path must be
 *                        absolute; relative paths are not supported.
 */
DARWIN_API_AVAILABLE_20191015
typedef struct _os_subcommand_option {
	const os_struct_version_t osco_version;
	os_subcommand_option_flags_t osco_flags;
	const struct option *osco_option;
	const char *osco_argument_usage;
	const char *osco_argument_human;
} os_subcommand_option_t;

/*!
 * @const OS_SUBCOMMAND_OPTION_TERMINATOR
 * A convenience terminator for an array of {@link os_subcommand_option_t}
 * structures.
 */
#define OS_SUBCOMMAND_OPTION_TERMINATOR (os_subcommand_option_t){ \
	.osco_version = OS_SUBCOMMAND_OPTION_VERSION, \
	.osco_flags = OS_SUBCOMMAND_OPTION_FLAG_TERMINATOR, \
	.osco_option = NULL, \
	.osco_argument_usage = NULL, \
	.osco_argument_human = NULL, \
}

/*!
* @const OS_SUBCOMMAND_GETOPT_TERMINATOR
* A convenience terminator for an array of getopt(3) option structures.
*/
#define OS_SUBCOMMAND_GETOPT_TERMINATOR (struct option){ \
	.name = NULL, \
	.has_arg = 0, \
	.flag = NULL, \
	.val = 0, \
}

/*!
 * @const OS_SUBCOMMAND_VERSION
 * The maximum version of the {@link os_subcommand_t} structure supported by the
 * implementation.
 */
#define OS_SUBCOMMAND_VERSION ((os_struct_version_t)0)

/*!
 * @enum os_subcommand_flags_t
 * A type describing flags associated with a subcommand for a command line
 * utility.
 *
 * @const OS_SUBCOMMAND_FLAG_INIT
 * No flags set. This value is suitable for initialization purposes.
 *
 * @const OS_SUBCOMMAND_FLAG_REQUIRE_ROOT
 * This subcommand requires the user to be root. If the user is not root, the
 * {@link os_subcommand_dispatch} routine will return {@EX_PERM}.
 *
 * @const OS_SUBCOMMAND_FLAG_TTYONLY
 * This subcommand may only be invoked via a terminal interface, i.e. it should
 * not be invoked by scripts. Use this option to emphasize that a command's
 * output is human-readably only and should not be parsed.
 *
 * @const OS_SUBCOMMAND_FLAG_HIDDEN
 * This subcommand should not be displayed in the list of subcommands.
 *
 * @const OS_SUBCOMMAND_FLAG_MAIN
 * This subcommand is the "main" subcommand. Designating a main subcommand
 * allows the program to specify and parse global options using an
 * {@link os_subcommand_t} object and {@link os_subcommand_main}.
 *
 * This flag implies the behavior of {@link OS_SUBCOMMAND_FLAG_HIDDEN}.
 *
 * If the program specifies a main subcommand, that subcommand's invocation
 * routine is unconditionally called before calling the subcommand invocation,
 * if the user provided a subcommand. The invocation function for the main
 * subcommand should not exit on success and should instead return 0.
 *
 * If multiple subcommands in the same program set
 * {@link OS_SUBCOMMAND_FLAG_MAIN}, the implementation's behavior is undefined.
 *
 * @const OS_SUBCOMMAND_FLAG_HELPFUL
 * When invoked with no arguments, this subcommand will print usage information
 * to stdout and exit with status zero.
 *
 * @const OS_SUBCOMMAND_FLAG_HELPFUL_FIRST_OPTION
 * Allow the implementation to detect whether the user is attempting to print
 * usage information for the given subcommand. If the implementation concludes
 * that the user is seeking help, it will print the subcommand's usage
 * information to stdout and exit with status 0.
 *
 * The implementation will conclude that the user is seeking help if
 *
 *     - only one argument is provided to the subcommand, and
 *
 * any of the following
 *
 *     - the argument is "-h"
 *     - the argument is "-help"
 *     - the argument is "--help"
 *     - the argument is "help"
 */
DARWIN_API_AVAILABLE_20181020
OS_CLOSED_OPTIONS(os_subcommand_flags, uint64_t,
	OS_SUBCOMMAND_FLAG_INIT,
	OS_SUBCOMMAND_FLAG_REQUIRE_ROOT = (1 << 0),
	OS_SUBCOMMAND_FLAG_TTYONLY = (1 << 1),
	OS_SUBCOMMAND_FLAG_HIDDEN = (1 << 2),
	OS_SUBCOMMAND_FLAG_MAIN = (1 << 3),
	OS_SUBCOMMAND_FLAG_HELPFUL = (1 << 4),
	OS_SUBCOMMAND_FLAG_HELPFUL_FIRST_OPTION = (1 << 5),
);

/*!
 * @typedef os_subcommand_invoke_t
 * An type describing the invocation function for a subcommand.
 *
 * @param osc
 * The descriptor for the command being invoked.
 *
 * @param argc
 * The argument vector count.
 *
 * @param argv
 * The argument vector. The first element of this array is the name of the
 * subcommand.
 *
 * @result
 * An exit code, preferably from sysexits(3). Do not return a POSIX error code
 * directly from this routine.
 *
 * @discussion
 * You may exit directly on success or failure from this routine if desired. If
 * the routine is the invocation for a main subcommand, then on success, the
 * routine should return zero to the caller rather than exiting so that the
 * implementation may continue parsing the command line arguments.
 */
DARWIN_API_AVAILABLE_20181020
typedef int (*os_subcommand_invoke_t)(
	const os_subcommand_t *osc,
	int argc,
	const char *argv[]
);

/*!
 * @struct os_subcommand_t
 * A type describing a subcommand for a command line tool.
 *
 * @field osc_version
 * The version of the structure. Initialize to {@link OS_SUBCOMMAND_VERSION}.
 *
 * @field osc_flags
 * The flags for the subcommand.
 *
 * @field osc_name
 * The name of the subcommand. The second argument of user input will be matched
 * against this name.
 *
 * @field osc_desc
 * A brief description of the subcommand. This description will be displayed
 * next to the subcommand when the user lists all subcommands.
 *
 * @field osc_long_desc
 * A long description of the subcommand. This description will be displayed
 * when the user invokes help on the subcommand. If it's NULL the brief
 * description from osc_desc will be displayed.
 *
 * @field osc_optstring
 * The option string associated with the subcommand. The implementation does not
 * recognize this string directly; the intent of storing it here is to provide a
 * convenient place to access the string for the implementation function.
 * Combined with the {@link osc_options} field, this enables the following
 * pattern:
 *
 *     int ch = 0;
 *     while ((ch = getopt_long(argc, argv, cmd->osc_optstring,
 *             cmd->osc_options, NULL)) != -1) {
 *         switch (ch) {
 *             // process option
 *         }
 *     }
 *
 * This pattern keeps the option string and option structs co-located in code.
 *
 * @field osc_options
 * A pointer to an array of option structures describing the long options
 * recognized by the subcommand. This array must be terminated by a NULL entry
 * as expected by getopt_long(3).
 *
 * @field osc_required
 * A pointer to an array of subcommand option descriptors. The options described
 * in this array are required for the subcommand to execute successfully. This
 * array should be terminated with {@link OS_SUBCOMMAND_OPTION_TERMINATOR}.
 *
 * This array is consulted when printing usage information.
 *
 * @field osc_optional
 * A pointer to an array of subcommand option descriptors. The options described
 * in this array are parsed by the subcommand but not required for it to execute
 * successfully. This array should be terminated with
 * {@link OS_SUBCOMMAND_OPTION_TERMINATOR}.
 *
 * This array is consulted when printing usage information.
 *
 * @field osc_positional
 * A pointer to an array of subcommand option descriptors. The options described
 * in this array are expected to follow the required and optional arguments in
 * the command line invocation, in the order given in this array. This array
 * should be terminated with {@link OS_SUBCOMMAND_OPTION_TERMINATOR}.
 *
 * These options are expected to have their {@link osco_option} fields set to
 * NULL.
 *
 * This array is consulted when printing usage information.
 *
 * @field osc_info
 * A pointer to a function which returns information about the subcommand's
 * individual options.
 *
 * @field osc_invoke
 * The implementation for the subcommand.
 */
DARWIN_API_AVAILABLE_20200401
struct _os_subcommand {
	const os_struct_version_t osc_version;
	const os_subcommand_flags_t osc_flags;
	const char *const osc_name;
	const char *const osc_desc;
	const char *osc_optstring;
	const struct option *osc_options;
	const os_subcommand_option_t *osc_required;
	const os_subcommand_option_t *osc_optional;
	const os_subcommand_option_t *osc_positional;
	const os_subcommand_invoke_t osc_invoke;
	const char *const osc_long_desc;
};

/*!
 * @typedef os_subcommand_main_flags_t
 * Flags modifying the behavior of {@link os_subcommand_main}.
 *
 * @const OS_SUBCOMMAND_MAIN_FLAG_INIT
 * No flags set. This value is suitable for initialization purposes.
 */
OS_CLOSED_OPTIONS(os_subcommand_main_flags, uint64_t,
	OS_SUBCOMMAND_MAIN_FLAG_INIT,
);

/*!
 * @function os_subcommand_main
 * Dispatches the subcommand appropriate for the given arguments. All
 * subcommands will be implicitly discovered by virtue of their delcarations
 * with the OS_SUBCOMMAND_REGISTER attribute.
 *
 * @param argc
 * The argument count supplied to main().
 *
 * @param argv
 * The argument vector supplied to main().
 *
 * @param flags
 * Flags modifying the behavior of the implementation.
 *
 * @result
 * The exit status from the subcommand's invocation function or an exit status
 * from the implementation indicating that the subcommand could not be
 * dispatched. Exit codes that can be returned by the implementation are:
 *
 *     [EX_USAGE]       The subcommand was invoked with improper syntax. Usage
 *                      has been printed to stderr.
 *     [EX_USAGE]       The subcommand specified is not recognized.
 *     [EX_PERM]        The command required root privileges, and the caller is
 *                      not running as root.
 *     [EX_UNAVAILABLE] The command specified that it may only run within
 *                      the context of a tty(3), and either stdin or stdout are
 *                      not a tty(3).
 *
 * @discussion
 * In general, the code returned by this routine should immediately be passed to
 * exit(3). The reason this routine does not implicitly exit is to allow for the
 * caller to process multiple subcommand invocations as a batch.
 *
 * The caller should not print anything after this routine has returned -- the
 * expectation is that all relevant information has already been conveyed to the
 * user either by the implementation or from one of the subcommand invocation
 * routines.
 *
 * This routine implicitly implements a "help" subcommand.
 */
DARWIN_API_AVAILABLE_20191015
OS_EXPORT OS_WARN_RESULT OS_NONNULL2
int
os_subcommand_main(int argc, const char *argv[],
		os_subcommand_main_flags_t flags);

/*!
 * @function os_subcommand_fprintf
 * Prints a message in the context of a subcommand to the given output stream.
 *
 * @param osc
 * The subcommand which represents the context of the message.
 *
 * @param f
 * The stream to which the message shall be printed. If NULL, will be printed to
 * stderr(4).
 *
 * @param fmt
 * A printf(3)-style format string.
 *
 * @param ...
 * The arguments corresponding to {@link fmt}.
 *
 * @discussion
 * This routine provides a uniform method for printing messages in the context
 * of a subcommand. It will ensure that the message is prefixed appropriately
 * (e.g. with the program name and/or subcommand name).
 *
 * This routine should be used for printing messages intended for humans to
 * read; the implementation makes no guarantees about the output format's
 * stability. If any output is intended to be machine-parseable, it should be
 * written with fprintf(3) et al.
 */
DARWIN_API_AVAILABLE_20191015
OS_EXPORT OS_NONNULL3 OS_FORMAT_PRINTF(3, 4)
void
os_subcommand_fprintf(const os_subcommand_t *osc, FILE *f,
		const char *fmt, ...);

/*!
 * @function os_subcommand_vfprintf
 * Prints a message in the context of a subcommand to the given output stream.
 *
 * @param osc
 * The subcommand which represents the context of the message.
 *
 * @param f
 * The stream to which the message shall be printed. If NULL, will be printed to
 * stderr(4).
 *
 * @param fmt
 * A printf(3)-style format string.
 *
 * @param ap
 * The argument list corresponding to {@link fmt}.
 *
 * @discussion
 * See discussion for {@link os_subcommand_fprintf}.
 */
DARWIN_API_AVAILABLE_20191015
OS_EXPORT OS_NONNULL3
void
os_subcommand_vfprintf(const os_subcommand_t *osc, FILE *f,
		const char *fmt, va_list ap);

__END_DECLS;

#endif // __DARWIN_CTL_H
