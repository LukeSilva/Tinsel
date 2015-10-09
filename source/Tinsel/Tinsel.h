#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <tuple>
class DefaultCodeGenerator;
class Tinsel
{
public:
	enum VariableType{
		U8,S8,U16,S16,U32,S32,INT,STRING,UNKNOWN_TYPE
	};
	class Variable{
	public:
		Tinsel::VariableType Type;
		std::string Name;
		bool IsArray;
	};
	enum TokenTypes{
		//Variable Types
		TOKEN_U8, TOKEN_S8, TOKEN_U16, TOKEN_S16, TOKEN_U32, TOKEN_S32, TOKEN_INT, TOKEN_STRING,
		//Syntax tokens
		OSBRACKET, CSBRACKET, OBLK, CBLK, OBRACKET, CBRACKET, SEPERATOR,
		//Operations
		ASSIGN, ADDITION, SUBTRACTION, MULTIPLICATION, DIVISION, MODULO, LT, LTEQ, GT, GTEQ, EQUAL, NEQ, XOR, OR, AND, NOT, BOOLAND, BOOLOR, BOOLNOT, SHIFTL, SHIFTR,
		//Control
		FOR, WHILE, IF, ELSE, RETURN,
		//Other
		UNKNOWN, PRINTSTR, NUMBER_AS_INT, STRING_LITERAL, CALL, PRINTINT, GETINT, EXIT

	};
	class Token{
	public:
		std::string Name;
		Tinsel::TokenTypes Type;
		void ReadFromFile(std::ifstream& InputFile);
		long Value;
	};

	Tinsel();
	bool Error();
	void DefineGlobalVariableInBss(std::string Scope, Tinsel::Variable Var);
	void DefineGlobalVariableInData(std::string Scope, Tinsel::Variable Var, long Value, std::string StringValue);
	void DefineLocalVariable(std::string Scope, Tinsel::Variable Var, long Value, std::string StringValue);


	//Namespace, Variable, address
	typedef std::tuple < std::string, Tinsel::Variable, long> GlobalReference;
	//Variable, Stack Offset
	typedef std::tuple < Tinsel::Variable, int > LocalReference;

	void StartVariableGlobal(std::ifstream& Stream, Tinsel::Token Token);
	void StartVariableLocal(std::ifstream& Stream, Tinsel::Token Token);
	std::vector <GlobalReference>::iterator FindGlobalReferenceByName(std::string Name);
	std::vector <LocalReference>::iterator FindLocalReferenceByName(std::string Name);

	int LoadFile(std::string);
	bool Compile(std::string filename);
	static bool IsStringInt(const std::string& String);
	~Tinsel();
protected:
	DefaultCodeGenerator* CodeGenerator;
	int ErrorCode;
	int StackOffset;
	Tinsel::VariableType ExpressionType;

	std::vector < GlobalReference > GlobalReferences;
	std::vector <LocalReference> LocalReferences;
	bool IsVariableType(Tinsel::Token Token);
	void ResetGlobalVariableAddress(std::string Scope, long Address, long Value);
	Tinsel::VariableType GetVariableTypeFromToken(Tinsel::Token Token);
	bool Expect(std::ifstream& Stream, Tinsel::TokenTypes Type);
	bool CodeBlock(std::ifstream& InputFile, bool IsFunction, Tinsel::Variable Function);
	bool Tinsel::ParseFactor(std::ifstream& InputFile);
	Tinsel::Token Tinsel::ParseTerm(std::ifstream& InputFile);
	Tinsel::Token Tinsel::ParseLevel2(std::ifstream& InputFile);
	Tinsel::Token ParseExpression(std::ifstream& InputFile);

	void DoIf(std::ifstream& InputFile, Tinsel::Variable Function);


	void PushMainRegister(Tinsel::VariableType MainRegisterType, int Bytes);
	void PushMainRegister(Tinsel::VariableType MainRegisterType);
	void PopSecondaryRegister(Tinsel::VariableType SecondaryRegisterType, int Bytes);
	void PopSecondaryRegister(Tinsel::VariableType SecondaryRegisterType);
	void ZeroStackOffset(int StackOffset = 0);
	void Tinsel::RemoveInvalidLocals(int StackOffset);
};

#include "../CodeGenerator/DefaultCodeGenerator.h"