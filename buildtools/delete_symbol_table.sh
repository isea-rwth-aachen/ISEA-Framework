#!/bin/bash


echo -en "\n\033[1;31m######################## Creating Release ########################\033[0m\n"
echo -en "\n\033[1;31mMerge all .o files ...\033[0m\n"
file CMakeFiles/ISEAFrame.dir/src/electrical/paralleltwoport.cpp.o | grep 32-bit
if [ $? == 0 ]
then

echo -en "\033[1;32mld -melf_i386 -r CMakeFiles/ISEAFrame.dir/src/*/*.o src/cinterface/CMakeFiles/ISEAFrameCInterfaceObject.dir/cinterface.cpp.o -o libISEA.o\033[0m\n\n"
ld -r -melf_i386 $(find ./CMakeFiles/ -name "*.o") src/cinterface/CMakeFiles/ISEAFrameCInterfaceObject.dir/cinterface.cpp.o -o libISEA.o
else 
echo -en "\033[1;32mld -r CMakeFiles/ISEAFrame.dir/src/*/*.o src/cinterface/CMakeFiles/ISEAFrameCInterfaceObject.dir/cinterface.cpp.o -o libISEA.o\033[0m\n\n"
ld -r $(find ./CMakeFiles/ -name "*.o") src/cinterface/CMakeFiles/ISEAFrameCInterfaceObject.dir/cinterface.cpp.o -o libISEA.o
fi

echo -en "\033[1;31mFind global symbols in cinterface.cpp.o ...\033[0m\n"
echo -en "\033[1;32mobjdump -t src/cinterface/CMakeFiles/ISEAFrameCInterfaceObject.dir/cinterface.cpp.o  | grep " g  " | tr -s ' ' | cut -d ' ' -f 5 > keep.syms\033[0m\n\n"
objdump -t src/cinterface/CMakeFiles/ISEAFrameCInterfaceObject.dir/cinterface.cpp.o  | grep " g  " | tr -s ' ' | cut -d ' ' -f 5 > keep.syms

echo -en "\033[1;31mFind extern symbols in merged .o ...\033[0m\n"
echo -en "\033[1;32mnm -u libISEA.o  | tr -s " " | cut -d " " -f3 >> keep.syms \033[0m\n\n"
nm libISEA.o | grep " __" | tr -s " " | cut -d " " -f3 >> keep.syms
nm -u libISEA.o  | tr -s " " | cut -d " " -f3 >> keep.syms
objdump -t libISEA.o  | grep " g  " | tr -s ' ' | cut -d ' ' -f 5 >> keep.syms

echo -en "\033[1;31mStrip unwanted symbols ...\033[0m\n"
echo -en "\033[1;32mstrip -w -K \!*_Z* \$(sed 's/^/-K /' keep.syms) libISEA.o -o libISEA_stripped.o \033[0m\n\n"
strip -w -K \!*_Z* $(sed 's/^/-K /' keep.syms) libISEA.o -o libISEA_stripped.o

echo -en "\033[1;31mPack everything together for library ...\033[0m\n"
echo -en "\033[1;32mar rcs libISEAFrame_stripped.a libISEA_stripped.o\033[0m\n\n"
ar  rcs libISEAFrame_stripped.a libISEA_stripped.o

echo -en "\033[1;31mCheck symboltable ...\033[0m\n"
echo -en "\033[1;32mnm -C libISEAFrame_stripped.a | less\033[0m\n\n"
#nm -C libISEAFrame_stripped.a | less


