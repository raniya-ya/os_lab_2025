#!/bin/bash
i=0
sum=0
for average in "$@"; do
	sum=$(($sum + ${average}))
	i=$(($i + 1))
done

echo "колво чисел : $i"
echo  $(($sum / $i))

