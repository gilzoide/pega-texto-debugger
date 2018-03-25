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

#include "pega-texto-debugger.h"

pt_match_options ptdb_wrap(pt_grammar *grammar, pt_match_options *match_options, ptdb_options opts) {
	ptdb_t *debugger = ptdb_for_grammar(grammar, match_options, opts);
	return ptdb_match_options_for_debugger(debugger);
}

void ptdb_destroy_wrapped(pt_match_options *match_options) {
	if(match_options) {
		ptdb_destroy((ptdb_t *) match_options->userdata);
		match_options->userdata = NULL;
	}
}

