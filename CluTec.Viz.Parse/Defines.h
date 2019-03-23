////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Parse
// file:      Defines.h
//
// summary:   Declares the defines class
//
//            Copyright (c) 2019 by Christian Perwass.
//
//            This file is part of the CluTecLib library.
//
//            The CluTecLib library is free software: you can redistribute it and / or modify
//            it under the terms of the GNU Lesser General Public License as published by
//            the Free Software Foundation, either version 3 of the License, or
//            (at your option) any later version.
//
//            The CluTecLib library is distributed in the hope that it will be useful,
//            but WITHOUT ANY WARRANTY; without even the implied warranty of
//            MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
//            GNU Lesser General Public License for more details.
//
//            You should have received a copy of the GNU Lesser General Public License
//            along with the CluTecLib library.
//            If not, see <http://www.gnu.org/licenses/>.
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

// Operator Levels
// 0 binds strongest
#define OPLEVEL_FUNCTION          2		// '[label]('

#define OPLEVEL_GRADE             3		// '?'
#define OPLEVEL_DUAL              5		// '*'
#define OPLEVEL_INVERSE           5		// '!'
#define OPLEVEL_REVERSE           5		// '~'
#define OPLEVEL_INVOLUTE          5		// '''
#define OPLEVEL_NEGATE            5		// '-'
#define OPLEVEL_MODULUS           6		// '%'
#define OPLEVEL_POWER             7		// '^^'

#define OPLEVEL_WEDGE             10	// '^' e.g. outer product
#define OPLEVEL_POP               10	// '.^' e.g. point-wise outer product
#define OPLEVEL_POINT             13	// '.' e.g. inner product
#define OPLEVEL_PIP               13	// '..' e.g. point-wise inner product
#define OPLEVEL_TIMES             15	// '*' e.g. geometric product
#define OPLEVEL_PMUL              15	// '.*' e.g. point-wise geometric product
#define OPLEVEL_DIV               15	// '/' e.g. division
#define OPLEVEL_PDIV              15	// './' e.g. point-wise division
#define OPLEVEL_JOIN              17	// '|'
#define OPLEVEL_MEET              17	// '&'
#define OPLEVEL_MINUS             19	// '-'
#define OPLEVEL_PMINUS            19	// '.-'
#define OPLEVEL_PLUS              20	// '+'
#define OPLEVEL_PPLUS             20	// '.+'

#define OPLEVEL_STEPLIST          30	// '~'

#define OPLEVEL_EQUAL             50	// '=='
#define OPLEVEL_NOT               51	// '!!'
#define OPLEVEL_AND               52	// '&&'
#define OPLEVEL_PAND              52	// '.&&'
#define OPLEVEL_OR                53	// '||'
#define OPLEVEL_POR               53	// '.||'

#define OPLEVEL_LSHIFT            70	// '<<'

#define OPLEVEL_ASSIGN            100	// '='
#define OPLEVEL_OGLDRAW           200	// ':'
#define OPLEVEL_PRINT             200	// '?'

// Error IDs
#define CPB_NOERROR               0
#define CPB_UNKNOWN               1
#define CPB_OUTOFMEMORY           2
#define CPB_INVALIDSYNTAX         3
#define CPB_MISSINGORB            4		// Missing Open Round Bracket
#define CPB_MISSINGCRB            5		// Missing Close Round Bracket
#define CPB_UNKNOWNSYM            6		// Unkown symbol
#define CPB_UNEXPECTEDSYM         7		// Unexpected Symbol
#define CPB_UNEXPSYMCOMMA         8		// Unexpected Symbol Comma
#define CPB_NONUNARYOP            9		// Non-Unary Operator
#define CPB_NONBINARYOP           10	// Non-Binary Operator
#define CPB_MISSINGOCB            11	// Missing Open Curly Bracket
#define CPB_MISSINGCCB            12	// Missing Close Curly Bracket
#define CPB_MISSING_IFCOND        13	// Missing if condition
#define CPB_MISSING_THEN          14	// Missing THEN
#define CPB_MISSING_IFTRUE        15	// Missing IF TRUE branch
#define CPB_MISSING_IFFALSE       16	// Missing IF FALSE branch
#define CPB_NO_INCLUDEFILE        17	// Include file could not be found
#define CPB_MISSINGOSB            18	// Missing Open Square Bracket
#define CPB_MISSINGCSB            19	// Missing Close Square Bracket
#define CPB_UNEXPORB              20	// Unexpected Open Round Bracket
#define CPB_RECURSIVE_INCLUDEFILE 21	// Recursive includefile

// Messages returned for internal parsing operations
#define CPB_WEAKEROP              -1
#define CPB_COMMA                 -2
#define CPB_CLOSEDRB              -3	// Closed Round Bracket
#define CPB_EOL                   -4	// End of Line
#define CPB_CLOSEDCB              -5	// Closed Curly Bracket
#define CPB_THEN                  -6	// Start of TRUE branch
#define CPB_ELSE                  -7	// Start of FALSE branch
#define CPB_REPARSE               -8	// Parse current line again
#define CPB_CLOSEDSB              -9	// Closed Square Bracket
