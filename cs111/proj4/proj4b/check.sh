#NAME:Andrew Ding
#EMAIL:andrewxding@ucla.edu
#ID:504748356

./lab4b --period=2 --scale="C" --log="LOGFILE" <<-EOF
SCALE=F
SCALE=C
STOP
START
OFF
EOF
echo "args correct handled"
if [ $? -ne 0 ]
then
	echo "Test FAILED"
else
	echo "Test PASSED"
fi
# created file test
if [ ! -s LOGFILE ]
then
	echo "Test FAILED"
else
	echo "Test PASSED"
fi
# logged command test
echo "stop command test"
grep "STOP" LOGFILE &> /dev/null; \
if [ $? -ne 0 ]
then
	echo "Test FAILED"
else
	echo "Test PASSED"
fi
# logged command test
echo "start command test"
grep "START" LOGFILE &> /dev/null; \
if [ $? -ne 0 ]
then
	echo "Test FAILED"
else
	echo "Test PASSED"
fi
# SHUTDOWN logged test
grep "SHUTDOWN" LOGFILE &> /dev/null; \
if [ $? -ne 0 ]
then 
	echo "Test FAILED"
else
	echo "Test PASSE"
fi
# temperature format test
echo "Temperature format test"
egrep '[0-9][0-9]:[0-9][0-9]:[0-9][0-9] [0-9][0-9].[0-9]' LOGFILE &> /dev/null; \
if [ $? -ne 0 ]
then 
	echo "Test FAILED"
else
	echo "Test PASSED"
fi
rm -f LOGFILE
# test for invalid command
echo "invalid command test"
./lab4b --period=2 --scale="C" --log="LOGFILE" <<-EOF
AAA
EOF
if [ $? -eq 0 ]
then
	echo "Test FAILED"
else
	echo "Test PASSED"
fi
rm -f LOGFILE

#check bad arg
echo "bad arg test"
echo | ./lab4b --badarg &> /dev/null; 
if [[ $? -ne 1 ]]; then 
	echo "Test FAILED"; 
else 
	echo "Test PASSED"; 
fi