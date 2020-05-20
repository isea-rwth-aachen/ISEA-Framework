import difflib

def compareTextfiles(fileA, fileRef):
    handleA = open(fileA, 'r')
    handleRef = open(fileRef, 'r')
    difflines = list(difflib.unified_diff(handleRef.readlines(), handleA.readlines()))
    handleA.close()
    handleRef.close()
    if difflines:
        return ''.join(difflines)
    else:
        return True
