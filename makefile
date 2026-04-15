TOPTARGETS := all clean release emscripten emscripten_release

UTILSDIR := Utils/.
FIRSTDIR :=

EXCLUDE_DIRS := CG.xcworkspace
EXCLUDE_SUBDIRS := $(addsuffix /.,$(EXCLUDE_DIRS))

SUBDIRS := $(wildcard */.)
SUBDIRS := $(filter-out \
    LatexUtils/. \
    VS141/. \
    VS/. \
    $(UTILSDIR) \
    $(FIRSTDIR) \
    $(EXCLUDE_SUBDIRS), \
    $(SUBDIRS))

$(TOPTARGETS): $(SUBDIRS)

$(SUBDIRS): $(FIRSTDIR)
	$(MAKE) -C $@ $(MAKECMDGOALS)

$(FIRSTDIR): $(UTILSDIR)
	$(MAKE) -C $@ $(MAKECMDGOALS)

$(UTILSDIR):
	$(MAKE) -C $@ $(MAKECMDGOALS)

.PHONY: $(TOPTARGETS) $(SUBDIRS) $(FIRSTDIR) $(UTILSDIR)
