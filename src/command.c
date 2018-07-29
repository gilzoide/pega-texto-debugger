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

#include "command.h"
#include "pega-texto-debugger.h"
#include "print_expr.h"
#include "ptdb.h"

#include <pega-texto-memory.h>
#include <replxx.h>

#include <stdio.h>

int ptdb_command_resume_matching(ptdb_command cmd) {
	switch(cmd.opcode) {
		case PTDB_HELP:
		case PTDB_BACKTRACE:
		case PTDB_LIST:
		case PTDB_PRINT:
		case PTDB_RULE:
		case PTDB_BREAK_EXPR:
		case PTDB_BREAK_RULE:
		case PTDB_BREAK_INPUT_POS:
		case PTDB_BREAK_END:
		case PTDB_BREAK_ERROR:
		case PTDB_MEMORY:
		default:
			return 0;

		case PTDB_STEP:
		case PTDB_NEXT:
		case PTDB_CONTINUE:
		case PTDB_FINISH:
			return 1;
	}
}

static inline void ptdb_help(ptdb_t *debugger, ptdb_opcode op) {
	const char *help_string;
	switch(op) {
		case PTDB_HELP:
			help_string =
			"help [COMMAND]\n"
			"\n"
			"Print the help text for selected command"
			;
			break;
		case PTDB_STEP:
			help_string =
			"step\n"
			;
			break;
		case PTDB_NEXT:
			help_string =
			"next\n"
			;
			break;
		case PTDB_CONTINUE:
			help_string =
			"continue\n"
			;
			break;
		case PTDB_FINISH:
			help_string =
			"finish\n"
			;
			break;
		case PTDB_BACKTRACE:
			help_string =
			"backtrace\n"
			;
			break;
		case PTDB_LIST:
			help_string =
			"list\n"
			;
			break;
		case PTDB_PRINT:
			help_string =
			"print\n"
			;
			break;
		case PTDB_RULE:
			help_string =
			"rule\n"
			;
			break;
		case PTDB_BREAK_EXPR:
		case PTDB_BREAK_RULE:
		case PTDB_BREAK_INPUT_POS:
		case PTDB_BREAK_END:
		case PTDB_BREAK_ERROR:
			help_string =
			"break\n"
			;
			break;
		case PTDB_MEMORY:
			help_string =
			"memory [grammar]\n"
			;
			break;
		case PTDB_OPCODE_MAX:
			help_string =
			"help [COMMAND]\n"
			"\n"
			"Available commands:\n"
			"\n"
			"help\n"
			"step\n"
			"next\n"
			"continue\n"
			"finish\n"
			"backtrace\n"
			"list\n"
			"print\n"
			"rules\n"
			"break\n"
			"memory"
			;
			break;
		default:
			help_string = "invalid command";
			break;
	}
	replxx_print(debugger->shell.replxx, "%s\n", help_string);
}

static inline void ptdb_next(ptdb_t *debugger, int current_level) {
	debugger->level_to_break = current_level;
}

static inline void ptdb_step(ptdb_t *debugger, int current_level) {
	debugger->level_to_break = current_level + 1;
}

static inline void ptdb_continue(ptdb_t *debugger) {
	debugger->options ^= PTDB_BREAK_ON_ITERATION;
	debugger->level_to_break = 0;
}

static inline void ptdb_finish(ptdb_t *debugger) {
	debugger->options ^= PTDB_BREAK_ON_ITERATION | PTDB_BREAK_ON_ERROR | PTDB_BREAK_ON_END;
	debugger->level_to_break = 0;
}

static void ptdb_print_match_state(Replxx *replxx, const pt_match_state *state) {
	pt_expr *expr = state->e;
	switch(expr->op) {
		case PT_LITERAL:
			replxx_print(replxx, "{ op: L, str: \"%s\" }", expr->data.characters);
			break;
		case PT_CASE_INSENSITIVE:
			replxx_print(replxx, "{ op: I, str: \"%s\" }", expr->data.characters);
			break;
		case PT_CHARACTER_CLASS:
			replxx_print(replxx, "{ op: C, class: ");
			ptdb_print_expr(replxx, expr);
			replxx_print(replxx, " }");
			break;
		case PT_SET:
			replxx_print(replxx, "{ op: S, set: ");
			ptdb_print_expr(replxx, expr);
			replxx_print(replxx, " }");
			break;
		case PT_RANGE:
			replxx_print(replxx, "{ op: R, range: ");
			ptdb_print_expr(replxx, expr);
			replxx_print(replxx, " }");
			break;
		case PT_ANY:
			replxx_print(replxx, "{ op: ANY }");
			break;
		case PT_NON_TERMINAL:
			replxx_print(replxx, "{ op: V, rule: ");
			ptdb_print_expr(replxx, expr);
			replxx_print(replxx, " }");
			break;
		case PT_QUANTIFIER:
			replxx_print(replxx, "{ op: Q, N: %d, current: %d, chars matched: %d }", expr->N, state->r1, state->r2);
			break;
		case PT_AND:
			replxx_print(replxx, "{ op: AND }");
			break;
		case PT_NOT:
			replxx_print(replxx, "{ op: NOT }");
			break;
		case PT_SEQUENCE:
			replxx_print(replxx, "{ op: SEQ, current: %d, chars matched: %d }", state->r1, state->r2);
			break;
		case PT_CHOICE:
			replxx_print(replxx, "{ op: OR, current: %d }", state->r1);
			break;
		case PT_CUSTOM_MATCHER:
			replxx_print(replxx, "{ op: F, matcher: ");
			ptdb_print_expr(replxx, expr);
			replxx_print(replxx, " }");
			break;
		case PT_ERROR:
			replxx_print(replxx, "{ op: E }");
			break;
	}
}
static void ptdb_print_backtrace_line(Replxx *replxx, int index, const pt_match_state *state, int rule_index, const char **rule_names) {
	replxx_print(replxx, "%d) ", index);
	if(rule_names[rule_index]) {
		replxx_print(replxx, "%s ", rule_names[rule_index]);
	}
	else {
		replxx_print(replxx, "<%d> ", rule_index);
	}
	ptdb_print_match_state(replxx, state);
	replxx_print(replxx, "\n");
}
static inline void ptdb_backtrace(ptdb_t *debugger, const pt_match_state_stack *s, int depth_to_print) {
	int i, j, previous_rule_index, rule_index;
	depth_to_print = depth_to_print < 0 ? s->size : depth_to_print;
		
	const int expr_position_line_size = 3 * s->size;
	char expr_position_line[expr_position_line_size];
	expr_position_line[expr_position_line_size - 1] = '\0';
	int expr_position_length = 0;

	const pt_match_state *state;
	const pt_expr *expr;
	for(i = previous_rule_index = s->size - 1; i > 0; i--) {
		state = s->states + i;
		expr = state->e;
		if(expr->op == PT_NON_TERMINAL) {
			rule_index = expr->N;
			for(j = previous_rule_index; j > i; j--) {
				state = s->states + j;
				ptdb_print_backtrace_line(debugger->shell.replxx, j, state, rule_index, debugger->grammar->names);
				if(--depth_to_print <= 0) return;
			}
			previous_rule_index = i;
		}
	}
	for(j = previous_rule_index; j >= 0; j--) {
		state = s->states + j;
		ptdb_print_backtrace_line(debugger->shell.replxx, j, state, 0, debugger->grammar->names);
		if(--depth_to_print <= 0) return;
	}
}

static inline void ptdb_list(ptdb_t *debugger, const char *str, size_t pos) {
	int i;
	replxx_print(debugger->shell.replxx, "%s\n", str);
	for(i = 0; i < pos; i++) {
		replxx_print(debugger->shell.replxx, ".");
	}
	replxx_print(debugger->shell.replxx, "^");
	for(i = pos + 1; str[i]; i++) {
		replxx_print(debugger->shell.replxx, ".");
	}
	replxx_print(debugger->shell.replxx, "\n");
}

static inline void ptdb_rule(ptdb_t *debugger, int index) {
	if(index < 0) {
		ptdb_print_grammar(debugger->shell.replxx, debugger->grammar);
	}
	else {
		ptdb_print_rule(debugger->shell.replxx, debugger->grammar, index);
	}
}

static inline void ptdb_memory(int memory_target_grammar, ptdb_t *debugger, const pt_match_state_stack *s, const pt_match_action_stack *a) {
	if(memory_target_grammar) {
		pt_memory memory_usage = pt_memory_for_grammar(debugger->grammar);
		printf("Expression: %d\n", memory_usage.expression_byte_count);
		printf("Grammar:    %d\n", memory_usage.grammar_byte_count);
		printf("Array:      %d\n", memory_usage.array_byte_count);
		printf("String:     %d\n", memory_usage.string_byte_count);
		printf("Total:      %d\n", memory_usage.expression_byte_count
				+ memory_usage.grammar_byte_count
				+ memory_usage.array_byte_count
				+ memory_usage.string_byte_count);
	}
	else {
		pt_memory_match_stacks memory_usage = pt_memory_for_match_stacks(s, a);
		printf("State stack:  %d / %d\n", memory_usage.state_stack_used_byte_count, memory_usage.state_stack_total_byte_count);
		printf("Action stack: %d / %d\n", memory_usage.action_stack_used_byte_count, memory_usage.action_stack_total_byte_count);
	}
}

void ptdb_run_command(ptdb_t *debugger, ptdb_command cmd, const pt_match_state_stack *s, const pt_match_action_stack *a, const char *str) {
	switch(cmd.opcode) {
		case PTDB_HELP:
			ptdb_help(debugger, cmd.data.opcode);
			break;
		case PTDB_STEP:
			ptdb_step(debugger, s->size);
			break;
		case PTDB_NEXT:
			ptdb_next(debugger, s->size);
			break;
		case PTDB_CONTINUE:
			ptdb_continue(debugger);
			break;
		case PTDB_FINISH:
			ptdb_finish(debugger);
			break;
		case PTDB_BACKTRACE:
			ptdb_backtrace(debugger, s, cmd.data.depth);
			break;
		case PTDB_LIST:
			ptdb_list(debugger, str, pt_get_current_state(s)->pos + pt_get_current_state(s)->r2);
			break;
		case PTDB_PRINT:
			break;
		case PTDB_RULE:
			ptdb_rule(debugger, cmd.data.index);
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
		case PTDB_MEMORY:
			ptdb_memory(cmd.data.memory_target_grammar, debugger, s, a);
			break;
	}
}

