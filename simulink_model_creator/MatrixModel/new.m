system = 'TestMatrixModel';
InitializeMatrixModel(system, 5);

componentCounter = 0;
symbolicString = 'NEG(MUL(ADD(ID1_Obj1,ID2_Obj1),1.0))';
row = 0;
col = 2;
MakeComponent([system '/MatrixAB'], symbolicString, componentCounter, row, col);

componentCounter = componentCounter + 1;
symbolicString = 'SUB(SQU(DIV(ID1_Obj1,ID2_Obj1)),ID3_Obj1)';
row = 1;
col = 4;
MakeComponent([system '/MatrixAB'], symbolicString, componentCounter, row, col);

ConnectMatrix([system '/MatrixAB']);
AddSeparator([system '/MatrixAB']);
DeleteSeparator([system '/MatrixAB']);

i = 0;
