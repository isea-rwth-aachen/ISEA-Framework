#! /bin/bash
set -e

args=("$@")

if [ "$#" -lt "1" ]
then
  echo "Usage: ./runTest.sh <standalones-folder>"
  exit 1
fi

echo "Running electrical scenarios"
python3 ./testStandalone.py ${args[0]}/electricalSimulation ../../scenarios/electrical/*.xml
echo "Running thermal scenarios"
python3 ./testStandalone.py ${args[0]}/thermalSimulation ../../scenarios/thermal/*.xml
echo "Running thermal-electrical scenarios"
python3 ./testStandalone.py ${args[0]}/thermalElectricalSimulation ../../scenarios/thermal_electrical/*.xml
echo "Running aging scenarios"
python3 ./testStandalone.py ${args[0]}/agingSimulation ../../scenarios/aging/*.xml
