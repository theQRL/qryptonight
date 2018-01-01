#!/usr/bin/env bash

echo "TRAVIS_OS_NAME" ${TRAVIS_OS_NAME}
echo "PLATFORM" ${PLATFORM}

case "${TRAVIS_OS_NAME}" in
    osx)
        echo "OSX PREPARE"
        brew install python3 swig hwloc boost
        brew outdated cmake || brew upgrade cmake
        sudo pip3 install -U pip setuptools
        ;;

    linux)
        echo "LINUX PREPARE " ${PLATFORM}
        USER_INFO="$( id -u ${USER} ):$( id -g ${USER} )"
        SHARE_USER_INFO="-v /etc/group:/etc/group:ro -v /etc/passwd:/etc/passwd:ro -u ${USER_INFO}"
        SHARE_SRC="-v $(pwd):/travis"

        docker stop $(docker ps -aq --filter name=builder) || true
        docker rm $(docker ps -aq --filter name=builder) || true
        docker build --file travis/Dockerfile.${PLATFORM} -t builder-${PLATFORM} .
        docker run -d --name builder ${SHARE_SRC} ${SHARE_USER_INFO} builder-${PLATFORM} tail -f /dev/null
        ;;
    *)
        echo "UNSUPPORTED OS"
        exit 1
        ;;
esac
