function[] = MatrixCreateParallelRC(destination, iD ...
                                  , rObjRowPoints, rObjColPoints, rObjMatrix ...
                                  , tauObjRowPoints, tauObjColPoints, tauObjMatrix)

[position, number] = GetPosition(destination);
destination = SetElemNoToDestination(destination, number);

rIdString = ['ID' num2str(iD) '_ObjR'];
tauIdString = ['ID' num2str(iD) '_ObjTau'];

add_block('ElectricalElements/ParallelRC', destination, 'Position', position);
SetObject(destination, 'Object', 'ObjectGoto/1', rIdString, rObjRowPoints, rObjColPoints, rObjMatrix);
set_param([destination '/ObjectGoto'], 'GotoTag', rIdString);
SetObject(destination, 'Tau', 'TauGoto/1', tauIdString, tauObjRowPoints, tauObjColPoints, tauObjMatrix);
set_param([destination '/TauGoto'], 'GotoTag', tauIdString);

end
