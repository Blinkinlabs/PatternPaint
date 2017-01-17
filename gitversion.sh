#!/bin/bash

# Describe the current project using a git tag, commits since the tag, and hash

VERSION='0.0.1'

GIT_VERSION=`git describe --always --tags 2> /dev/null`
VERSION=`echo ${GIT_VERSION} | sed 's/-/\./g' | sed 's/g//g'`

echo "PatternPaint version: " ${VERSION}
