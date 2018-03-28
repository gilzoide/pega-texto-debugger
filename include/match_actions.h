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

/** @file match_actions.h
 */

#ifndef __PEGA_TEXTO_DEBUGGER_MATCH_ACTIONS_H__
#define __PEGA_TEXTO_DEBUGGER_MATCH_ACTIONS_H__

#include <pega-texto.h>

/**
 * Function called on each iteration of the match algorithm.
 *
 * This opens the REPL the first time it's called and whenever needed (if
 * stepping, on breakpoints, etc...).
 */
void ptdb_each_iteration(const pt_match_state_stack *s, const pt_match_action_stack *a, const char *str, void *data);

/**
 * Function called when match ends.
 *
 * This resets the Debugger state and frees it, if #PTDB_AUTORELEASE option is
 * enabled.
 */
void ptdb_on_end(const pt_match_state_stack *s, const pt_match_action_stack *a, const char *str, pt_match_result result, void *data);

/**
 * Function called when a match succeeds.
 */
void ptdb_each_success(const pt_match_state_stack *s, const pt_match_action_stack *a, const char *str, size_t begin, size_t end, void *data);

/**
 * Function called when a match fails.
 */
void ptdb_each_fail(const pt_match_state_stack *s, const pt_match_action_stack *a, const char *str, void *data);

/**
 * Function called when an Error Expression is matched.
 *
 * Breakpoints may be set when matching Error.
 */
void ptdb_on_error(const char *str, size_t where, int code, void *data);

#endif

