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

ptdb_t *ptdb_for_grammar(pt_grammar *grammar, pt_match_options *match_options, ptdb_options opts) {
	ptdb_t *debugger;
	if(debugger = malloc(sizeof(ptdb_t))) {
		if(match_options == NULL) match_options = &pt_default_match_options;
		debugger->match_options = *match_options;
		debugger->grammar = grammar;
		debugger->options = opts;
	}
	return debugger;
}

void ptdb_destroy(ptdb_t *debugger) {
	if(debugger) {
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

