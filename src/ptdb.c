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

#include <pega-texto-debugger/ptdb.h>

/**
 * Allocate a Debugger with the options passed.
 */
ptdb_t *ptdb_new(pt_match_options *match_options) {
	ptdb_t *debugger;
	if(debugger = malloc(sizeof(ptdb_t))) {
		if(match_options == NULL) match_options = &pt_default_match_options;
		debugger->match_options = *match_options;
	}
	return debugger;
}

ptdb_t *ptdb_for_expr(pt_expr *expr, pt_match_options *match_options) {
	ptdb_t *debugger;
	if(debugger = ptdb_new(match_options)) {
	}
	return debugger;
}

ptdb_t *ptdb_for_grammar(pt_grammar *grammar, pt_match_options *match_options) {
	ptdb_t *debugger;
	if(debugger = ptdb_new(match_options)) {
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
	};
}

