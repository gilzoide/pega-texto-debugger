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

#include "shell.h"
#include "ptdb.h"

#include <stdio.h>

int ptdb_shell_init(ptdb_shell *shell) {
	return (shell->command_grammar = ptdb_create_command_grammar())
	       && (shell->replxx = replxx_init());
}

void ptdb_shell_destroy(ptdb_shell *shell) {
	if(shell) {
		pt_destroy_grammar(shell->command_grammar);
		replxx_end(shell->replxx);
	}
}

#include <pega-texto/macro-on.h>
pt_grammar *ptdb_create_command_grammar() {
	pt_rule rules[] = {
		{ "help", I("help") },
		{ NULL, NULL },
	};
	pt_grammar *grammar = pt_create_grammar(rules, 0);
	pt_validate_grammar(grammar, PT_VALIDATE_PRINT_ERROR);
	return grammar;
}
#include <pega-texto/macro-off.h>

ptdb_command ptdb_read_command(const char *line) {
	ptdb_command command = {};
	return command;
}

void ptdb_prompt_shell(ptdb_t *debugger) {
	const char *line = replxx_input(debugger->shell.replxx, "ptdb> ");
}

