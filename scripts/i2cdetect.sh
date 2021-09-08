echo "ls -l /dev/i2c*"
ls -l /dev/i2c*

echo

echo "i2cdetect -y 1 "
i2cdetect -y 1 

echo

echo "i2cdetect -y 0 "
i2cdetect -y 0 

echo
echo "press Enter"
read reply
