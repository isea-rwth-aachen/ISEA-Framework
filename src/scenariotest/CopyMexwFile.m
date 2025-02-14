%% Copy XML file
function CopyMexwFile(pathToFinalRelease, model, mexw32String, mexw64String, fileID)

% Enable deleting of the current mewx-file
clear mex;

try
    mexwPath = [pathToFinalRelease, '/model/', model, '/', mexw32String];
    copyfile(mexwPath);
    fprintf(fileID, [GetTimeStamp(), ' Copied mexw-file "%s". \n'], mexwPath);
end

try
    mexwPath = [pathToFinalRelease, '/model/', model, '/', mexw64String];
    copyfile(mexwPath);
    fprintf(fileID, [GetTimeStamp(), ' Copied mexw-file "%s". \n'], mexwPath);
end

end