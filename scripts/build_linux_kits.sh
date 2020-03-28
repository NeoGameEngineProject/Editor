#!/bin/sh -e

function build_and_extract_packages() {
	scripts/build_kit_linux64_static.sh "$1"
	scripts/build_kit_linux64_shared.sh "$1"

	cd build-linux64-static-$1
	tar -xf "Neo-0.6.0-Linux.tar.gz"

	rm -rf linux64-static-$1
	mv Neo-0.6.0-Linux linux64-static-$1
	cd ..

	cd build-linux64-shared-$1
	tar -xf "Neo-0.6.0-Linux.tar.gz"

	rm -rf linux64-shared-$1
	mv Neo-0.6.0-Linux linux64-shared-$1
	cd ..
}

cd SDK

# scripts/build_kit_linux64_static.sh "Release"
# scripts/build_kit_linux64_shared.sh "Release"

# scripts/build_kit_linux64_static.sh "Debug"
# scripts/build_kit_linux64_shared.sh "Debug"

# cd build-linux64-static-Release
# tar -xf "Neo-0.6.0-Linux.tar.gz"

# rm -rf linux64-static-Release
# mv Neo-0.6.0-Linux linux64-static-release
# cd ..

# cd build-linux64-shared-Release
# tar -xf "Neo-0.6.0-Linux.tar.gz"

# rm -rf linux64-shared-release
# mv Neo-0.6.0-Linux linux64-shared-release
# cd ..

build_and_extract_packages "Release"
build_and_extract_packages "Debug"

cd ..

