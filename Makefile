CC:=g++ -g3 -Wall -fno-builtin -std=c++17 -Iapi -Iinclude -DDEBUG -D_XOPEN_SOURCE

# List of source files for your pager
PAGER_SOURCES:=$(wildcard src/*.cpp)

# Generate the names of the pager's object files
PAGER_OBJS:=${PAGER_SOURCES:.cpp=.o}

TEST_SOURCES:=$(wildcard test/*.cpp)

TEST_OBJS:=${TEST_SOURCES:.cpp=.o}

CUSTOM_HEADERS:=$(wildcard include/*.h)

all: pager app

.PHONY: all pager app clean syntax restore push

pager: env/pager

app: env/app

# Compile the pager and tag this compilation
env/pager: ${PAGER_OBJS:%=.cache/%} lib/libvm_pager_macos.o
	${CC} -o $@ $^

# Compile an application program
env/app: app.cpp lib/libvm_app_macos.o
	${CC} -o $@ $^ -ldl

env/test%: .cache/test/test%.o lib/libvm_app_macos.o
	${CC} -o $@ $^ -ldl

# Generic rules for compiling a source file to an object file
.cache/%.o: %.cpp
	@mkdir -p $(@D)
	${CC} -c $< -o $@

.cache/%.o: %.cc
	@mkdir -p $(@D)
	${CC} -c $< -o $@

syntax: ${PAGER_SOURCES} ${CUSTOM_HEADERS}
	cppcheck --enable=all --language=c++ --std=c++17 --inline-suppr --suppress=missingInclude $^

restore:
	git restore env

clean:
	rm -rf env
	git restore env
	rm -rf .cache/* pager.log
