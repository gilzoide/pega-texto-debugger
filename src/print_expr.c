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

#include "print_expr.h"

#include <ctype.h>

const char *reset = "\033[0m";
const char *black = "\033[0;22;30m";
const char *red = "\033[0;22;31m";
const char *green = "\033[0;22;32m";
const char *brown = "\033[0;22;33m";
const char *blue = "\033[0;22;34m";
const char *magenta = "\033[0;22;35m";
const char *cyan = "\033[0;22;36m";
const char *lightgray = "\033[0;22;37m";
const char *gray = "\033[0;1;30m";
const char *brightred = "\033[0;1;31m";
const char *brightgreen = "\033[0;1;32m";
const char *yellow = "\033[0;1;33m";
const char *brightblue = "\033[0;1;34m";
const char *brightmagenta = "\033[0;1;35m";
const char *brightcyan = "\033[0;1;36m";
const char *white = "\033[0;1;37m";
const char *ptdb_color_for_paren_level(int level) {
	const char *colors[] = {
		red,
		green,
		brown,
		blue,
		magenta,
		cyan,
		black,
	};

	return colors[level % 7];
}

void ptdb_print_char_class(Replxx *replxx, pt_character_class_function f) {
		if(f == isalnum)       replxx_print(replxx, "\\w");
		else if(f == isalpha)  replxx_print(replxx, "\\a");
		else if(f == iscntrl)  replxx_print(replxx, "\\c");
		else if(f == isdigit)  replxx_print(replxx, "\\d");
		else if(f == isgraph)  replxx_print(replxx, "\\g");
		else if(f == islower)  replxx_print(replxx, "\\l");
		else if(f == ispunct)  replxx_print(replxx, "\\p");
		else if(f == isspace)  replxx_print(replxx, "\\s");
		else if(f == isupper)  replxx_print(replxx, "\\u");
		else if(f == isxdigit) replxx_print(replxx, "\\x");
		else                   replxx_print(replxx, "C%p", f);
}

void ptdb_print_expr_rec(Replxx *replxx, pt_expr *expr, int *paren_count) {
	if(expr) {
		int i;
		const char *paren_color;
		switch(expr->op) {
			case PT_LITERAL:
				replxx_print(replxx, "'%s'", expr->data.characters);
				break;
			case PT_CASE_INSENSITIVE:
				replxx_print(replxx, "I'%s'", expr->data.characters);
				break;
			case PT_CHARACTER_CLASS:
				ptdb_print_char_class(replxx, expr->data.test_character_class);
				break;
			case PT_SET:
				replxx_print(replxx, "[%s]", expr->data.characters);
				break;
			case PT_RANGE:
				replxx_print(replxx, "[%c-%c]", expr->data.characters[0], expr->data.characters[1]);
				break;
			case PT_ANY:
				replxx_print(replxx, ".");
				break;
			case PT_NON_TERMINAL:
				if(expr->data.characters) replxx_print(replxx, "%s", expr->data.characters);
				else replxx_print(replxx, "<%d>", expr->N);
				break;
			case PT_QUANTIFIER:
				ptdb_print_expr_rec(replxx, expr->data.e, paren_count);
				switch(expr->N) {
					case 0:
						replxx_print(replxx, "*");
						break;
					case 1:
						replxx_print(replxx, "+");
						break;
					case -1:
						replxx_print(replxx, "?");
						break;
					default:
						replxx_print(replxx, "{%d}", expr->N);
						break;
				}
				break;
			case PT_AND:
				replxx_print(replxx, "&");
				ptdb_print_expr_rec(replxx, expr->data.e, paren_count);
				break;
			case PT_NOT:
				replxx_print(replxx, "!");
				ptdb_print_expr_rec(replxx, expr->data.e, paren_count);
				break;
			case PT_SEQUENCE:
				paren_color = ptdb_color_for_paren_level(*paren_count);
				*paren_count = *paren_count + 1;
				replxx_print(replxx, "%s(%s", paren_color, reset);
				for(i = 0; i < expr->N; i++) {
					if(i) replxx_print(replxx, " ");
					ptdb_print_expr_rec(replxx, expr->data.es[i], paren_count);
				}
				replxx_print(replxx, "%s)%s", paren_color, reset);
				break;
			case PT_CHOICE:
				paren_color = ptdb_color_for_paren_level(*paren_count);
				*paren_count = *paren_count + 1;
				replxx_print(replxx, "%s(%s", paren_color, reset);
				for(i = 0; i < expr->N; i++) {
					if(i) replxx_print(replxx, " %s/%s ", paren_color, reset);
					ptdb_print_expr_rec(replxx, expr->data.es[i], paren_count);
				}
				replxx_print(replxx, "%s)%s", paren_color, reset);
				break;
			case PT_CUSTOM_MATCHER:
				replxx_print(replxx, "F%p", expr->data.matcher);
				break;
			case PT_ERROR:
				replxx_print(replxx, "#%d", expr->N);
				break;
		}
	}
}

void ptdb_print_expr(Replxx *replxx, pt_expr *expr) {
	int paren_level = 0;
	ptdb_print_expr_rec(replxx, expr, &paren_level);
}

void ptdb_print_grammar(Replxx *replxx, pt_grammar *grammar) {
	int i;
	for(i = 0; i < grammar->N; i++) {
		replxx_print(replxx, "%s <- ", grammar->names[i]);
		ptdb_print_expr(replxx, grammar->es[i]);
		replxx_print(replxx, "\n");
	}
}

