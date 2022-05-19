ifneq ($(MKENV_INCLUDED),1)
# We assume that mkenv is in the same directory as this file.
THIS_MAKEFILE = $(lastword $(MAKEFILE_LIST))
include $(dir $(THIS_MAKEFILE))mkenv.mk
endif

# Extra deps that need to happen before object compilation.
OBJ_EXTRA_ORDER_DEPS =

# This file expects that OBJ contains a list of all of the object files.
# The directory portion of each object file is used to locate the source
# and should not contain any ..'s but rather be relative to the top of the
# tree.
#
# So for example, py/map.c would have an object file name py/map.o
# The object files will go into the build directory and mantain the same
# directory structure as the source tree. So the final dependency will look
# like this:
#
# build/py/map.o: py/map.c
#
# We set vpath to point to the top of the tree so that the source files
# can be located. By following this scheme, it allows a single build rule
# to be used to compile all .c files.

vpath %.S . $(TOP) $(USER_C_MODULES)
$(BUILD)/%.o: %.S
	$(ECHO) "CC $<"
	$(Q)$(CC) $(CFLAGS) -c -o $@ $<

vpath %.s . $(TOP) $(USER_C_MODULES)
$(BUILD)/%.o: %.s
	$(ECHO) "AS $<"
	$(Q)$(AS) -o $@ $<

define compile_c
$(ECHO) "CC $<"
$(Q)$(CC) $(CFLAGS) -c -MD -o $@ $<
@# The following fixes the dependency file.
@# See http://make.paulandlesley.org/autodep.html for details.
@# Regex adjusted from the above to play better with Windows paths, etc.
@$(CP) $(@:.o=.d) $(@:.o=.P); \
  $(SED) -e 's/#.*//' -e 's/^.*:  *//' -e 's/ *\\$$//' \
      -e '/^$$/ d' -e 's/$$/ :/' < $(@:.o=.d) >> $(@:.o=.P); \
  $(RM) -f $(@:.o=.d)
endef

define compile_cxx
$(ECHO) "CXX $<"
$(Q)$(CXX) $(CXXFLAGS) -c -MD -o $@ $<
@# The following fixes the dependency file.
@# See http://make.paulandlesley.org/autodep.html for details.
@# Regex adjusted from the above to play better with Windows paths, etc.
@$(CP) $(@:.o=.d) $(@:.o=.P); \
  $(SED) -e 's/#.*//' -e 's/^.*:  *//' -e 's/ *\\$$//' \
      -e '/^$$/ d' -e 's/$$/ :/' < $(@:.o=.d) >> $(@:.o=.P); \
  $(RM) -f $(@:.o=.d)
endef

vpath %.c . $(TOP) $(USER_C_MODULES)
$(BUILD)/%.o: %.c
	$(call compile_c)

vpath %.cpp . $(TOP) $(USER_C_MODULES)
$(BUILD)/%.o: %.cpp
	$(call compile_cxx)

$(BUILD)/%.pp: %.c
	$(ECHO) "PreProcess $<"
	$(Q)$(CPP) $(CFLAGS) -Wp,-C,-dD,-dI -o $@ $<

# The following rule uses | to create an order only prerequisite. Order only
# prerequisites only get built if they don't exist. They don't cause timestamp
# checking to be performed.
#
# We don't know which source files actually need the generated.h (since
# it is #included from str.h). The compiler generated dependencies will cause
# the right .o's to get recompiled if the generated.h file changes. Adding
# an order-only dependency to all of the .o's will cause the generated .h
# to get built before we try to compile any of them.

# The logic for qstr regeneration (applied by makeqstrdefs.py) is:
# - if anything in QSTR_GLOBAL_DEPENDENCIES is newer, then process all source files ($^)
# - else, if list of newer prerequisites ($?) is not empty, then process just these ($?)
# - else, process all source files ($^) [this covers "make -B" which can set $? to empty]
# See more information about this process in docs/develop/qstr.rst.


# Compressed error strings.


# $(sort $(var)) removes duplicates
#
# The net effect of this, is it causes the objects to depend on the
# object directories (but only for existence), and the object directories
# will be created if they don't exist.
OBJ_DIRS = $(sort $(dir $(OBJ)))
$(OBJ): | $(OBJ_DIRS)
$(OBJ_DIRS):
	$(MKDIR) -p $@

$(HEADER_BUILD):
	$(MKDIR) -p $@

submodules:
	$(ECHO) "Updating submodules: $(GIT_SUBMODULES)"
ifneq ($(GIT_SUBMODULES),)
	$(Q)git submodule sync $(addprefix $(TOP)/,$(GIT_SUBMODULES))
	$(Q)git submodule update --init $(addprefix $(TOP)/,$(GIT_SUBMODULES))
endif
.PHONY: submodules

print-cfg:
	$(ECHO) "BUILD  = $(BUILD)"
	$(ECHO) "OBJ    = $(OBJ)"
.PHONY: print-cfg

print-def:
	@$(ECHO) "The following defines are built into the $(CC) compiler"
	$(TOUCH) __empty__.c
	@$(CC) -E -Wp,-dM __empty__.c
	@$(RM) -f __empty__.c

-include $(OBJ:.o=.P)
