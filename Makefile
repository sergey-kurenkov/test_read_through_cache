.PHONE: all test run_test coverage clean

all:
	$(MAKE) -C cpp $@

test:
	$(MAKE) -C cpp $@


run_test:
	$(MAKE) -C cpp $@


coverage:
	$(MAKE) -C cpp $@


run_coverage:
	$(MAKE) -C cpp $@


clean:
	$(MAKE) -C cpp $@
    find . -name "*.pyc" -exec rm {} \;


