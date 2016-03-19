CXXFLAGS := -std=c++14 -Ilibs/C/h

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

tinsel_doc: tokenizer_base
	bash ctinsel.sh src/TinselDoc/TinselDoc.tnl build/TinselDoc.ctobj tinsel_doc
	bash ctinsel.sh src/TinselDoc/OutputGenerator.tnl build/OutputGenerator.ctobj
	bash ctinsel.sh src/TinselDoc/HTMLOutputGenerator.tnl build/HTMLOutputGenerator.ctobj
	cat build/TokenType.ctobj build/Token.ctobj build/TokenStream.ctobj build/OutputGenerator.ctobj build/HTMLOutputGenerator.ctobj build/TinselDoc.ctobj > build/tinsel_doc.cpp
	g++ $(CXXFLAGS) build/tinsel_doc.cpp -o build/tinsel_doc

docs: tinsel_doc
	mkdir -p docs
	echo "cd libs/base && for i in *.tnl; do ../../build/tinsel_doc --infile \$$$ i --outfile ../../docs/\$$$ i; done " | bash
 
