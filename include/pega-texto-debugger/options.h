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

/** @file options.h
 */

#ifndef __PEGA_TEXTO_DEBUGGER_OPTIONS_H__
#define __PEGA_TEXTO_DEBUGGER_OPTIONS_H__

/**
 * Options for the debugger.
 */
typedef enum {
	/**
	 * Turn this on if you want the wrapped debugger to be automatically
	 * destroyed on the end of the `pt_match_grammar` call.
	 */
	PTDB_AUTORELEASE = 1 << 0,
} ptdb_options;

#endif

