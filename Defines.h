#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define maxText 100000			// ������������ ����� ������
#define maxLex 100				// ������������ ����� �������
#define maxKeyword 16			// ������������ ���������� �������� ����
#define maxNumber 10			// ������������ ����� �����
typedef char type_mod[maxText]; // �����
typedef char type_lex[maxLex];  // �������

/**
* ��� ������
*/
#define typeId 1

/**
* �������� �����
*/
#define typeShort 10
#define typeLong 11
#define typeInt 12
#define typeBool 13

#define typeSwitch 14
#define typeReturn 15
#define typeMain 16
#define typeBreak 17
#define typeCase 18
#define typeDefault 19
#define typeNot 20
#define typeAnd 21
#define typeOr 22

/**
* ���������
*/
#define constInt 30
#define constBool 31

/**
* ����������� �����
*/
#define typeColon 40			// :
#define typeComma 41			// ,
#define typeSemicolon 42		// ;
#define typeLeftBracket 43		// (
#define typeRightBracket 44		// )
#define typeLeftBrace 45		// {
#define typeRightBrace 46		// }

/**
* ����� ��������
*/
#define typeEval 50				// =
#define typeUnEq 51				// !=
#define typeEq 52				// ==
#define typeMore 53				// >
#define typeMoreOrEq 54			// >=
#define typeLess 55				// <
#define typeLessOrEq 56			// <=
#define typeMul 59				// *
#define typeDiv 60				// /
#define typeMod 61				// %
#define typePlus 62				// +
#define typeMinus 63			// -

/**
* �������������
*/
#define typeEnd 100
#define typeError 404