echo "-------------------------------------------------"
echo " backup raspi libraries to /usr/local/include "
echo "-------------------------------------------------"
echo


rsync -vax  /home/pi/programs/libraries /usr/local/include


echo
echo finished!
echo
echo press Enter to quit
read reply


