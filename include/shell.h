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

/** @file shell.h
 * Functionalities related to the debugger shell.
 */

#ifndef __PEGA_TEXTO_DEBUGGER_SHELL_H__
#define __PEGA_TEXTO_DEBUGGER_SHELL_H__

#include <pega-texto.h>
#include <replxx.h>

#include "command.h"

typedef struct ptdb_t ptdb_t;

/**
 * Shell data.
 */
typedef struct {
	/// The Grammar that matches the debugger commands.
	pt_grammar *command_grammar;
	/// Replxx structure for reading the input.
	Replxx *replxx;
} ptdb_shell;

/**
 * Initialize the shell.
 *
 * @return Non-zero if shell was properly initialized, zero otherwise.
 */
int ptdb_shell_init(ptdb_shell *shell);
/**
 * Destroy the shell, cleaning the used resources.
 */
void ptdb_shell_destroy(ptdb_shell *shell);

/**
 * Create a Grammar that parses the debugger commands.
 */
pt_grammar *ptdb_create_command_grammar();

/**
 * Read a command from string.
 */
ptdb_command ptdb_read_command(ptdb_shell *shell, const char *str);

/**
 * Show the prompt and wait for user 
 */
void ptdb_prompt_shell(ptdb_t *debugger);

#endif

