#!/usr/bin/env bash

set -e

source ./travis/prepare.sh

case "${TRAVIS_OS_NAME}" in
    osx)
        echo "OSX BUILD"

        export BUILD_DIR=build
        mkdir -p ${BUILD_DIR}
        cd ${BUILD_DIR}
        cmake ${CMAKE_ARGS} ..
        make

        if [ -n "${TEST:+1}" ]; then
          echo "Running Tests"
          export GTEST_COLOR=1
          ctest -VV
        fi
        ;;

    linux)
        echo "LINUX BUILD " ${PLATFORM}
        docker exec -t -e CC_VER=${CC_VER} -e CMAKE_ARGS=${CMAKE_ARGS} -e TEST=${TEST} -e DEPLOY=${DEPLOY} builder /build.sh
        ;;
    *)
        echo "UNSUPPORTED OS"
        exit 1
        ;;
esac
