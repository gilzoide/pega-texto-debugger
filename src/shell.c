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
#include <Judy.h>

#include <ctype.h>
#include <stdio.h>
#include <string.h>

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
//  Parsing errors
////////////////////////////////////////////////////////////////////////////////
enum {
	PTDB_INVALID_COMMAND = 0,
	PTDB_UNEXPECTED_ARGUMENT,
	PTDB_INVALID_RULE_INDEX,
	PTDB_INVALID_RULE_NAME,
	PTDB_BACKTICK_EXPECTED,
};

char *ptdb_parsing_errors[] = {
	"invalid command, check out `help`",
	"unexpected argument, check out `help COMMAND`",
	"invalid rule index",
	"invalid rule name",
	"closing backtick expected",
};

////////////////////////////////////////////////////////////////////////////////
//  Match actions
////////////////////////////////////////////////////////////////////////////////
typedef struct {
	ptdb_t *debugger;
	ptdb_command *cmd;
} ptdb_command_match_userdata;

#define command_match_action(op) \
	static pt_data _ ## op(const char *str, size_t begin, size_t end, int argc, pt_data *argv, void *data) { \
		return (pt_data){ .i = PTDB_ ## op }; \
	}
command_match_action(HELP)
command_match_action(STEP)
command_match_action(NEXT)
command_match_action(CONTINUE)
command_match_action(FINISH)
command_match_action(BACKTRACE)
command_match_action(LIST)
command_match_action(PRINT)
command_match_action(RULE)
command_match_action(BREAK_EXPR)
command_match_action(MEMORY)

static pt_data _last_command(const char *str, size_t begin, size_t end, int argc, pt_data *argv, void *data) {
	ptdb_command_match_userdata *userdata = data;
	*userdata->cmd = userdata->debugger->last_command;
	return PT_NULL_DATA;
}
static pt_data _help(const char *str, size_t begin, size_t end, int argc, pt_data *argv, void *data) {
	ptdb_command_match_userdata *userdata = data;
	userdata->cmd->opcode = PTDB_HELP;
	userdata->cmd->data.opcode = argc > 0 ? argv[0].i : PTDB_OPCODE_MAX;
	return PT_NULL_DATA;
}
static pt_data _next(const char *str, size_t begin, size_t end, int argc, pt_data *argv, void *data) {
	ptdb_command_match_userdata *userdata = data;
	userdata->cmd->opcode = PTDB_NEXT;
	return PT_NULL_DATA;
}
static pt_data _step(const char *str, size_t begin, size_t end, int argc, pt_data *argv, void *data) {
	ptdb_command_match_userdata *userdata = data;
	userdata->cmd->opcode = PTDB_STEP;
	return PT_NULL_DATA;
}
static pt_data _continue(const char *str, size_t begin, size_t end, int argc, pt_data *argv, void *data) {
	ptdb_command_match_userdata *userdata = data;
	userdata->cmd->opcode = PTDB_CONTINUE;
	return PT_NULL_DATA;
}
static pt_data _finish(const char *str, size_t begin, size_t end, int argc, pt_data *argv, void *data) {
	ptdb_command_match_userdata *userdata = data;
	userdata->cmd->opcode = PTDB_FINISH;
	return PT_NULL_DATA;
}
static pt_data _backtrace(const char *str, size_t begin, size_t end, int argc, pt_data *argv, void *data) {
	ptdb_command_match_userdata *userdata = data;
	userdata->cmd->opcode = PTDB_BACKTRACE;
	userdata->cmd->data.depth = -1;
	return PT_NULL_DATA;
}
static pt_data _list(const char *str, size_t begin, size_t end, int argc, pt_data *argv, void *data) {
	ptdb_command_match_userdata *userdata = data;
	userdata->cmd->opcode = PTDB_LIST;
	return PT_NULL_DATA;
}
static pt_data toint(const char *str, size_t begin, size_t end, int argc, pt_data *argv, void *data) {
	return (pt_data){ .i = atoi(str + begin) };
}
static pt_data _rule_arg_num(const char *str, size_t begin, size_t end, int argc, pt_data *argv, void *data) {
	ptdb_command_match_userdata *userdata = data;
	int arg = argv[0].i;
	return (pt_data){ .i = arg < userdata->debugger->grammar->N ? arg : -PTDB_INVALID_RULE_INDEX };
}
static pt_data _rule_arg_str(const char *str, size_t begin, size_t end, int argc, pt_data *argv, void *data) {
	ptdb_command_match_userdata *userdata = data;
	if(str[begin] == '`') {
		begin++;
		end--;
	}
	char rule_name[end - begin + 1];
	strncpy(rule_name, str + begin, end - begin);
	rule_name[end - begin] = '\0';
	Word_t *pvalue;
	JSLG(pvalue, userdata->debugger->rule_table, (uint8_t *) rule_name);
	return (pt_data){ .i = pvalue ? *pvalue : -PTDB_INVALID_RULE_NAME };
}
static pt_data _rule(const char *str, size_t begin, size_t end, int argc, pt_data *argv, void *data) {
	ptdb_command_match_userdata *userdata = data;
	if(argc > 0) {
		if(argv[0].i < 0) {
			userdata->cmd->opcode = PTDB_INVALID;
			userdata->cmd->data.str = ptdb_parsing_errors[-argv[0].i];
		}
		else {
			userdata->cmd->opcode = PTDB_RULE;
			userdata->cmd->data.index = argv[0].i;
		}
	}
	else {
		userdata->cmd->opcode = PTDB_RULE;
		userdata->cmd->data.index = -1;
	}
	return PT_NULL_DATA;
}
static pt_data _memory(const char *str, size_t begin, size_t end, int argc, pt_data *argv, void *data) {
	ptdb_command_match_userdata *userdata = data;
	userdata->cmd->opcode = PTDB_MEMORY;
	userdata->cmd->data.memory_target_grammar = str[end - 1] == 'r' || str[end - 1] == 'g';
	return PT_NULL_DATA;
}

////////////////////////////////////////////////////////////////////////////////
//  Custom matchers
////////////////////////////////////////////////////////////////////////////////
static int isnotspace(int c) {
	return !isspace(c) && c;
}
static int eof(int c) {
	return !c;
}

////////////////////////////////////////////////////////////////////////////////
#include <pega-texto/macro-on.h>
pt_grammar *ptdb_create_command_grammar() {
	pt_rule rules[] = {
		{ "axiom", SEQ(V("S*"),
		               OR(C_(_last_command, eof),
		                  OR(V("help"),
						     V("step"),
						     V("next"),
						     V("continue"),
						     V("finish"),
						     V("backtrace"),
						     V("list"),
						     /* V("print"), */
						     V("rule"),
						     /* V("break"), */
						     V("memory"),
						     E(PTDB_INVALID_COMMAND, NULL)),
		                  V("S*"), OR(NOT(ANY), E(PTDB_UNEXPECTED_ARGUMENT, NULL)))) },
		{ "help", SEQ_(_help, OR(L("help"), L("h")),
		                      Q(SEQ(V("S+"),
		                            OR(OR_(_HELP, L("help"), L("h")),
		                               OR_(_STEP, L("step"), L("s")),
		                               OR_(_NEXT, L("next"), L("n")),
		                               OR_(_CONTINUE, L("continue"), L("c")),
		                               OR_(_FINISH, L("finish"), L("f")),
		                               OR_(_BACKTRACE, L("backtrace"), L("bt")),
		                               OR_(_LIST, L("list"), L("l")),
		                               OR_(_PRINT, L("print"), L("p")),
		                               OR_(_RULE, L("rule"), L("r")),
		                               OR_(_BREAK_EXPR, L("break"), L("b")),
		                               OR_(_MEMORY, L("memory"), L("m")),
									   E(PTDB_INVALID_COMMAND, NULL))
								   ), -1)) },
		{ "next", OR_(_next, L("next"), L("n")) },
		{ "step", OR_(_step, L("step"), L("s")) },
		{ "continue", OR_(_continue, L("continue"), L("c")) },
		{ "finish", OR_(_finish, L("finish"), L("f")) },
		{ "backtrace", OR_(_backtrace, L("backtrace"), L("bt")) },
		{ "list", OR_(_list, L("list"), L("l")) },
		{ "rule", SEQ_(_rule, OR(L("rule"), L("r")),
		                      Q(SEQ(V("S+"),
		                            OR(V_(_rule_arg_num, "number"),
		                               V_(_rule_arg_str, "rule-name"))
		                      ), -1)) },
		{ "rule-name", OR(SEQ(L("`"), Q(BUT(L("`")), 1), OR(L("`"), E(PTDB_BACKTICK_EXPECTED, NULL))),
		                  Q(C(isnotspace), 1)) },
		{ "memory", SEQ_(_memory, OR(L("memory"), L("m")),
		                          Q(SEQ(V("S+"),
		                                OR(L("grammar"), L("g"))
								  ), -1)) },
		{ "number", Q_(toint, C(isdigit), 1) },
		{ "S+", Q(C(isspace), 1) },
		{ "S*", Q(C(isspace), 0) },
		{ NULL, NULL },
	};
	pt_grammar *grammar = pt_create_grammar(rules, 0);
	pt_validate_grammar(grammar, PT_VALIDATE_PRINT_ERROR);
	return grammar;
}
#include <pega-texto/macro-off.h>

ptdb_command ptdb_read_command(ptdb_t *debugger, const char *line) {
	ptdb_command cmd = {};
	ptdb_command_match_userdata data = { .debugger = debugger, .cmd = &cmd };

	pt_match_options opts = { .userdata = &data };
	pt_match_result res = pt_match_grammar(debugger->shell.command_grammar, line, &opts);
	switch(res.matched) {
		case PT_NO_MATCH:
			cmd = (ptdb_command){ PTDB_INVALID, { "no match" }};
			break;
		case PT_NO_STACK_MEM:
			cmd = (ptdb_command){ PTDB_INVALID, { "insuficient memory for cmd parser" }};
			break;
		case PT_MATCHED_ERROR:
			cmd = (ptdb_command){ PTDB_INVALID, { ptdb_parsing_errors[res.data.i] }};
			break;
		case PT_NULL_INPUT:
			cmd.opcode = PTDB_FINISH;
			break;
		default:
			break;
	}

	return cmd;
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
		cmd = ptdb_read_command(debugger, line);
		if(cmd.opcode == PTDB_INVALID) {
			replxx_print(shell->replxx, ERROR_COLOR "error" NORMAL_COLOR ": %s\n", cmd.data.str);
		}
		else {
			debugger->last_command = cmd;
			break;
		}
	}
	return cmd;
}

