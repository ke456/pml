#!/bin/bash

for i in 100 200 400 800 1600 3200 6400 12000 24000 48000; do
	./test_charpolymod $i 0.3 | tail -1 >> charpoly.dat
done
