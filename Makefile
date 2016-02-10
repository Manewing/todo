##########################
#        Check CMD       #
##########################
ifeq ($(VERBOSE), 1)
	V=
else
	V=@
endif

ifeq ($(DEBUG), 1)
	GFLAG = -ggdb
else
	GFLAG = -ggdb
endif

###########################
#        Settings         #
###########################
TARGET:=td

BUILDDIR:=build
INCDIR  :=inc
SRCDIR  :=src
DEFINES :=
INCLUDE := -I $(INCDIR)
LIBS    :=-lncurses
WARNING :=-Wall

CPP:=g++
CC :=gcc

CPP_EXT:=cpp
C_EXT  :=c


###########################
#           Init          #
###########################

CPP_FLAGS:=$(GFLAG) $(WARNING)
C_FLAGS  :=$(GFLAG) $(WARNING)

CPP_SRC  :=$(wildcard $(SRCDIR)/*.$(CPP_EXT))
C_SRC    :=$(wildcard $(SRCDIR)/*.$(C_EXT))
SRC      :=$(CPP_SRC) $(C_SRC)
CPP_OBJS :=$(patsubst %.$(CPP_EXT),$(BUILDDIR)/%.o,$(CPP_SRC))
C_OBJS   :=$(patsubst %.$(C_EXT),$(BUILDDIR)/%.o,$(C_SRC))
OBJS     :=$(CPP_OBJS) $(C_OBJS)
CPP_DEPS :=$(patsubst %.$(CPP_EXT),$(BUILDDIR)/%.d,$(CPP_SRC))
C_DEPS   :=$(patsubst %.$(C_EXT),$(BUILDDIR)/%.d,$(C_SRC))
DEPS     :=$(CPP_DEPS) $(C_DEPS)


#####################
#      Compile      #
#####################


.PHONY: all clean run
.SUFFIXES:
.SECONDARY:

all: $(TARGET)

#debug:
#	@echo DEBUG ON!
#	@echo "SRC: "$(SRC)
#	@echo "OBJS: "$(OBJS)
#	@echo "DEPS: "$(DEPS)
#	@echo "------------- END OF DEBUG -----------"

ifneq ($(MAKECMDGOALS), clean)
-include $(DEPS)
endif

$(BUILDDIR):
	@echo "# Creating build-directory ( "$(BUILDDIR)")"
	$(V) mkdir -p $(BUILDDIR)/$(SRCDIR)

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
