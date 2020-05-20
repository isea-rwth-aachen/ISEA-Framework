#!/bin/bash
#declare -a TREE=("ADD(1,1) ADD(ID0_ObjR,1) MUL(1,1) MUL(ID0_ObjR,2) SUB(1,1) SUB(ID0_ObjR,1) ADD(TANH(ID0_ObjR),ADD(TANH(ID1_ObjR),TANH(ID2_ObjR))) MUL(TANH(ID0_ObjR),MUL(TANH(ID1_ObjR),TANH(ID2_ObjR))) ADD(ADD(1,ID0_ObjR),2) MUL(MUL(3,ID0_ObjR),2) MUL(ADD(3,ID0_ObjR),2) MUL(2,ADD(3,ID0_ObjR)) MUL(ADD(ID0_ObjR,ID1_ObjR),2) MUL(2,ADD(ID0_ObjR,ID1_ObjR)) MUL(SUB(ID0_ObjR,ID1_ObjR),2) MUL(2,SUB(ID0_ObjR,ID1_ObjR)) MUL(ID0_ObjR,ADD(ID1_ObjR,ID2_ObjR)) MUL(ADD(ID1_ObjR,ID2_ObjR),ID0_ObjR) MUL(ID0_ObjR,SUB(ID1_ObjR,ID2_ObjR)) MUL(SUB(ID1_ObjR,ID2_ObjR),ID0_ObjR)")
#
#count=0
#
#for i in ${TREE[@]};
#do
#    txt="R" 
#    txt+=$count
#    txt+="_unoptimzed.dot"
#    echo $i | ./symbolic2Dot > $txt
#    dot -Tsvg $txt > $txt.svg
#    dot -Tpng $txt > $txt.png
#
#    txt="R" 
#    txt+=$count
#    txt+="_optimzed.dot"
#    echo $i | ./symbolicOptimization | ./symbolic2Dot > $txt
#    dot -Tsvg $txt > $txt.svg
#    dot -Tpng $txt > $txt.png
#    ((count++))
#done


declare -a TREE=("DIV(10,5) ADD(DIV(ID0_ObjR,ID1_ObjR),ID2_ObjR) ADD(ID2_ObjR,DIV(ID0_ObjR,ID1_ObjR)) SUB(DIV(ID0_ObjR,ID1_ObjR),ID2_ObjR) SUB(ID2_ObjR,DIV(ID0_ObjR,ID1_ObjR)) DIV(ID0_ObjR,DIV(1,ID0_ObjR)) DIV(ID0_ObjR,DIV(2,ID0_ObjR)) ADD(DIV(ID0_ObjR,ID1_ObjR),DIV(ID3_ObjR,ID4_ObjR)) SUB(DIV(ID0_ObjR,ID1_ObjR),DIV(ID3_ObjR,ID4_ObjR)) ADD(DIV(ID0_ObjR,ID1_ObjR),DIV(ID3_ObjR,ID1_ObjR)) SUB(DIV(ID0_ObjR,ID1_ObjR),DIV(ID3_ObjR,ID1_ObjR))")


count=1

for i in ${TREE[@]};
do
    txt="RA" 
    txt+=$count
    txt+="_unoptimzed.dot"
    echo $i | ./symbolic2Dot > $txt
    dot -Tsvg $txt > $txt.svg
    dot -Tpng $txt > $txt.png

    txt="RA" 
    txt+=$count
    txt+="_optimzed.dot"
    echo $i | ./symbolicOptimization | ./symbolic2Dot > $txt
    dot -Tsvg $txt > $txt.svg
    dot -Tpng $txt > $txt.png
    ((count++))
done


