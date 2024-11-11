#!/bin/bash

#do sudo ./main -p 80 before running this script

#Testing /static
echo "-->Testing /static..."
curl -v http://localhost:80/static/images/gudetama.png -o test_gudetama.png
echo ""
curl -v http://localhost:80/static/docs/sample.txt

echo ""
echo ""

#Testing /stats
echo "-->Testing /stats..."
curl -v http://localhost:80/stats

echo ""
echo ""

#Testing /calc
echo "-->Testing /calc..."
curl -v "http://localhost:80/calc?a=10&b=20"