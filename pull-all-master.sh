#!/bin/sh
set -e
git checkout master
git pull --rebase origin master
git submodule update
git submodule foreach git checkout master
git submodule foreach git pull origin master
git submodule foreach git status -s
git status -s