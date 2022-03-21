#!/bin/bash

counter=1
while [ $counter -le 50 ]
do
	./voting 7 5
	((counter++))
done

echo All done
