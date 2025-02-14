function ChangeSubsystem(model, subSystemString, fileID)

%% Change the subsystem, e.g. BatteryMultiCell_El -> BatteryMulticell_Th

% Load subsystem without opening it
close_system('MultiCellInterface'); % If it should be open
load_system('MultiCellInterface');

% Replace the subsystem with the needed one
replace_block('MultiCellInterface', 'Name', 'Variable_Battery_Multicell', subSystemString, 'noprompt');

% Save and close the subsystem
save_system('MultiCellInterface');
close_system('MultiCellInterface');

fprintf(fileID, [GetTimeStamp(), ' Set subsystem to "%s". \n'], model);

end