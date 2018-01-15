#!/bin/bash
for file in *.bmp; do convert "$file" "${file/.bmp/.png}"; done
