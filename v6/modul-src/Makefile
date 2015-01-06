PACKAGES=`ls -d */`

.PHONY: all clean

default all clean install:
	for p in $(PACKAGES); do \
		(cd $$p && $(MAKE) $@); \
	done 
