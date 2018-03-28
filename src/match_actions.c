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

#include "match_actions.h"
#include "ptdb.h"

void ptdb_each_iteration(const pt_match_state_stack *s, const pt_match_action_stack *a, const char *str, void *data) {
	ptdb_t *debugger = data;
	if(debugger) {
		// call user define each_iteration
		if(debugger->match_options.each_iteration) debugger->match_options.each_iteration(s, a, str, debugger->match_options.userdata);

		// open REPL on first time, if stepping or on breakpoints
	}
}

void ptdb_on_end(const pt_match_state_stack *s, const pt_match_action_stack *a, const char *str, pt_match_result result, void *data) {
	ptdb_t *debugger = data;
	if(debugger) {
		// deallocate stuff if needed
		// reset stuff if needed

		// call user define on_end
		if(debugger->match_options.on_end) debugger->match_options.on_end(s, a, str, result, debugger->match_options.userdata);
	}
}

void ptdb_each_success(const pt_match_state_stack *s, const pt_match_action_stack *a, const char *str, size_t begin, size_t end, void *data) {
	ptdb_t *debugger = data;
	if(debugger) {
		// call user define each_success
		if(debugger->match_options.each_success) debugger->match_options.each_success(s, a, str, begin, end, debugger->match_options.userdata);
	}
}

void ptdb_each_fail(const pt_match_state_stack *s, const pt_match_action_stack *a, const char *str, void *data) {
	ptdb_t *debugger = data;
	if(debugger) {
		// call user define each_fail
		if(debugger->match_options.each_fail) debugger->match_options.each_fail(s, a, str, debugger->match_options.userdata);
	}
}

void ptdb_on_error(const char *str, size_t where, int code, void *data) {
	ptdb_t *debugger = data;
	if(debugger) {
		// open REPL if breakpoint on errors

		// call user define on_error
		if(debugger->match_options.on_error) debugger->match_options.on_error(str, where, code, debugger->match_options.userdata);
	}
}

