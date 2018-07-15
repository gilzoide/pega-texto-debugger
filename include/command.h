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

/** @file command.h
 * Commands available at the REPL.
 */

#ifndef __PEGA_TEXTO_DEBUGGER_COMMAND_H__
#define __PEGA_TEXTO_DEBUGGER_COMMAND_H__

#include <pega-texto.h>

/**
 * Command numerical codes.
 */
typedef enum {
	PTDB_INVALID = -1,
	PTDB_HELP = 0,
	PTDB_STEP,
	PTDB_NEXT,
	PTDB_CONTINUE,
	PTDB_FINISH,
	PTDB_BACKTRACE,
	PTDB_LIST,
	PTDB_PRINT,
	PTDB_RULES,
	PTDB_BREAK_EXPR,
	PTDB_BREAK_RULE,
	PTDB_BREAK_INPUT_POS,
	PTDB_BREAK_END,
	PTDB_BREAK_ERROR,
	PTDB_MEMORY,

	PTDB_OPCODE_MAX,
} ptdb_opcode;

/**
 * A command to be run by the Debugger.
 */
typedef struct {
	/// Command code from #ptdb_opcode.
	int opcode;
	/// Input data for some commands.
	union {
		char *str;     ///< Rule names, error message.
		pt_expr *expr; ///< Pega-texto Expression.
		int pos;       ///< Position at input string.
		int opcode;    ///< Opcode of reference command for help.
		int memory_target_grammar;  ///< Memory command is targeting grammar?
	} data;
} ptdb_command;

/**
 * Should matching be resumed after running command?
 */
int ptdb_command_resume_matching(ptdb_command cmd);

typedef struct ptdb_t ptdb_t;
void ptdb_run_command(ptdb_t *debugger, ptdb_command cmd, const pt_match_state_stack *s, const pt_match_action_stack *a, const char *str);

#endif

