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

/** @file ptdb.h
 */

#ifndef __PEGA_TEXTO_DEBUGGER_PTDB_H__
#define __PEGA_TEXTO_DEBUGGER_PTDB_H__

#include <pega-texto.h>

#include "pega-texto-debugger.h"
#include "shell.h"

/**
 * Debugger information and state.
 */
typedef struct ptdb_t {
	/// Match options that will be wrapped by the Debugger ones.
	pt_match_options match_options;
	/// The Grammar that will be debugged.
	pt_grammar *grammar;
	/// Debugger shell data.
	ptdb_shell shell;
	/// Last command executed.
	ptdb_command last_command;
	/// String -> Expression mapping, for fast rule indexing.
	void *rule_table;
	/// State stack level where to break.
	unsigned int level_to_break;
	/// The debugger options.
	int options;
} ptdb_t;

/**
 * Create a debugger for Grammar.
 *
 * @param grammar       Grammar that will be debugged.
 * @param match_options User match options.
 * @param opts          Debugger options.
 */
ptdb_t *ptdb_for_grammar(pt_grammar *grammar, pt_match_options *match_options, enum ptdb_options opts);
/**
 * Destroy an instance of Debugger, clearing the memory used by it.
 *
 * It is safe to pass a `NULL` pointer.
 *
 * @param debugger The Debugger.
 */
void ptdb_destroy(ptdb_t *debugger);

/**
 * Get the match options wrapper for a Debugger.
 *
 * Use the returned match options in `pt_match_grammar` and the Debugger REPL
 * should open.
 *
 * @param debugger The Debugger.
 */
pt_match_options ptdb_match_options_for_debugger(ptdb_t *debugger);

#endif

