function[] = TestMatrices()

dglCount = 3;
algCount = 2;
totalCount = dglCount + algCount;

ADgl = MakeTestMatrix(dglCount, totalCount, 1.1);
CDgl = MakeTestMatrix(dglCount, 1, 1.2);
AAlg1 = MakeTestMatrix(algCount, dglCount, 1.3);
AAlg2 = MakeTestMatrix(algCount, algCount, 1.4);
CAlg = MakeTestMatrix(algCount, 1, 1.5);

states.signals.values = ones(1, totalCount) * 1.6;

MakeMatrixCsv(states, ADgl, CDgl, AAlg1, AAlg2, CAlg, dglCount, algCount);

end



function[matrix] = MakeTestMatrix(rows, cols, val)

matrix.signals.values = zeros(1, rows * cols * 3);
for i = 0:(rows-1)
    for j = 0:(cols-1)
        matrix.signals.values(1, (j + i*cols) * 3 + 1) = i;
        matrix.signals.values(1, (j + i*cols) * 3 + 2) = j;
        matrix.signals.values(1, (j + i*cols) * 3 + 3) = val;
    end
end

end
