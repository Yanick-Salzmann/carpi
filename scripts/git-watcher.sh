#!/bin/bash

git remote update

UPSTREAM=${1:-'@{u}'}
LOCAL=$(git rev-parse @)
REMOTE=$(git rev-parse "$UPSTREAM")
BASE=$(git merge-base @ "$UPSTREAM")

if [ $LOCAL = $REMOTE ]; then
    echo "Up-to-date"
elif [ $LOCAL = $BASE ]; then
    echo "Need to pull local=$LOCAL remote=$REMOTE"
    git pull origin master
    mkdir -p cmake-build-debug
    (cd cmake-build-debug && /usr/local/bin/cmake ..)
    (cd cmake-build-debug && /usr/local/bin/cmake --build . -- -j $(( $(lscpu | awk '/^Socket\(s\)/{ print $2 }') * $(lscpu | awk '/^Core\(s\) per socket/{ print $4 }') )))
fi