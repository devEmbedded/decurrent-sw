
unittests: huffman_tests deflate_test
	./huffman_tests
	./deflate_test < deflate_test | python ../tools/inflate.py >inflate_result
	cmp deflate_test inflate_result
	rm -f inflate_results


huffman_tests: huffman_tests.c | huffman.c huffman.h
	gcc -std=gnu11 -o $@ -g -Wall -Wextra -Werror $^

deflate_test: deflate_test.c deflate.c huffman.c | deflate.h huffman.h
	gcc -std=gnu11 -o $@ -g -Wall -Wextra -Werror $^
	
