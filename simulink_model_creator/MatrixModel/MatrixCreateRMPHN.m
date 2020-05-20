function[destination] = MatrixCreateRMPHN(destination, iD ...
                                  , rObjRowPoints, rObjColPoints, rObjMatrix ...
                                  , tauObjRowPoints, tauObjColPoints, tauObjMatrix)

[position, number] = GetPosition(destination);
destination = SetElemNoToDestination(destination, number);

objectIdString = ['ID' num2str(iD) '_ObjCT'];
tauIdString = ['ID' num2str(iD) '_ObjMP'];

add_block('ElectricalElements/ParallelRC', destination, 'Position', position);
SetObject(destination, 'Object', 'ObjectGoto/1', objectIdString, rObjRowPoints, rObjColPoints, rObjMatrix);
set_param([destination '/ObjectGoto'], 'GotoTag', objectIdString);
SetObject(destination, 'Tau', 'TauGoto/1', tauIdString, tauObjRowPoints, tauObjColPoints, tauObjMatrix);
set_param([destination '/TauGoto'], 'GotoTag', tauIdString);

end
