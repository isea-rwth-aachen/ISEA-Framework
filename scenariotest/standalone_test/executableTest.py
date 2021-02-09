import os
import subprocess
import compareMatfiles
import compareTextfiles


class ExecutableTest:
    def __init__(self, folder, command):
        self.folder = folder
        self.command = command
        self.textfiles = []
        self.matfiles = []
        self.ignoredFiles = []
        self.success = True

    def addTextfile(self, outputFile, referenceFile):
        self.textfiles.append((outputFile, referenceFile))

    def addMatfile(self, outputFile, referenceFile):
        self.matfiles.append((outputFile, referenceFile))

    def addIgnoredFile(self, outputFile, referenceFile):
        self.ignoredFiles.append((outputFile, referenceFile))

    def isUnexceptedFile(self, filename):
        for knownFile in self.textfiles:
            if filename == knownFile[0] or filename == knownFile[1]:
                return False
        for knownFile in self.matfiles:
            if filename == knownFile[0] or filename == knownFile[1]:
                return False
        for knownFile in self.ignoredFiles:
            if filename == knownFile[0] or filename == knownFile[1]:
                return False
        return True

    def run(self,):
        if not os.path.exists(self.folder):
            os.mkdir(self.folder)
        print("\n{}:".format(self.folder))

        for f in self.textfiles:
            if os.path.exists(self.folder + "/" + f[0]):
                os.remove(self.folder + "/" + f[0])
        for f in self.matfiles:
            if os.path.exists(self.folder + "/" + f[0]):
                os.remove(self.folder + "/" + f[0])

        with open(self.folder + "/output.txt", "w") as outfile:
            print("Running {}".format(" ".join(self.command)))
            subprocess.run(self.command, cwd=self.folder, stdout=outfile)
        try:
            for (fileOut, fileRef) in self.textfiles:
                print("Comparing {} to {}".format(fileOut, fileRef))
                diff = compareTextfiles.compareTextfiles(
                    self.folder + "/" + fileOut, self.folder + "/" + fileRef
                )
                if type(diff) == str:
                    print("Found differences in file {}:".format(fileOut))
                    print(diff)
                    self.success = False
            for (fileOut, fileRef) in self.matfiles:
                print("Comparing {} to {}".format(fileOut, fileRef))
                diff = compareMatfiles.compareMatfiles(
                    self.folder + "/" + fileOut, self.folder + "/" + fileRef
                )
                if type(diff) == str:
                    print("Found differences in file {}:".format(fileOut))
                    print(diff)
                    self.success = False
        except FileNotFoundError as e:
            print("Could not find file {}".format(e.filename))
            self.success = False

        for (dirpath, dirnames, filenames) in os.walk(self.folder):
            for f in filenames:
                filepath = os.path.relpath(dirpath + "/" + f, self.folder)
                if self.isUnexceptedFile(filepath):
                    print("Unexpected file {}".format(filepath))
                    self.success = False

        return self.success
