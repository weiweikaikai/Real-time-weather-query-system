#!/bin/bash
while :
do
         sleep 10
         touch result.html
         ./weather > result.html
         cat result.html >  index.html
done