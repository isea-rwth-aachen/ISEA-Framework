myFolder ='..\images';
if ~isdir(myFolder)
    errorMessage = sprintf('Error: The following folder does not exist:\n%s', myFolder);
    uiwait(warndlg(errorMessage));
    return;
end
filePattern = fullfile(myFolder, '*.jpg');
jpgFiles = dir(filePattern);

writerObj = VideoWriter('MyAVI.avi');
writerObj.FrameRate=1;
open(writerObj);


for frameNumber = 1 : length(jpgFiles)
    % Construct the full filename.
    baseFileName = strcat("img",num2str(frameNumber),".jpg");
    fullFileName = fullfile(myFolder, baseFileName);
    % Display image name in the command window.
    fprintf(1, 'Now reading %s\n', fullFileName);
    % Display image in an axes control.
    thisimage = imread(fullFileName);
    imshow(thisimage);  % Display image.
    drawnow; % Force display to update immediately.
    % Write this frame out to the AVI file.
    writeVideo(writerObj, thisimage);
end
% Close down the video writer object to finish the file.
close(writerObj)