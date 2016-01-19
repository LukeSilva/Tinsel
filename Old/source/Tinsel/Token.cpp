#include "Tinsel.h"
#include "../CodeGenerator/DefaultCodeGenerator.h"
#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <iterator>
#include <vector>

const std::map < std::string, Tinsel::TokenTypes > TokenMap = {
	{ "u8", Tinsel::TokenTypes::TOKEN_U8 },
	{ "s8", Tinsel::TokenTypes::TOKEN_S8 },
	{ "u16", Tinsel::TokenTypes::TOKEN_U16 },
	{ "s16", Tinsel::TokenTypes::TOKEN_S16 },
	{ "u32", Tinsel::TokenTypes::TOKEN_U32 },
	{ "s32", Tinsel::TokenTypes::TOKEN_S32 },
	{ "string", Tinsel::TokenTypes::TOKEN_STRING },
	{ "int", Tinsel::TokenTypes::TOKEN_INT },
	{ "(", Tinsel::TokenTypes::OBRACKET },
	{ ")", Tinsel::TokenTypes::CBRACKET },
	{ "[", Tinsel::TokenTypes::OSBRACKET },
	{ "]", Tinsel::TokenTypes::CSBRACKET },
	{ "{", Tinsel::TokenTypes::OBLK },
	{ "}", Tinsel::TokenTypes::CBLK },
	{ ";", Tinsel::TokenTypes::SEPERATOR },
	{ "=", Tinsel::TokenTypes::ASSIGN },
	{ "+", Tinsel::TokenTypes::ADDITION },
	{ "-", Tinsel::TokenTypes::SUBTRACTION },
	{ "*", Tinsel::TokenTypes::MULTIPLICATION },
	{ "/", Tinsel::TokenTypes::DIVISION },
	{ "%", Tinsel::TokenTypes::MODULO },
	{ "<", Tinsel::TokenTypes::LT },
	{ "<=", Tinsel::TokenTypes::LTEQ },
	{ ">", Tinsel::TokenTypes::GT },
	{ ">=", Tinsel::TokenTypes::GTEQ },
	{ "==", Tinsel::TokenTypes::EQUAL },
	{ "!=", Tinsel::TokenTypes::NEQ },
	{ "|", Tinsel::TokenTypes::OR },
	{ "^", Tinsel::TokenTypes::XOR },
	{ "&", Tinsel::TokenTypes::AND },
	{ "~", Tinsel::TokenTypes::NOT },
	{ "||", Tinsel::TokenTypes::BOOLOR },
	{ "&&", Tinsel::TokenTypes::BOOLAND },
	{ "!", Tinsel::TokenTypes::BOOLNOT },
	{ "<<", Tinsel::TokenTypes::SHIFTL },
	{ ">>", Tinsel::TokenTypes::SHIFTR },
	{ "if", Tinsel::TokenTypes::IF },
	{ "else", Tinsel::TokenTypes::ELSE },
	{ "for", Tinsel::TokenTypes::FOR },
	{ "while", Tinsel::TokenTypes::WHILE },
	{ "return", Tinsel::TokenTypes::RETURN },
	{ "PrintString", Tinsel::TokenTypes::PRINTSTR },
	{ "PrintInt", Tinsel::TokenTypes::PRINTINT },
	{ "GetInt", Tinsel::TokenTypes::GETINT },
	{ "Exit", Tinsel::TokenTypes::EXIT },
	{ "@", Tinsel::TokenTypes::CALL },
	{ "call", Tinsel::TokenTypes::CALL }
};



void Tinsel::Token::ReadFromFile(std::ifstream& File)
{
	while (iswspace(File.peek())) File.get();
	//Here there are two circumstances, Either a string literal, or you can get the next word and read it as a token
	if (File.peek() == '"')
	{
		//Remove the '"' from the string
		File.get();
		//Now create a std::string containing the string literal.
		std::string StringLiteral;
		std::getline(File, StringLiteral, '"');
		this->Type = Tinsel::TokenTypes::STRING_LITERAL;
		this->Name = StringLiteral;
	}
	else
	{
		std::string str;
		File >> str;
		this->Type = Tinsel::TokenTypes::UNKNOWN;
		this->Name = str;
		auto it = TokenMap.find(str);
		if (it != TokenMap.end())
		{
			this->Type = it->second;
		}
		else if (Tinsel::IsStringInt(str))
		{
			this->Type = Tinsel::TokenTypes::NUMBER_AS_INT;
			this->Value = strtol(str.c_str(), NULL, 0);
		}
	}
}
