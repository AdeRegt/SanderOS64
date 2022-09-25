INCLUDES= -I ./ -I ./helpers -I ../../../programsapi/include
OBJECTS= ./build/misc.o ./build/lexer.o  ./build/lex_process.o ./build/token.o ./build/expressionable.o ./build/parser.o ./build/validator.o ./build/symresolver.o ./build/scope.o ./build/resolver.o ./build/rdefault.o ./build/helper.o ./build/codegen.o ./build/helpers/vector.o ./build/helpers/buffer.o ./build/helpers/hashmap.o ./build/compiler.o ./build/cprocess.o ./build/preprocessor/preprocessor.o ./build/preprocessor/native.o ./build/array.o ./build/node.o ./build/preprocessor/static-includes.o ./build/preprocessor/static-includes/stddef.o ./build/preprocessor/static-includes/stdarg.o  ./build/fixup.o ./build/native.o ./build/stackframe.o
GCCTHING=${SANDEROS_CROSS_LOCATION}/x86_64-elf-gcc
all: ${OBJECTS}
	${GCCTHING} main.c ${INCLUDES} -o main ${OBJECTS} -g

./build/misc.o: ./misc.c
	${GCCTHING} misc.c ${INCLUDES} -o ./build/misc.o -g -c

./build/preprocessor/preprocessor.o: ./preprocessor/preprocessor.c
	${GCCTHING} ./preprocessor/preprocessor.c ${INCLUDES} -o ./build/preprocessor/preprocessor.o -g -c

./build/preprocessor/native.o: ./preprocessor/native.c
	${GCCTHING} ./preprocessor/native.c ${INCLUDES} -o ./build/preprocessor/native.o -g -c

./build/preprocessor/static-includes.o: ./preprocessor/static-includes.c
	${GCCTHING} ./preprocessor/static-includes.c ${INCLUDES} -o ./build/preprocessor/static-includes.o -g -c

./build/preprocessor/static-includes/stddef.o: ./preprocessor/static-includes/stddef.c
	${GCCTHING} ./preprocessor/static-includes/stddef.c ${INCLUDES} -o ./build/preprocessor/static-includes/stddef.o -g -c

./build/preprocessor/static-includes/stdarg.o: ./preprocessor/static-includes/stdarg.c
	${GCCTHING} ./preprocessor/static-includes/stdarg.c ${INCLUDES} -o ./build/preprocessor/static-includes/stdarg.o -g -c


./build/lexer.o: ./lexer.c
	${GCCTHING} lexer.c ${INCLUDES} -o ./build/lexer.o -g -c

./build/lex_process.o: ./lex_process.c
	${GCCTHING} lex_process.c ${INCLUDES} -o ./build/lex_process.o -g -c

./build/token.o: ./token.c
	${GCCTHING} token.c ${INCLUDES} -o ./build/token.o -g -c

./build/expressionable.o: ./expressionable.c
	${GCCTHING} expressionable.c ${INCLUDES} -o ./build/expressionable.o -g -c

./build/parser.o: ./parser.c
	${GCCTHING} parser.c ${INCLUDES} -o ./build/parser.o -g -c

./build/validator.o: ./validator.c
	${GCCTHING} validator.c ${INCLUDES} -o ./build/validator.o -g -c

./build/symresolver.o: ./symresolver.c
	${GCCTHING} symresolver.c ${INCLUDES} -o ./build/symresolver.o -g -c

./build/codegen.o: ./codegen.c
	${GCCTHING} codegen.c ${INCLUDES} -o ./build/codegen.o -g -c

./build/scope.o: ./scope.c
	${GCCTHING} scope.c ${INCLUDES} -o ./build/scope.o -g -c

./build/helper.o: ./helper.c
	${GCCTHING} helper.c ${INCLUDES} -o ./build/helper.o -g -c

./build/resolver.o: ./resolver.c
	${GCCTHING} resolver.c ${INCLUDES} -o ./build/resolver.o -g -c

./build/compiler.o: ./compiler.c
	${GCCTHING} compiler.c ${INCLUDES} -o ./build/compiler.o -g -c

./build/cprocess.o: ./cprocess.c
	${GCCTHING} cprocess.c ${INCLUDES} -o ./build/cprocess.o -g -c

./build/array.o: ./array.c
	${GCCTHING} array.c ${INCLUDES} -o ./build/array.o -g -c 

./build/node.o: ./node.c
	${GCCTHING} node.c ${INCLUDES} -o ./build/node.o -g -c 

./build/rdefault.o: ./rdefault.c
	${GCCTHING} rdefault.c ${INCLUDES} -o ./build/rdefault.o -g -c 

./build/fixup.o: ./fixup.c
	${GCCTHING} fixup.c ${INCLUDES} -o ./build/fixup.o -g -c 


./build/native.o: ./native.c
	${GCCTHING} native.c ${INCLUDES} -o ./build/native.o -g -c 


./build/stackframe.o: ./stackframe.c
	${GCCTHING} stackframe.c ${INCLUDES} -o ./build/stackframe.o -g -c 


# Helper files
./build/helpers/vector.o: ./helpers/vector.c
	${GCCTHING} ./helpers/vector.c ${INCLUDES} -o ./build/helpers/vector.o -g -c

./build/helpers/buffer.o: ./helpers/buffer.c
	${GCCTHING} ./helpers/buffer.c ${INCLUDES} -o ./build/helpers/buffer.o -g -c

./build/helpers/hashmap.o: ./helpers/hashmap.c
	${GCCTHING} ./helpers/hashmap.c ${INCLUDES} -o ./build/helpers/hashmap.o -g -c




clean:
	rm -rf ${OBJECTS}
	rm -rf ./main
	rm -rf ./a.out
	rm -rf ./test.asm