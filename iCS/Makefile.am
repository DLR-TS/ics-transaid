SUBDIRS = src bin

EXTRA_DIST = README ChangeLog AUTHORS COPYING INSTALL NEWS sumo.doxyconf

doc:
	$(MAKE) -C docs $@

dist-hook:
	abs_distdir="`pwd`/$(distdir)" \
	&& cd $(srcdir) \
	&& for f in `find build -name .svn -prune -o -type d -print`; do mkdir $$abs_distdir/$$f; done \
	&& find build -regex ".*\.\(vcproj\|vsprops\|sln\|spec\)" -print0 | xargs -0 cp --parents --target-directory $$abs_distdir \
	&& find tools -regex ".*\.\(pl\|py\|java\|jar\|bat\)" -print0 | xargs -0 cp --parents --target-directory $$abs_distdir

dist-complete: dist
	mv $(PACKAGE)-$(VERSION).tar.gz $(PACKAGE)-src-$(VERSION).tar.gz
	mv $(PACKAGE)-$(VERSION).zip $(PACKAGE)-src-$(VERSION).zip

iCS$(EXEEXT) iTETRIS-iCS$(EXEEXT):
	$(MAKE) -C src $@

