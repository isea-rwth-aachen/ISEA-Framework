function [] = MakeDefinesH(system, numberOfCellelements, dglCount, algCount)

fileString = sprintf('#ifdef __cplusplus\nextern "C" {\n#endif\n\n');

fileString = [fileString sprintf('#define DGL_COUNT ')];
fileString = [fileString num2str(dglCount) sprintf('\n')];
fileString = [fileString sprintf('#define ALG_COUNT ')];
fileString = [fileString num2str(algCount) sprintf('\n\n')];



allComponents = GetAllComponents([system '/MatrixADgl']);
numberOfComponents = size(allComponents, 1);
if(numberOfComponents == 0)
    numberOfComponents = 1;
end
fileString = [fileString sprintf('#define A_DGL_DIM ')];
fileString = [fileString num2str(numberOfComponents * 3) sprintf('\n')];

allComponents = GetAllComponents([system '/MatrixCDgl']);
numberOfComponents = size(allComponents, 1);
if(numberOfComponents == 0)
    numberOfComponents = 1;
end
fileString = [fileString sprintf('#define C_DGL_DIM ')];
fileString = [fileString num2str(numberOfComponents * 3) sprintf('\n')];

allComponents = GetAllComponents([system '/MatrixAAlg1']);
numberOfComponents = size(allComponents, 1);
if(numberOfComponents == 0)
    numberOfComponents = 1;
end
fileString = [fileString sprintf('#define A_ALG1_DIM ')];
fileString = [fileString num2str(numberOfComponents * 3) sprintf('\n')];

allComponents = GetAllComponents([system '/MatrixAAlg2']);
numberOfComponents = size(allComponents, 1);
if(numberOfComponents == 0)
    numberOfComponents = 1;
end
fileString = [fileString sprintf('#define A_ALG2_DIM ')];
fileString = [fileString num2str(numberOfComponents * 3) sprintf('\n')];

allComponents = GetAllComponents([system '/MatrixCAlg']);
numberOfComponents = size(allComponents, 1);
if(numberOfComponents == 0)
    numberOfComponents = 1;
end
fileString = [fileString sprintf('#define C_ALG_DIM ')];
fileString = [fileString num2str(numberOfComponents * 3) sprintf('\n\n')];



allComponents = GetAllComponents([system '/CellCurrentMatrices']);
numberOfComponents = size(allComponents, 1);
fileString = [fileString sprintf('#define CURRENT_MATRIX_DIM ')];
fileString = [fileString num2str(numberOfComponents * 3) sprintf('\n')];

allComponents = GetAllComponents([system '/CellVoltageMatrices']);
numberOfComponents = size(allComponents, 1);
fileString = [fileString sprintf('#define VOLTAGE_MATRIX_DIM ')];
fileString = [fileString num2str(numberOfComponents * 3) sprintf('\n\n')];



fileString = [fileString sprintf('#define NUMBER_OF_CELL_ELEMENTS ')];
fileString = [fileString num2str(numberOfCellelements) sprintf('\n\n')];

fileString = [fileString sprintf('#ifdef __cplusplus\n}\n#endif\n')];

fileID = fopen('defines.h','w');
fprintf(fileID, '%s', fileString);
fclose(fileID);

end

