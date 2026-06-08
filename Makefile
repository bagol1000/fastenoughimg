CYTHON = fastenoughimg/fastenoughimg.pyx
C = src/fastenoughimg.c
C_FROM_CYTHON = fastenoughimg/fastenoughimg.c
SO = fastenoughimg/fastenoughimg.so

CFLAGS = -shared -fPIC -fopenmp -I/usr/include/python3.12 -I/usr/include/x86_64-linux-gnu/python3.12 -I/usr/lib/python3/dist-packages/numpy/core/include

all: $(SO)

$(SO): $(C) $(C_FROM_CYTHON)
	gcc $(CFLAGS) $^ -o $@ -lm -lgomp
	
$(C_FROM_CYTHON): $(CYTHON)
	cython $(CYTHON)

clean:
	rm -f $(SO) $(C_FROM_CYTHON)
