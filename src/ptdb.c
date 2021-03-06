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

#include "ptdb.h"
#include "match_actions.h"

#include <Judy.h>

void ptdb_recurse_expr(ptdb_t *debugger, pt_expr *expr) {
	// TODO: save actions to restore later?
	expr->action = NULL;
	int i;
	switch(expr->op) {
		case PT_CHOICE:
		case PT_SEQUENCE:
			for(i = 0; i < expr->N; i++) {
				ptdb_recurse_expr(debugger, expr->data.es[i]);
			}
			break;
		default:
			break;
	}
}

int ptdb_initialize_grammar_metadata(ptdb_t *debugger, pt_grammar *grammar) {
	Word_t *pvalue;
	int i;
	debugger->rule_table = NULL;
	for(i = 0; i < grammar->N; i++) {
		JSLI(pvalue, debugger->rule_table, (uint8_t *) grammar->names[i]);
		if(pvalue == PJERR) return 0;
		*pvalue = i;

		ptdb_recurse_expr(debugger, grammar->es[i]);
	}
	return 1;
}

ptdb_t *ptdb_for_grammar(pt_grammar *grammar, pt_match_options *match_options, enum ptdb_options opts) {
	ptdb_t *debugger;
	if(debugger = malloc(sizeof(ptdb_t))) {
		if(!ptdb_shell_init(&debugger->shell)) {
			ptdb_destroy(debugger);
			return NULL;
		}
		else if(opts & PTDB_SHELL_USE_HISTORY) {
			ptdb_shell_load_history(&debugger->shell, PTDB_HISTORY_FILE_NAME);
		}

		if(!ptdb_initialize_grammar_metadata(debugger, grammar)) {
			ptdb_destroy(debugger);
			return NULL;
		}

		if(match_options == NULL) match_options = &pt_default_match_options;
		debugger->last_command = (ptdb_command){ .opcode = PTDB_HELP, { .opcode = PTDB_OPCODE_MAX }};
		debugger->match_options = *match_options;
		debugger->grammar = grammar;
		debugger->level_to_break = INT_MAX;
		debugger->options = opts;
	}
	return debugger;
}

void ptdb_destroy(ptdb_t *debugger) {
	if(debugger) {
		Word_t rc_word;
		if(debugger->options & PTDB_SHELL_USE_HISTORY) {
			ptdb_shell_save_history(&debugger->shell, PTDB_HISTORY_FILE_NAME);
		}
		ptdb_shell_destroy(&debugger->shell);
		JSLFA(rc_word, debugger->rule_table);
		free(debugger);
	}
}

pt_match_options ptdb_match_options_for_debugger(ptdb_t *debugger) {
	return (pt_match_options){
		.userdata = debugger,
		.each_iteration = ptdb_each_iteration,
		.on_end = ptdb_on_end,
		.each_success = ptdb_each_success,
		.each_fail = ptdb_each_fail,
		.on_error = ptdb_on_error,
	};
}

