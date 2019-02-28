#!/bin/bash

SYSTEMDLLS=(
"KERNEL32.dll"
"msvcrt.dll"
"OPENGL32.dll"
)

BINDIR=$(dirname "$1")

function isSystemDLL () {
	for i in "${SYSTEMDLLS[@]}"; do
		if [ "$i" == "$1" ]; then
			return 0
		fi
	done
	return 1
}

function findAndCopyDLL() {
	if isSystemDLL "$1"; then
		echo "$1 is system DLL, ignored."
		return 0
	fi
	FILE=$(which "$1")
	if [ -f "$FILE" ]; then
		cp "$FILE" "$BINDIR"
		echo "Copied $FILE to $BINDIR"
		return 0
	fi

	return 1
}

function copyForOBJ() {
	dlls=$(objdump -p "$1" | grep "DLL Name:" | sed -e "s/\t*DLL Name: //g")
	while read -r filename; do
		findAndCopyDLL "$filename" || echo "Unable to find $filename"
	done <<< "$dlls"
}

copyForOBJ "$1"
