function [] = ClearGlobalCache()
global component;
global gotoBus;
global CellElementCounter;

clearvars -global component;
clearvars -global gotoBus;
CellElementCounter= 0;
end
