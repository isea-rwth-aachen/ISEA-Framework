#!/usr/bin/python3 -u

from executableTest import ExecutableTest as ExecTest
import sys, os


def testStandalone(standalone, xmlFiles):
    if len(sys.argv) < 3:
        print("Usage: ./testStandalone.py <standalone> <xml>")
        return 0

    success = True

    for xml in xmlFiles:
        # Use filename without .xml as scenario name
        scenario = os.path.splitext(os.path.basename(xml))[0]
        if (scenario == "*"): # happens if * is used on an empty/nonexistant directory
            continue
        profile = os.path.splitext(xml)[0] + ".txt"
        if not os.path.isfile(profile):
            profile = os.path.splitext(xml)[0] + ".csv"
        if not os.path.isfile(profile):
            profile = "./shortProfile.csv"  # default if no other profile exists

        test = ExecTest(
            scenario,
            [
                os.path.relpath(standalone, scenario),
                os.path.relpath(xml, scenario),
                os.path.relpath(profile, scenario),
            ],
        )
        referenceFolder = scenario + "/reference"
        for (dirpath, dirnames, filenames) in os.walk(referenceFolder):
            for f in filenames:
                filepath = os.path.relpath(dirpath + "/" + f, referenceFolder)
                if f.endswith(".mat"):
                    test.addMatfile(filepath, "reference/" + filepath)
                else:
                    test.addTextfile(filepath, "reference/" + filepath)

        if test.run():
            print("\nTest successful.")
        else:
            print("\nTest failed.")
            success = False

    if success:
        print("\nAll tests successful.")
        sys.exit(0)
    else:
        print("\nTests failed.")
        sys.exit(1)


def main():
    if (len(sys.argv) < 3):
        print("Usage: testStandalone <standalone> <xml>")
        return

    testStandalone(sys.argv[1], sys.argv[2:])


if __name__ == "__main__":
    main()
