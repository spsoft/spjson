
#--------------------------------------------------------------------

ifeq ($(origin version), undefined)
	version = 0.1
endif

#--------------------------------------------------------------------

all:
	@( cd spjson; make )

dist: clean spjson-$(version).src.tar.gz

spjson-$(version).src.tar.gz:
	@find . -type f | grep -v CVS | grep -v .svn | sed s:^./:spjson-$(version)/: > MANIFEST
	@(cd ..; ln -s spjson spjson-$(version))
	(cd ..; tar cvf - `cat spjson/MANIFEST` | gzip > spjson/spjson-$(version).src.tar.gz)
	@(cd ..; rm spjson-$(version))

clean:
	@( cd spjson; make clean )

