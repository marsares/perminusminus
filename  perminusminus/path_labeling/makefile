core_headers=decoder.h graph.h model.h scripts.h workbench.h

makeall: libper--.so per--
.PHONY: makeall

libper--.so: libper--.cpp command_line.cpp $(core_headers)
	g++ -shared -fPIC libper--.cpp -o libper--.so
per-- : command_line.cpp $(core_headers)
	g++ -o per-- command_line.cpp
