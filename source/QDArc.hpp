/*
*	Ported and translated from Quadruple D's QDArc.pas file.
*	http://karen.saiin.net/~hayase/dddd/download.html
*/
#pragma once

#include <stdio.h>

//Exctract from archive
FILE* ExtractFromQDAFile(char const* FileName, char const* ID);
FILE* ExtractFromQDAStream(FILE* Source, char const* ID);