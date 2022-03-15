#!/bin/sh
ps -ef | grep python | cut -c 9-15| xargs kill -s 9
python3 for.py > logs/for.log & python3 for2.py > logs/for2.log & python3 for3.py > logs/for3.log for4.py > logs/for4.log