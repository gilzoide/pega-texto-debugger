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

#ifndef __PEGA_TEXTO_DEBUGGER_PTDB_H__
#define __PEGA_TEXTO_DEBUGGER_PTDB_H__

#include <pega-texto.h>

/**
 * Debugger information and state.
 */
typedef struct {
	/// Match options that will be wrapped by the Debugger ones.
	pt_match_options match_options;
} ptdb_t;

/**
 * Create a debugger for Grammar.
 */
ptdb_t *ptdb_for_grammar(pt_grammar *grammar, pt_match_options *match_options);
/**
 * Create a debugger for Expression.
 */
ptdb_t *ptdb_for_expr(pt_expr *expr, pt_match_options *match_options);
/**
 * Destroy an instance of Debugger, clearing the memory used by it.
 */
void ptdb_destroy(ptdb_t *debugger);

/**
 * Get the Match Options wrapper for a Debugger.
 */
pt_match_options ptdb_match_options_for_debugger(ptdb_t *debugger);

#endif

