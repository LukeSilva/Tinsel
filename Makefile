SOURCE := main.o 
TINSEL_SOURCE := Boilerplate.o CodeBlock.o Conditionals.o Expression.o Globals.o Locals.o Tinsel.o Token.o 
CODEGEN_SOURCE := VMCodeGenerator.o
OBJS= $(SOURCE:%.o=source/%.o) $(TINSEL_SOURCE:%.o=source/Tinsel/%.o) $(CODEGEN_SOURCE:%.o=source/CodeGenerator/%.o)

CFLAGS := -std=gnu++11
CXX= g++ $(CFLAGS)



# link
tinsel: $(OBJS)
	$(CXX) $(OBJS) -o tinsel

# pull in dependency info for *existing* .o files
-include $(OBJS:.o=.d)

%.o: %.c
	$(CXX) -c $*.c -o $*.o
	$(CXX) -MM $*.c > $*.d
	@mv -f $*.d $*.d.tmp
	@sed -e 's|.*:|$*.o:|' < $*.d.tmp > $*.d
	@sed -e 's/.*://' -e 's/\\$$//' < $*.d.tmp | fmt -1 | \
	  sed -e 's/^ *//' -e 's/$$/:/' >> $*.d
	@rm -f $*.d.tmp

clean:
	rm -f tinsel
	rm -f source/*.o
	rm -f source/CodeGenerator/*.o
	rm -f source/Tinsel/*.o
