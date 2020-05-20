import numpy
import scipy.io

díff = ''

def errorFunction(description, *args):
    global diff
    diff += description.format(*args) + '\n'

def compareMatfiles(fileA, fileRef):
    global diff
    contentA = scipy.io.loadmat(fileA)
    contentRef = scipy.io.loadmat(fileRef)
    diff = ''
    if compareMatlab(contentA, contentRef):
        return True
    else:
        return diff

def compareMatlab(matA, matRef, location=''):
    if type(matA) != type(matRef):
        errorFunction('Field {} has the wrong type.', location);
        return False
    elif type(matA) == dict:
        return compareDicts(matA, matRef, location)
    elif type(matA) == numpy.ndarray:
        return compareArrays(matA, matRef, location)
    else:
        errorFunction('Field {} has unknown type {}.', location, type(matA));
        return False

def compareDicts(dictA, dictRef, location):
    if dictA.keys() == dictRef.keys():
        success = True
        for key in dictA:
            if key.startswith('__'):
                continue
            success = success and compareMatlab(dictA[key], dictRef[key], location + '.' + key)
        return success
    else:
        for key in dictA:
            if key not in dictRef:
                errorFunction('Unexpected field {} in {}.', key, location)
        for key in dictRef:
            if key not in dictA:
                errorFunction('Missing field {} in {}.', key, location)
        return False

def compareArrays(arrayA, arrayRef, location):
    # test if array is a structure
    if arrayA.dtype.names is not None or arrayRef.dtype.names is not None:
        if arrayA.dtype.names == arrayRef.dtype.names:
            success = True
            for fieldname in arrayA.dtype.names:
                success = success and  compareMatlab(arrayA[fieldname], arrayRef[fieldname], location + '.' + fieldname)
            return success
        else:
            for fieldname in arrayA.dtype.names:
                if fieldname not in arrayRef.dtype.names:
                    errorFunction('Unexpected field {} in {}.', fieldname, location)
            for fieldname in arrayRef.dtype.names:
                if fieldname not in arrayA.dtype.names:
                    errorFunction('Missing field {} in {}.', fieldname, location)
            return False
    elif arrayA.dtype == object and arrayRef.dtype == object:
        if arrayA.shape != arrayRef.shape:
            errorFunction('Field {} has the wrong shape.', location)
            return False
        else:
            success = True
            for i in range(arrayA.size):
                success = success and compareMatlab(arrayA[i], arrayRef[i], location + '[' + str(i) + ']')
            return success
    else:
        try:
            success = numpy.allclose(arrayA, arrayRef)
            if not success:
                errorFunction('Field {} has the wrong value.', location)
                return False
            else:
                return True
        except:
                errorFunction('Could not compare field {}.', location)
                return False
