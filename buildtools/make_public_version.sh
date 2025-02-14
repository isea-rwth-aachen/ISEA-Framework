#! /bin/bash

function removeScenarios
{
    cd scenarios
    scenariosToKeep=(aging/i3Cell_aging thermal_electrical/i3Cell)

    # remove xml files from the scenarios folder
    rmCommand="find . -type f \("
    for i in "${scenariosToKeep[@]}"
    do
        rmCommand="$rmCommand -not -regex '\./$i\.\(xml\|csv\|txt\)'"
    done
    rmCommand="$rmCommand \) -delete"
    eval $rmCommand

    # remove empty scenario folders
    find . -type d -empty -delete

    cd ../scenariotest/standalone_test

    # remove reference files for the deleted scenarios
    rmCommand="find . -mindepth 1 -maxdepth 1 -type d \("
    for i in "${scenariosToKeep[@]}"
    do
        dirName="${i##*/}"
        rmCommand="$rmCommand -not -name '$dirName'"
    done
    rmCommand="$rmCommand \) -exec rm -r {} +"
    eval $rmCommand

    cd ../..
}

function removeFolders
{
    rm -r sFunctions
    rm -r src/simulink_mock
    rm -r src/scenariotest
    rm -r scenariotest/simulink_test
    rm doxygen/simulink.md
    rm doxygen/xmlcinterface.md
}

function applyPatch
{
    git apply buildtools/public_release.patch
    rm buildtools/public_release.patch
}

function changePipeline
{
    mv buildtools/public_release_pipeline.yml .gitlab-ci.yml
}

function main
{
    if [ -z $1 ]
    then
        echo "Usage: ./make_public_version.sh <iseaframe-root>"
        exit 1
    fi
    rootdir=$1
    cd $rootdir
    removeScenarios
    removeFolders
    applyPatch
    changePipeline

    # switch to release branch without checking out files
    git symbolic-ref HEAD refs/heads/cleanUpForPublicNoHist
    git reset

    echo "Changes for the public release have been applied. Please review the changes and create a new commit."
    echo "To push the commit to the public gitlab:"
    echo -e "\tgit push rwth-gitlab cleanUpForPublicNoHist:master"

    rm buildtools/make_public_version.sh
}

main $@
