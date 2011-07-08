g++ -c libper~~.cpp -DBUILD_DLL
g++ -shared -o libper~~.dll libper~~.o
rem -Wl,--out-implib,libper~~.a
