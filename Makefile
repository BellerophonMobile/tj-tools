#-----------------------------------------------------------------------
# Copyright (c) 2011 Joe Kopena <tjkopena@gmail.com>
#
# Permission is hereby granted, free of charge, to any person
# obtaining a copy of this software and associated documentation
# files (the "Software"), to deal in the Software without
# restriction, including without limitation the rights to use, copy,
# modify, merge, publish, distribute, sublicense, and/or sell copies
# of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be
# included in all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
# EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
# MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
# NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
# BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
# ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
# CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.
#-----------------------------------------------------------------------

CFLAGS += -Wall -Werror
INCS = -Isrc -Ideps/uthash/src

CFLAGS += -O0 -g
#CFLAGS += -O3

TESTS=test-tj_buffer   \
      test-tj_template \
      test-tj_heap     \
      test-tj_log      \
      test-tj_error    \
	  test-tj_array

BIN_DIR = bin
DOC_DIR = doc
TST_DIR = test

TEST_SRCS = $(addsuffix .c,$(addprefix $(TST_DIR)/,$(TESTS)))
TEST_TGTS = $(addprefix $(BIN_DIR)/,$(TESTS))

.PHONY: all clean distclean docs test

all: dirs $(TEST_TGTS)

dirs:
	@if [ ! -d $(BIN_DIR) ]; then mkdir -p $(BIN_DIR); fi

#-----------------------------------------------------------------------

bin/test-tj_buffer: test/test-tj_buffer.c src/tj_buffer.c
	$(CC) $(CFLAGS) -o $@ $^ $(INCS)

bin/test-tj_template: test/test-tj_template.c                          \
                      src/tj_template.c src/tj_buffer.c
	$(CC) $(CFLAGS) -o $@ $^ $(INCS)

bin/test-tj_heap: test/test-tj_heap.c
	$(CC) $(CFLAGS) -o $@ $^ $(INCS)

bin/test-tj_log: test/test-tj_log.c                                    \
                 src/tj_error.c src/tj_log.c src/tj_log_sqlite.c
	$(CC) $(CFLAGS) -o $@ $^ $(INCS) -lsqlite3

bin/test-tj_error: test/test-tj_error.c src/tj_error.c
	$(CC) $(CFLAGS) -o $@ $^ $(INCS)

bin/test-tj_array: test/test-tj_array.c src/tj_array.c
	$(CC) $(CFLAGS) -o $@ $(INCS) -DHAVE_MALLOC_H -Ideps/cmocka/include \
		deps/cmocka/src/cmocka.c $<

#-----------------------------------------------------------------------

clean:

distclean:
	if [ -d $(BIN_DIR) ]; then rm -rf $(BIN_DIR); fi
	if [ -d $(DOC_DIR) ]; then rm -rf $(DOC_DIR); fi

docs:
	doxygen

test: all
	@\
		tmpdir=$$(mktemp -d); \
		for test in $(TESTS); \
		do \
			echo Executing $$test; \
			$(BIN_DIR)/$$test &> "$$tmpdir/$$test.log"; \
			rv=$$?; \
			if [ $$rv -ne 0 ]; \
			then \
				echo "$$test FAILED (return value $$rv);" \
				echo OUTPUT:; \
				cat "$$tmpdir/$$test.log"; \
				exit $$rv; \
			fi; \
		done; \
		rm -rf "$$tmpdir"
