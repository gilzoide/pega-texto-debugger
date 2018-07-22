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

#include "pega-texto-debugger.h"
#include "shell.h"

int main(int argc, char **argv) {
	pt_grammar *grammar = ptdb_create_command_grammar();
	pt_match_options opts = ptdb_wrap(grammar, NULL,
			PTDB_DEFAULT_OPTIONS | PTDB_AUTORELEASE | PTDB_SHELL_USE_HISTORY);
	pt_match_grammar(grammar, "help help", &opts);

	pt_destroy_grammar(grammar);
	return 0;
}

