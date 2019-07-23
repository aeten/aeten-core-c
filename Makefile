all: test
library:
	ninja deps
	ninja library
test: library
	ninja test
clean:
	ninja clean

