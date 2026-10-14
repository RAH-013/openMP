CONTAINER ?= docker
IMAGE ?= openmp
THREADS ?= 4
CFLAGS ?= -Wall -Wextra -O2 -fopenmp
ARGS ?=

.PHONY: main mandelbrot clean

main:
	$(CONTAINER) run --rm \
		-v "$(CURDIR):/workspace" \
		-w /workspace \
		-e OMP_NUM_THREADS=$(THREADS) \
		$(IMAGE) \
		sh -c 'mkdir -p bin && gcc $(CFLAGS) src/main.c -o bin/main && ./bin/main $(ARGS)'

mandelbrot:
	$(CONTAINER) run --rm \
		-v "$(CURDIR):/workspace" \
		-w /workspace \
		-e OMP_NUM_THREADS=$(THREADS) \
		$(IMAGE) \
		sh -c 'mkdir -p bin && gcc $(CFLAGS) src/mandelbrot.c -o bin/mandelbrot && ./bin/mandelbrot $(ARGS)'

clean:
	rm -f bin/main bin/mandelbrot