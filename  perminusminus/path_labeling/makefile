core_headers=decoder.h graph.h model.h scripts.h workbench.h

makeall: per--
.PHONY: makeall

libper--.so: libper--.cpp command_line.cpp $(core_headers)
	g++ -shared -fPIC libper--.cpp -o libper--.so
per-- : command_line.cpp $(core_headers)
	g++ -c -o command_line.o command_line.cpp  -O3
	g++ -o per-- command_line.o -O3


clear:
	rm per--
