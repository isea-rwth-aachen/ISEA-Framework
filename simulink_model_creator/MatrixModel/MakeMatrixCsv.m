function[] = MakeMatrixCsv(states, ADgl, CDgl, AAlg1, AAlg2, CAlg, dglCount, algCount, CurrentsMatrix, CurrentsMatrixSeparator, VoltagesMatrix, VoltagesMatrixSeparator)

totalCount = dglCount + algCount;
numberOfTimes = size(ADgl.signals.values, 1);
numberOfCellsPlusTotal = size(CurrentsMatrixSeparator, 2) - 1;
separator = ones(1, totalCount + 1) * 123456789101112131415161719.2;
TotalM = [];

for i = 1:numberOfTimes
    M = zeros(totalCount, totalCount + 1);

    for j = 1:(size(ADgl.signals.values, 2) / 3)
        M( ADgl.signals.values(i, 3*j-2) + 1, ADgl.signals.values(i, 3*j-1) + 1 ) = ADgl.signals.values(i, 3*j);
    end

    for j = 1:(size(CDgl.signals.values, 2) / 3)
        M( CDgl.signals.values(i, 3*j-2) + 1, CDgl.signals.values(i, 3*j-1) + totalCount + 1 ) = CDgl.signals.values(i, 3*j);
    end

    for j = 1:(size(AAlg1.signals.values, 2) / 3)
        M( AAlg1.signals.values(i, 3*j-2) + dglCount + 1, AAlg1.signals.values(i, 3*j-1) + 1 ) = AAlg1.signals.values(i, 3*j);
    end

    for j = 1:(size(AAlg2.signals.values, 2) / 3)
        M( AAlg2.signals.values(i, 3*j-2) + dglCount + 1, AAlg2.signals.values(i, 3*j-1) + dglCount + 1 ) = AAlg2.signals.values(i, 3*j);
    end

    for j = 1:(size(CAlg.signals.values, 2) / 3)
        M( CAlg.signals.values(i, 3*j-2) + dglCount + 1, CAlg.signals.values(i, 3*j-1) + totalCount + 1 ) = CAlg.signals.values(i, 3*j);
    end
    
    TotalM = [TotalM; M];
    TotalM = [TotalM; separator];
end



csvwrite('SystemMatrixSimulink.csv', TotalM);
csvwrite('StatesSimulink.csv', states.signals.values);



TotalV = [];
TotalC = [];

for i = 1:numberOfTimes
    M = zeros(numberOfCellsPlusTotal, totalCount + 1);
    sepCount = 1;
    for j = 1:(size(CurrentsMatrix.signals.values, 2) / 3)
        if(j >= CurrentsMatrixSeparator.signals.values(1, sepCount+1) + 1)
           sepCount = sepCount + 1;
        end
        row = CurrentsMatrix.signals.values(i, 3*j-2) + sepCount;
        col = CurrentsMatrix.signals.values(i, 3*j-1) + 1;
        M( CurrentsMatrix.signals.values(i, 3*j-2) + sepCount, CurrentsMatrix.signals.values(i, 3*j-1) + 1 ) = 1;
        M( CurrentsMatrix.signals.values(i, 3*j-2) + sepCount, CurrentsMatrix.signals.values(i, 3*j-1) + 1 ) = CurrentsMatrix.signals.values(i, 3*j);
    end
    TotalC = [TotalC; M];
    TotalC= [TotalC; separator];

    M = zeros(numberOfCellsPlusTotal, totalCount + 1);
    sepCount = 1;
    for j = 1:(size(VoltagesMatrix.signals.values, 2) / 3)
        if(j >= VoltagesMatrixSeparator.signals.values(1, sepCount+1) + 1)
           sepCount = sepCount + 1;
        end
        M( VoltagesMatrix.signals.values(i, 3*j-2) + sepCount, VoltagesMatrix.signals.values(i, 3*j-1) + 1 ) = VoltagesMatrix.signals.values(i, 3*j);
    end
    TotalV = [TotalV; M];
    TotalV= [TotalV; separator];
end

csvwrite('CurrentsMatrixSimulink.csv', TotalC);
csvwrite('VoltagesMatrixSimulink.csv', TotalV);

end
