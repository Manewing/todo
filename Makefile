##########################
#        Check CMD       #
##########################
ifeq ($(VERBOSE), 1)
	V=
else
	V=@
endif

DEBUG:=1
ifeq ($(DEBUG), 1)
	GFLAG = -ggdb -DTD_DEBUG
else
	GFLAG = -O3
endif

###########################
#        Settings         #
###########################
TARGET:=td

BUILDDIR :=build
INCDIR   :=inc
SRCDIR   :=src
TESTSDIR :=tests
DEFINES  :=
INCLUDE  := -I $(INCDIR)
LIBS     :=-lncurses
WARNING  :=-Wall

CPP:=g++
CC :=gcc

CPP_EXT:=cpp
C_EXT  :=c


###########################
#           Init          #
###########################

CPP_FLAGS:=-std=c++14 $(GFLAG) $(WARNING)
C_FLAGS  :=$(GFLAG) $(WARNING)

TESTS_SRC:=$(wildcard $(TESTSDIR)/*.$(CPP_EXT))
TESTS    :=$(patsubst %.$(CPP_EXT),%,$(TESTS_SRC))

CPP_SRC  :=$(wildcard $(SRCDIR)/*.$(CPP_EXT))
C_SRC    :=$(wildcard $(SRCDIR)/*.$(C_EXT))
SRC      :=$(CPP_SRC) $(C_SRC)
CPP_OBJS :=$(patsubst %.$(CPP_EXT),$(BUILDDIR)/%.o,$(CPP_SRC))
C_OBJS   :=$(patsubst %.$(C_EXT),$(BUILDDIR)/%.o,$(C_SRC))
OBJS     :=$(CPP_OBJS) $(C_OBJS)
CPP_DEPS :=$(patsubst %.$(CPP_EXT),$(BUILDDIR)/%.d,$(CPP_SRC))
C_DEPS   :=$(patsubst %.$(C_EXT),$(BUILDDIR)/%.d,$(C_SRC))
DEPS     :=$(TEST_DEPS) $(CPP_DEPS) $(C_DEPS)


TESTS_     :=$(patsubst %.$(CPP_EXT),$(BUILDDIR)/%.o,$(TESTS_SRC))
TESTS_OBJS :=$(filter-out $(BUILDDIR)/$(SRCDIR)/main.o, $(OBJS))

#####################
#      Compile      #
#####################


.PHONY: all clean run test
.SUFFIXES:
.SECONDARY:

all: debug $(TESTS) $(TARGET)
test: debug $(TESTS)

debug:
#	@echo DEBUG ON!
#	@echo "TARGET(s): "$(TARGET) $(TESTS)
#	@echo "SRC: "$(SRC)
#	@echo "OBJS: "$(OBJS)
#	@echo "DEPS: "$(DEPS)
#	@echo "TESTS_OBJS: "$(TESTS_OBJS)
#	@echo "------------- END OF DEBUG -----------"

ifneq ($(MAKECMDGOALS), clean)
-include $(DEPS)
endif

$(BUILDDIR):
	@echo "# Creating build-directory ( "$(BUILDDIR)")"
	$(V) mkdir -p $(BUILDDIR)/$(SRCDIR)
	$(V) mkdir -p $(BUILDDIR)/$(TESTSDIR)

$(TESTS): $(TESTS_) $(TESTS_OBJS) Makefile | $(BUILDDIR)
	@echo "# Linking Test ( "$(TESTS)" )"
	$(V) $(CPP) $(CPP_FLAGS) $(TESTS_FLAGS) $(INCLUDE) $(DEFINES) -o \
      $(patsubst $(BUILDDIR)/%.o,%,$<) $< $(TESTS_OBJS) $(LIBS)

$(TARGET): $(OBJS) $(DEPS) Makefile | $(BUILDDIR)
	@echo "# Linking ( "$(TARGET)" )"
	$(V) $(CPP) $(CPP_FLAGS) $(INCLUDE) $(DEFINES) -o $(TARGET) $(OBJS) $(LIBS)

$(BUILDDIR)/%.d: %.$(C_EXT) Makefile | $(BUILDDIR)
	@echo "# Creating dependencies for ( "$<" )"
	$(V) $(CC) $(DEFINES) $(INCLUDE) $(C_FLAGS) -M -MT $(@:.d=.o) -o $@ $<

$(BUILDDIR)/%.d: %.$(CPP_EXT) Makefile | $(BUILDDIR)
	@echo "# Creating dependencies for ( "$<" )"
	$(V) $(CPP) $(DEFINES) $(INCLUDE) $(CPP_FLAGS) -M -MT $(@:.d=.o) -o $@ $<

$(BUILDDIR)/%.o: %.$(C_EXT) Makefile | $(BUILDDIR)
	@echo "# Compiling ( "$<" )"
	$(V) $(CC) $(DEFINES) $(INCLUDE) $(C_FLAGS) -c $< -o $@

$(BUILDDIR)/%.o: %.$(CPP_EXT) Makefile | $(BUILDDIR)
	@echo "# Compiling ("$<" )"
	$(V) $(CPP) $(DEFINES) $(INCLUDE) $(CPP_FLAGS) -c $< -o $@

clean:
	@echo "# Cleaning up..."
	$(V) $(RM) -r $(BUILDDIR)
	$(V) $(RM) $(TARGET)

run:
	./$(TARGET)
