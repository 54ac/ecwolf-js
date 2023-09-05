#
# decorate.py
#
#---------------------------------------------------------------------------
# Copyright 2023 Braden Obrzut
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
#
# 1. Redistributions of source code must retain the above copyright
#    notice, this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright
#    notice, this list of conditions and the following disclaimer in the
#    documentation and/or other materials provided with the distribution.
# 3. The name of the author may not be used to endorse or promote products
#    derived from this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
# IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
# OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
# IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
# INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
# NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
# DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
# THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
# THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#---------------------------------------------------------------------------
#
# Borrows a few expressions from the c-family lexer built into pygments which
# is also BSD licensed. See pygments' c_cpp.py.
#

import pygments.lexer
from pygments.token import *

__all__ = ['DecorateLexer']

class DecorateLexer(pygments.lexer.RegexLexer):
	name = 'DECORATE'
	aliases = ['decorate']
	filenames = ['*.txt']

	# Hexadecimal part in an hexadecimal integer/floating-point literal.
	# This includes decimal separators matching.
	_hexpart = r'[0-9a-fA-F](\'?[0-9a-fA-F])*'
	# Decimal part in an decimal integer/floating-point literal.
	# This includes decimal separators matching.
	_decpart = r'\d(\'?\d)*'

	tokens = {
		'root': [
			(r'\s+', Whitespace),
			pygments.lexer.include('comments'),
			(r'(actor)(\s*)(\w+)(\s*)(?:(:)(\s*)(\w+)(\s*))?(?:(replaces)(\s*)(\w+)(\s*))?(?:(\d+)(\s*))?({)',
				pygments.lexer.bygroups(Keyword, Whitespace, Name.Class, Whitespace, Punctuation, Whitespace, Name.Class, Whitespace, Keyword, Whitespace, Name.Class, Whitespace, Number, Whitespace, Punctuation), 'actor'),
			(r'.+', Text)
		],
		'actor': [
			(r'\s+', Whitespace),
			pygments.lexer.include('comments'),
			(r'"', String, 'string'),
			pygments.lexer.include('number'),
			(r',', Punctuation),
			(r'\(', Punctuation, 'expression'),
			(r'(action)(\s*)(native)(\s*)(\w+)(\()(.*)(\))(;)', pygments.lexer.bygroups(Keyword, Whitespace, Keyword, Whitespace, Name.Function, Punctuation, Text, Punctuation, Punctuation)),
			(r'(states)(\s*)({)', pygments.lexer.bygroups(Keyword, Whitespace, Punctuation), 'states'),
			(r'\w+(?:\.\w+)?', Name.Property),
			(r'([+\-])(\w+(?:\.\w+)?)', pygments.lexer.bygroups(Punctuation, Name.Attribute)),
			(r'}', Punctuation, '#pop'),
			(r'.+', Text)
		],
		'comments': [
			(r'(?s)/\*.*?\*/', Comment),
			(r'//.*?\n', Comment),
		],
		'expression': [
			(r'\s+', Whitespace),
			pygments.lexer.include('comments'),
			(r'\(', Punctuation, '#push'),
			(r'\)', Punctuation, '#pop'),
			(r',', Punctuation),
			(r'"', String, 'string'),
			pygments.lexer.include('number'),
			(r'\w+', Name.Builtin),
		],
		'stateduration': [
			(r'\s+', Whitespace),
			pygments.lexer.include('comments'),
			pygments.lexer.include('number'),
			(r'(random)(\()', pygments.lexer.bygroups(Name.Builtin, Punctuation), 'expression'),
			pygments.lexer.default('#pop'),
		],
		'statefunctions': [
			(r'\s+', Whitespace),
			pygments.lexer.include('comments'),
			(r'NOP', Keyword),
			(r'(\w{5,})(\s*)(\()', pygments.lexer.bygroups(Name.Function, Whitespace, Punctuation), 'expression'),
			(r'\w{5,}', Name.Function),
			pygments.lexer.default('#pop'),
		],
		'stateproperties': [
			(r'\s+', Whitespace),
			pygments.lexer.include('comments'),
			(r'bright|canraise|fast|nodelay|slow', Keyword),
			(r'(light|offset)(\s*)(\()', pygments.lexer.bygroups(Keyword, Whitespace, Punctuation), 'expression'),
			pygments.lexer.default('#pop'),
		],
		'states': [
			(r'\s+', Whitespace),
			pygments.lexer.include('comments'),
			(r'(\w+)(:)', pygments.lexer.bygroups(Name.Label, Punctuation)),
			(r'loop|fail|wait|stop', Keyword),
			(r'(goto)(\s*)(?:(\w+)(::))?(\w+)(?:(\s*)(\+)(\s*)(\d+))?', pygments.lexer.bygroups(Keyword, Whitespace, Name.Label, Punctuation, Name.Label, Whitespace, Punctuation, Whitespace, Number.Decimal)),
			(r'(\w{4}|"\w{4}")(\s*)(\w+|"\w+")', pygments.lexer.bygroups(Literal, Whitespace, Literal), ('statefunctions', 'stateproperties', 'stateduration')),
			(r'}', Punctuation, '#pop'),
			(r'.+', Text)
		],
		'string': [
			(r'"', String, '#pop'),
			(r'\\([\\abfnrtv"\']|x[a-fA-F0-9]{2,4}|'
			 r'u[a-fA-F0-9]{4}|U[a-fA-F0-9]{8}|[0-7]{1,3})', String.Escape),
			(r'[^\\"\n]+', String),  # all other characters
			(r'\\\n', String),  # line continuation
			(r'\\', String),  # stray backslash
		],
		'number': [
			(r'0[xX](' + _hexpart + r'\.' + _hexpart + r'|\.' + _hexpart +
			 r'|' + _hexpart + r')[pP][+-]?' + _hexpart, Number.Float),
			(r'(-)?(' + _decpart + r'\.' + _decpart + r'|\.' + _decpart + r'|' +
			 _decpart + r')[eE][+-]?' + _decpart, Number.Float),
			(r'(-)?((' + _decpart + r'\.(' + _decpart + r')?|\.' +
			 _decpart + r'))|(' + _decpart + ')', Number.Float),
			(r'(-)?0[xX]' + _hexpart, Number.Hex),
			(r'(-)?0[bB][01](\'?[01])*', Number.Bin),
			(r'(-)?0(\'?[0-7])+', Number.Oct),
			(r'(-)?' + _decpart, Number.Integer), 
		],
	}
