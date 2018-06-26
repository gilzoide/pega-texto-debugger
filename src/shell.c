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

#include <pega-texto.h>

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

ptdb_command ptdb_read_command(ptdb_shell *shell, const char *line) {
	ptdb_command command = {};

	pt_match_options opts = {};
	pt_match_result res = pt_match_grammar(shell->command_grammar, line, &opts);
	switch(res.matched) {
		case PT_NO_MATCH:
			command.opcode = PTDB_INVALID;
			command.data.str = "invalid command";
			break;
		case PT_NO_STACK_MEM:
			command.opcode = PTDB_INVALID;
			command.data.str = "unsuficient memory for command parser";
			break;
		case PT_MATCHED_ERROR:
			command.opcode = PTDB_INVALID;
			command.data.str = "matched error";
			break;
		case PT_NULL_INPUT:
			command.opcode = PTDB_FINISH;
			break;
		default:
			//TODO
			break;
	}

	return command;
}

#define PROMPT_COLOR "\x1b[32m"
#define NORMAL_COLOR "\x1b[0m"
#define ERROR_COLOR "\x1b[1;31m"

void ptdb_prompt_shell(ptdb_t *debugger) {
	ptdb_shell *shell = &debugger->shell;
	const char *line = replxx_input(shell->replxx, PROMPT_COLOR "ptdb" NORMAL_COLOR "> ");
	ptdb_command cmd = ptdb_read_command(shell, line);
	switch(cmd.opcode) {
		case PTDB_INVALID:
			replxx_print(shell->replxx, ERROR_COLOR "error" NORMAL_COLOR ": %s\n", cmd.data.str);
			break;
		case PTDB_HELP:
			break;
		case PTDB_STEP:
			break;
		case PTDB_NEXT:
			break;
		case PTDB_CONTINUE:
			break;
		case PTDB_FINISH:
			break;
		case PTDB_BACKTRACE:
			break;
		case PTDB_LIST:
			break;
		case PTDB_PRINT:
			break;
		case PTDB_RULES:
			break;
		case PTDB_BREAK_EXPR:
			break;
		case PTDB_BREAK_RULE:
			break;
		case PTDB_BREAK_INPUT_POS:
			break;
		case PTDB_BREAK_END:
			break;
		case PTDB_BREAK_ERROR:
			break;
	}
}

