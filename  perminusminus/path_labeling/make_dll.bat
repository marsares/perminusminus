g++ -c win_api.cpp -DBUILD_DLL
g++ -shared -o per~~.dll win_api.o
rem -Wl,--out-implib,libstatederivative.a
