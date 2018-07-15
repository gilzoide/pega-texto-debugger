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

#include <ctype.h>
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

void ptdb_shell_save_history(ptdb_shell *shell, const char *filename) {
	replxx_history_save(shell->replxx, filename);
}

void ptdb_shell_load_history(ptdb_shell *shell, const char *filename) {
	replxx_history_load(shell->replxx, filename);
}

////////////////////////////////////////////////////////////////////////////////
//  Match actions
////////////////////////////////////////////////////////////////////////////////
#define command_match_action(op) \
	static pt_data _ ## op(const char *str, size_t begin, size_t end, int argc, pt_data *argv, void *data) { \
		return (pt_data){ .d = PTDB_ ## op }; \
	}
command_match_action(HELP)
command_match_action(STEP)
command_match_action(NEXT)
command_match_action(CONTINUE)
command_match_action(FINISH)
command_match_action(BACKTRACE)
command_match_action(LIST)
command_match_action(PRINT)
command_match_action(RULES)
command_match_action(BREAK_EXPR)
command_match_action(MEMORY)

static pt_data _help(const char *str, size_t begin, size_t end, int argc, pt_data *argv, void *data) {
	ptdb_command *cmd = data;
	cmd->opcode = PTDB_HELP;
	cmd->data.opcode = argc > 0 ? argv[0].d : PTDB_OPCODE_MAX;
	return PT_NULL_DATA;
}
static pt_data _continue(const char *str, size_t begin, size_t end, int argc, pt_data *argv, void *data) {
	ptdb_command *cmd = data;
	cmd->opcode = PTDB_CONTINUE;
	return PT_NULL_DATA;
}
static pt_data _finish(const char *str, size_t begin, size_t end, int argc, pt_data *argv, void *data) {
	ptdb_command *cmd = data;
	cmd->opcode = PTDB_FINISH;
	return PT_NULL_DATA;
}
static pt_data _memory(const char *str, size_t begin, size_t end, int argc, pt_data *argv, void *data) {
	ptdb_command *cmd = data;
	cmd->opcode = PTDB_MEMORY;
	cmd->data.memory_target_grammar = (end - begin) > 6; // strlen("memory") == 6
	return PT_NULL_DATA;
}

////////////////////////////////////////////////////////////////////////////////
//  Syntax errors
////////////////////////////////////////////////////////////////////////////////
enum {
	PTDB_INVALID_COMMAND = 0,
	PTDB_UNEXPECTED_ARGUMENT,
};

char *ptdb_syntax_errors[] = {
	"invalid command, check out `help`",
	"unexpected argument, check out `help COMMAND`",
};
////////////////////////////////////////////////////////////////////////////////
#include <pega-texto/macro-on.h>
pt_grammar *ptdb_create_command_grammar() {
	pt_rule rules[] = {
		{ "axiom", SEQ(V("S*"),
		               OR(V("help"),
						  /* V("step"), */
						  /* V("next"), */
						  V("continue"),
						  V("finish"),
						  /* V("backtrace"), */
						  /* V("list"), */
						  /* V("print"), */
						  /* V("rules"), */
						  /* V("break"), */
						  V("memory"),
						  E(PTDB_INVALID_COMMAND, NULL)),
		               V("S*"), OR(NOT(ANY), E(PTDB_UNEXPECTED_ARGUMENT, NULL))) },
		{ "help", SEQ_(_help, I("help"),
		                      Q(SEQ(V("S+"),
		                            OR(I_(_HELP, "help"),
		                               I_(_STEP, "step"),
		                               I_(_NEXT, "next"),
		                               I_(_CONTINUE, "continue"),
		                               I_(_FINISH, "finish"),
		                               I_(_BACKTRACE, "backtrace"),
		                               I_(_LIST, "list"),
		                               I_(_PRINT, "print"),
		                               I_(_RULES, "rules"),
		                               I_(_BREAK_EXPR, "break"),
		                               I_(_MEMORY, "memory"))
								   ), -1)) },
		{ "continue", I_(_continue, "continue") },
		{ "finish", I_(_finish, "finish") },
		{ "memory", SEQ_(_memory, I("memory"),
		                          Q(SEQ(V("S+"),
		                                I("grammar")
								  ), -1)) },
		{ "S+", Q(C(isspace), 1) },
		{ "S*", Q(C(isspace), 0) },
		{ NULL, NULL },
	};
	pt_grammar *grammar = pt_create_grammar(rules, 0);
	pt_validate_grammar(grammar, PT_VALIDATE_PRINT_ERROR);
	return grammar;
}
#include <pega-texto/macro-off.h>

ptdb_command ptdb_read_command(ptdb_shell *shell, const char *line) {
	ptdb_command command = {};

	pt_match_options opts = { .userdata = &command };
	pt_match_result res = pt_match_grammar(shell->command_grammar, line, &opts);
	switch(res.matched) {
		case PT_NO_MATCH:
			command = (ptdb_command){ PTDB_INVALID, { "no match" }};
			break;
		case PT_NO_STACK_MEM:
			command = (ptdb_command){ PTDB_INVALID, { "insuficient memory for command parser" }};
			break;
		case PT_MATCHED_ERROR:
			command = (ptdb_command){ PTDB_INVALID, { ptdb_syntax_errors[res.data.i] }};
			break;
		case PT_NULL_INPUT:
			command.opcode = PTDB_FINISH;
			break;
		default:
			break;
	}

	return command;
}

#define PROMPT_COLOR "\x1b[32m"
#define NORMAL_COLOR "\x1b[0m"
#define ERROR_COLOR "\x1b[1;31m"

ptdb_command ptdb_prompt_shell(ptdb_t *debugger) {
	ptdb_shell *shell = &debugger->shell;
	ptdb_command cmd;
	while(1) {
		const char *line = replxx_input(shell->replxx, PROMPT_COLOR "ptdb" NORMAL_COLOR "> ");
		if(line != NULL) replxx_history_add(shell->replxx, line);
		cmd = ptdb_read_command(shell, line);
		if(cmd.opcode == PTDB_INVALID) {
			replxx_print(shell->replxx, ERROR_COLOR "error" NORMAL_COLOR ": %s\n", cmd.data.str);
		}
		else {
			break;
		}
	}
	return cmd;
}

