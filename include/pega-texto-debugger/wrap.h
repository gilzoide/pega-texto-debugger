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

/** @file wrap.h
 */

#ifndef __PEGA_TEXTO_DEBUGGER_WRAP_H__
#define __PEGA_TEXTO_DEBUGGER_WRAP_H__

/**
 * Create a debugger from Grammar and match options, wrapping it in the
 * returned match options.
 *
 * This may be the only addition needed for one-shot debugging a __pega-texto__
 * match: just wrap your Grammar with the #PTDB_AUTORELEASE option enabled and
 * call the `pt_match_grammar` function with the returned match options:
 *
 * @code
 * pt_match_options my_options_with_debugger = ptdb_wrap(grammar, &my_options, PTDB_AUTORELEASE);
 * pt_match_result result = pt_match_grammar(grammar, str, &my_options_with_debugger);
 * @endcode
 *
 * @param grammar       The wrapped Grammar.
 * @param match_options The user match options.
 * @param opts          Debugger options.
 *
 * @return 
 */
pt_match_options ptdb_wrap(pt_grammar *grammar, pt_match_options *match_options, ptdb_options opts);

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

#endif

