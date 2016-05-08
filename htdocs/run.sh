#!/bin/bash
while :
do
         sleep 3600
         touch result.html
         ./weather > result.html
         cat result.html >  index.html
done
