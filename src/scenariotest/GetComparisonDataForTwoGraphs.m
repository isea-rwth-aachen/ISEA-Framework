%% Get comparison data for two graphs
function [ myX, myY1, myY2 ] = GetComparisonDataForTwoGraphs(aTime, aValues, bTime, bValues)

% Check for column or row vector
if(size(aTime, 1) > size(aTime, 2)) % aTime
    sizeA = size(aTime, 1);
else
    sizeA = size(aTime, 2);
end

if(size(bTime, 1) > size(bTime, 2)) % bTime
    sizeB = size(bTime, 1);
else
    sizeB = size(bTime, 2);
end

% Get the pair with the larger sample rate
if(sizeA > sizeB)
    myLargerTime = aTime;
    myLargerValues = aValues;
    mySmallerTime = bTime;
    mySmallerValues = bValues;
else
    myLargerTime = bTime;
    myLargerValues = bValues;
    mySmallerTime = aTime;
    mySmallerValues = aValues;
end

% Delete double and triple time and data points
erase_index_small = diff(mySmallerTime) == 0;
mySmallerTime(erase_index_small) = [];
mySmallerValues(erase_index_small) = [];
mySmallerTime(1) = 0;

erase_index_large = diff(myLargerTime) == 0;
myLargerTime(erase_index_large) = [];
myLargerValues(erase_index_large) = [];
myLargerTime(1) = 0;

% Create equidistant time and data points and interpolate
myX = mySmallerTime(1):1:mySmallerTime(end);
myY1 = interp1(mySmallerTime, mySmallerValues, myX, 'linear');
myY2 = interp1(myLargerTime, myLargerValues, myX, 'linear');

end