CXXFLAGS := -std=c++14 -Ilibs/C/h -g 	

.PHONY: libs tokenizer

libs: 
	bash -e build.sh

tokenizer_base:
	mkdir -p build
	bash ctinsel.sh src/Tokenizer/TokenType.tnl build/TokenType.ctobj
	bash ctinsel.sh src/Tokenizer/Token.tnl build/Token.ctobj
	bash ctinsel.sh src/Tokenizer/TokenStream.tnl build/TokenStream.ctobj

tokenizer_test: tokenizer_base
	bash ctinsel.sh src/Tokenizer/TokenStandalone.tnl build/TokenStandalone.ctobj
	bash ctinsel.sh src/Tokenizer/FileCompare.tnl build/FileCompare.ctobj
	bash ctinsel.sh src/Tokenizer/TokenTest.tnl build/TokenTest.ctobj test_Tokenizer
	cat build/TokenType.ctobj build/Token.ctobj build/TokenStream.ctobj build/TokenStandalone.ctobj build/FileCompare.ctobj build/TokenTest.ctobj > build/tokenizer_test.cpp
	g++ $(CXXFLAGS) build/tokenizer_test.cpp -o build/tokenizer_test
	
tokenizer_standalone: tokenizer_base
	bash ctinsel.sh src/Tokenizer/TokenStandalone.tnl build/TokenStandalone.ctobj standalone_tokenizer
	cat build/TokenType.ctobj build/Token.ctobj build/TokenStream.ctobj build/TokenStandalone.ctobj > build/tokenizer_standalone.cpp
	g++ $(CXXFLAGS) build/tokenizer_standalone.cpp -o build/tokenizer_standalone

tokenizer: tokenizer_base
	cat build/TokenType.ctobj build/Token.ctobj build/TokenStream.ctobj > build/Tokenizer.ctobj

tinsel_doc: tokenizer
	bash ctinsel.sh src/TinselDoc/TinselDoc.tnl build/TinselDoc.ctobj tinsel_doc
	bash ctinsel.sh src/TinselDoc/OutputGenerator.tnl build/OutputGenerator.ctobj
	bash ctinsel.sh src/TinselDoc/HTMLOutputGenerator.tnl build/HTMLOutputGenerator.ctobj
	cat build/Tokenizer.ctobj build/OutputGenerator.ctobj build/HTMLOutputGenerator.ctobj build/TinselDoc.ctobj > build/tinsel_doc.cpp
	g++ $(CXXFLAGS) build/tinsel_doc.cpp -o build/tinsel_doc

syntaxer_standalone: tokenizer
	bash ctinsel.sh src/Syntaxer/Syntaxer.tnl build/Syntaxer.ctobj
	bash ctinsel.sh src/Syntaxer/Variable.tnl build/Variable.ctobj
	bash ctinsel.sh src/Syntaxer/SyntaxFileWriter.tnl build/SyntaxFileWriter.ctobj
	bash ctinsel.sh src/Syntaxer/Expression.tnl build/Expression.ctobj
	bash ctinsel.sh src/Syntaxer/SyntaxerStandalone.tnl build/SyntaxerStandalone.ctobj standalone_syntaxer
	cat build/Tokenizer.ctobj build/Variable.ctobj build/SyntaxFileWriter.ctobj build/Expression.ctobj build/Syntaxer.ctobj build/SyntaxerStandalone.ctobj > build/syntaxer_standalone.cpp
	g++ $(CXXFLAGS) build/syntaxer_standalone.cpp -o build/syntaxer_standalone

docs: tinsel_doc
	mkdir -p docs
	echo "cd libs/base && for i in *.tnl; do ../../build/tinsel_doc --infile \$$$ i --outfile ../../docs/\$$$ i.htm; done " | bash
	mkdir -p docs/Tokenizer
	echo "cd src/Tokenizer && for i in *.tnl; do ../../build/tinsel_doc --infile \$$$ i --outfile ../../docs/Tokenizer/\$$$ i.htm; done " | bash
	mkdir -p docs/Syntaxer
	echo "cd src/Syntaxer && for i in *.tnl; do ../../build/tinsel_doc --infile \$$$ i --outfile ../../docs/Syntaxer/\$$$ i.htm; done " | bash
 
