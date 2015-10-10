# Tinsel - Programming Language


##Command Line:

The Tinsel Project consists of two executables, tinsel and tinsel-vm

###tinsel: The tinsel compiler

The tinsel compiler will compile a tinsel source code into a vm code file.
Usage
	tinsel <source-file> <output>
Eg:
	tinsel Examples\Factorial.txt Factorial.tnl


###tinsel-vm: The tinsel Virtual Machine

The tinsel virtual machine will execute a vm code file
Usage:
	tinsel-vm <vm-code-file>
Eg:
	tinsel-vm Factorial.tnl



##Reading of source files:

Tinsel reads through source files by going through tokens. Every token must be seperated by a space. All defined tokens are as follows:
```
"u8" "s8" "u16" "s16" "u32" "s32" "string" "int" 
"(" ")" "[" "]" "{" "}" ";" 
"=" "+" "-" "*" "/" "%"
"<" "<=" ">" ">=" "==" "!="
"|" "^" "&" "~"
"||" "&&" "!"
"<<" ">>"
"if" "else" "for" "while" "return"
"PrintString" "PrintInt" "GetInt" "Exit"
"@" "call"
```
All tokens are reserved, and variables may not have the same name as an existing token.



##Variables:

Tinsel supports the following variable types
```
u8 - Unsigned 8 bit variable
s8 - Signed 8 bit variable
u16 - Unsigned 16 bit variable
s16 - Signed 16 bit variable
u32 - Unsigned 32 bit variable
s32 - Signed 32 bit variable
int - A variable that is at least 8 bits long, and is signed.
	This type will be the fastest variable type to run on a specific platform
string - An array of ASCII characters.
	This variable can only be defined globally, and can not be used in expressions.
```

A main tinsel code file consists of a number of global variables, these variables can also be functions.
A variable may be defined as follows

    <variable-type> <variable-name> [ = <default-value> ] ;

Eg:
```
int A = 5 ;
u8 Hex = 0xFF ;
u16 C ;
```

The default value for a variable may be given in decimal without a prefix, or in hexadecimal with the 0x prefix.
NOTE: the default value is _ALWAYS_ a literal, and never an expression.



##Functions:

Functions are special types of global variables; they consist of a code block, and return a value, which can be assigned to variables.

Internally, the address of a function is stored in the data page of an exectuable as a variable of the name for the function. This is to allow a single pass through the code file. 

If a function is not defined when referenced, a blank variable is created, with a (random) jump point. Then when the function is defined, the address of the function in the data page is updated.

Functions may be defined as follows.

    <function-type> <function-name> ( )

At the moment, functions do not accept argument lists, and all potential arguments must be passed through the use of global variables.

Immediately after a function definition, a code block must appear.

At the moment, tinsel will begin execution by running the first function that is defined. 

Traditionally in other langauges the compiler will create an machine specific function which sets up the environment, and calls main. If you would like this functionality in tinsel, add the following function to the top of your source code file.

```
int CRT0 ( )
{
 call main ;
 Exit ;
}
```


##Code Blocks:

A code block in tinsel houses all executable code. A code block begins with an open curly bracket, and ends with a close curly bracket.
Eg:
```
{
}
```

A code block may only occur after a function definition, or an if statement. At the moment, in both these conditions, a code block _MUST_ appear.

A code block may house the following executable statements:
```
Local variable definitions. These are defined exactly like global variables, but may only be 
	referenced in the code block defined, and in code blocks within the code block defined.
	<variable-type> <variable-name> [ = <default-value> ] ;
A return statement. These only take a single variable, or integer literal.
	return <variable-name / integer> ;
An Exit statement. This ends the program immedietly, without any errors
	Exit ;
A PrintString statement. This statment will print a global string variable to stdin
	PrintString <variable-name> ;
A PrintInt statement. This statement will print the result of an expression
	PrintInt <expression> ;
An assignment. This statement will assign a variable to the result of an expression.
	<variable-name> = <expression> ;
A call statement. This statement will call a function, ignoring the return value.	
	call <function-name> ;
If statement. This will evaluate an expression, and execute another code block if the expression
	does not evaluate to 0 / false.
	if ( <expression> ) <code-block>
```
Tinsel does not support while loops or else and else if statements.


	
##Expressions:

Tinsel houses all calculations in expressions. There are a variety of operators that can be used in expressions. 
Every operator has a certain preceedence level, of which there are 4 possible levels. These levels allow expressions to generally follow the rules of traditional BODMAS.
The currently implemented operators, by preceedence level are as follows.

```
Level1:
	<  = Less than, returns true if lval is lower than rval.
	>  = Greater than, returns true if lval is greater than rval.
Level2:
	+  = Addition, returns the result of the addition of lval and rval.
	-  = Subtraction, returns the result of the subtraction of lval from rval.
	|  = Bitwise Or, returns the result of a bitwise or operation between lval and rval.
	^  = Bitwise Xor, returns the result of a bitwise xor operation between lval and rval.
	|| = Logical Or, returns the result of a logical or operation between lval and rval.
Level3:
	*  = Multiplication, returns the result of a multiplication operation between lval and rval.
	/  = Division, returns the result of a division operation of lval by rval.
	&  = Bitwise And, retruns the result of a bitwise and operation between lval and rval.
	&& = Logical And, returns the result of a logical and operation between lval and rval.
Level4:
	~  = Bitwise Not, returns the bitwise inverse of a number.
	!  = Logical Not, returns the logical inverse of a number.
	GetInt = returns the value of an integer from stdin.
	call <function-name> = calls a function, and returns the result of that function.
	( <expression> ) = Allows for parentheses.
```
	
There are a number of operators that are currently unimplemented in tinsel. They, and potential work arounds, are as follows.
```
%  = Modulo, no workaround, other than a (complicated) bitwise calculation function, 
	which is out of the scope of this document.
== = Equal to, One can use ! ( a ^ b ) to workaround this.
!= = Not equal to, one can use ( a ^ b ) to workaround this.
<= = Less than or equal to, one can use ( ( a < b ) || ! ( a ^ b ) ) to workaround this
>= = Greater than or equal to, one can use ( ( a > b ) || ! ( a ^ b ) ) to workaround this
<< = Bitwise shift left, one can use multiplication / division to work around this
>> = Bitwise shift right, one can use multiplication / division to work around this.
```	
Due to the few number of precedence levels of operators in tinsel, excess parentheses may be required in locations that are unfamiliar to users of other programming languages.



#Credits:

Main Developers: Luke Silva ( CoolAs )
