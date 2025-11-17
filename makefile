CFLAGS = -D _DEBUG -ggdb3 -std=c++17 -O0 -Wall -Wextra -Weffc++ -Wc++14-compat -Wmissing-declarations \
         -Wcast-align -Wcast-qual -Wchar-subscripts -Wconversion -Wctor-dtor-privacy -Wempty-body \
         -Wfloat-equal -Wformat-nonliteral -Wformat-security -Wformat-signedness -Wformat=2 -Winline \
         -Wnon-virtual-dtor -Woverloaded-virtual -Wpacked -Wpointer-arith -Winit-self -Wredundant-decls \
         -Wshadow -Wsign-conversion -Wsign-promo -Wstrict-overflow=2 -Wsuggest-override -Wswitch-default \
         -Wswitch-enum -Wundef -Wunreachable-code -Wunused -Wvariadic-macros \
         -Wno-missing-field-initializers -Wno-narrowing -Wno-old-style-cast -Wno-varargs -Wstack-protector \
         -fcheck-new -fsized-deallocation -fstack-protector -fstrict-overflow -fno-omit-frame-pointer \
         -Wlarger-than=8192 -fPIE -Werror=vla \
         #-fsanitize=address,alignment,bool,bounds,enum,float-cast-overflow,float-divide-by-zero,integer-divide-by-zero,nonnull-attribute,null,return,returns-nonnull-attribute,shift,signed-integer-overflow,undefined,unreachable,vla-bound,vptr

LDFLAGS =
OBJDIR = objects
CC = g++
SOURCES = main.cpp tree_func.cpp logger.cpp dot.cpp tree_verify.cpp
OBJECTS = $(addprefix $(OBJDIR)/, $(SOURCES:.cpp=.o))
EXECUTABLE = binary_file

STACK_SRC_DIR = stack
STACK_OBJDIR = stack_obj
STACK_SOURCES = stack_func.cpp defender_system.cpp stack_attack.cpp
STACK_OBJECTS = $(addprefix $(STACK_OBJDIR)/, $(STACK_SOURCES:.cpp=.o))
STACK_LIB = stack_lib.a

all: stack $(EXECUTABLE)

stack: $(STACK_LIB)

$(STACK_LIB): $(STACK_OBJECTS)
	@ar rcs $@ $^

$(STACK_OBJDIR)/%.o: $(STACK_SRC_DIR)/%.cpp
	@mkdir -p $(@D)
	@$(CC) $(CFLAGS) -c $< -o $@

$(EXECUTABLE): $(OBJECTS) $(STACK_LIB)
	$(CC) $(LDFLAGS) $(OBJECTS) $(STACK_LIB) -o $@

$(OBJDIR)/%.o: %.cpp
	@mkdir -p $(@D)
	@$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJDIR) $(EXECUTABLE)

cleanstack:
	rm -rf $(STACK_OBJDIR) $(STACK_LIB)

