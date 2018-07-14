/*
 * Copyright 2018 Gil Barbosa Reis <gilzoide@gmail.com>
 * This file is part of pega-texto-debugger.
 *
 * Pega-texto-debugger is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * Pega-texto-debugger is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this pega-texto-debugger.  If not, see <http://www.gnu.org/licenses/>.
 */

/** @file pega-texto-debugger.h
 */

#ifndef __PEGA_TEXTO_DEBUGGER_H__
#define __PEGA_TEXTO_DEBUGGER_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <pega-texto.h>

/**
 * Options for the debugger, to be OR'ed together.
 */
enum ptdb_options {
	/**
	 * Turn this on if you want the wrapped debugger to be automatically
	 * destroyed on the end of the `pt_match_grammar` call [default: off].
	 */
	PTDB_AUTORELEASE = 1 << 0,
	/**
	 * If on, the debugger REPL will be opened in the next iteration. Turn
	 * this on before start matching to open the REPL in the first iteration.
	 * [default: on].
	 */
	PTDB_BREAK_ON_ITERATION = 1 << 1,
	/**
	 * If on, the debugger REPL will be opened when an Error Expression is
	 * matched [default: on].
	 */
	PTDB_BREAK_ON_ERROR = 1 << 2,
	/**
	 * If on, the debugger REPL will be opened before exiting the match
	 * [default: off].
	 */
	PTDB_BREAK_ON_END = 1 << 3,
	/**
	 * If on, the command history will be saved/loaded in the file
	 * PTDB_HISTORY_FILE_NAME [default: off].
	 */
	PTDB_SHELL_USE_HISTORY = 1 << 4,
	/**
	 * Set of default options.
	 */
	PTDB_DEFAULT_OPTIONS = (PTDB_BREAK_ON_ITERATION | PTDB_BREAK_ON_ERROR),
};

#define PTDB_HISTORY_FILE_NAME ".ptdb_history"

/**
 * Create a debugger from Grammar and match options, wrapping it in the
 * returned match options.
 *
 * This is the only addition needed for debugging a __pega-texto__ match.
 * If you want to just one-shot debug your matching, wrap your Grammar with the
 * #PTDB_AUTORELEASE option enabled and call the `pt_match_grammar` function
 * with the returned match options:
 *
 * @code
 * pt_match_options my_options_with_debugger = ptdb_wrap(grammar, &my_options, PTDB_DEFAULT_OPTIONS | PTDB_AUTORELEASE);
 * pt_match_result result = pt_match_grammar(grammar, str, &my_options_with_debugger);
 * @endcode
 *
 * @param grammar       The wrapped Grammar.
 * @param match_options The user match options.
 * @param opts          Debugger options.
 *
 * @return TODO
 */
pt_match_options ptdb_wrap(pt_grammar *grammar, pt_match_options *match_options, enum ptdb_options opts);

/**
 * Destroy a Debugger that is wrapped in a match options.
 *
 * It is safe to pass a `NULL` pointer. It is also safe to call this function
 * more than once on the same instance.
 *
 * @warning Make sure this match options contain a wrapped Debugger as userdata,
 *          or things will go terribly wrong (XABLAU, SEGFAULT).
 *
 * @param match_options The match options with the wrapped Debugger.
 */
void ptdb_destroy_wrapped(pt_match_options *match_options);

/// Pega-texto-debugger version string, generated by the CMake build.
#define PTDB_VERSION "@PROJECT_VERSION@"

#ifdef __cplusplus
}
#endif

#endif

