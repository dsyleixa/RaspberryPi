echo "-------------------------------------------------"
echo " update: (local) raspi_sav to SD and USB"
echo "-------------------------------------------------"
echo


mkdir -p /home/pi/raspi_sav
mkdir -p /home/pi/scripts
mkdir -p /home/pi/settings
echo

rsync -vax --delete /home/pi/raspiProgs    /home/pi/raspi_sav  
rsync -vax --delete /home/pi/scripts       /home/pi/raspi_sav  
rsync -vax --delete /home/pi/settings      /home/pi/raspi_sav 


echo
echo "-------------------------------------------------"
echo " update: (local) Akten_mini/Programmierung/raspi "
echo

rsync -vax --delete /home/pi/raspi_sav /home/pi/Akten_mini/
echo
echo

if [ -e /media/pi/USB120DRV/ ]; then

  echo
  echo "-------------------------------------------------"
  echo " update:  /home/pi/raspi_sav to USB drv"
  echo
  echo
  echo "-------------------------------------------------"
  echo " update: (USB) /media/pi/USB120DRV/Akten/Programmierung/raspiProgs "
  echo

  sudo rm -rf /media/pi/USB120DRV/Akten/Programmierung/raspiProgs 
  echo
  mkdir -p /media/pi/USB120DRV/Akten/Programmierung 
  rsync -vax --delete /home/pi/raspi_sav    /media/pi/USB120DRV/Akten  
  rsync -vax --delete /home/pi/raspiProgs   /media/pi/USB120DRV/Akten/Programmierung 
 

  echo
  echo "-------------------------------------------------"
  echo " update: (USB) /media/pi/USB120DRV/Akten_mini "
  echo

  mkdir -p /media/pi/USB120DRV/Akten_mini/Programmierung 
  rsync -vax --delete /home/pi/raspi_sav    /media/pi/USB120DRV/Akten_mini/ 
  rsync -vax --delete /home/pi/raspiProgs   /media/pi/USB120DRV/Akten_mini/Programmierung/



  echo 
  echo "-------------------------------------------------"
  echo "copy to USB Drive succcessful" 
  echo "-------------------------------------------------"
else
  echo
  echo "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"
  echo " The folder /media/pi/USB120DRV/ does not exist"
  echo "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" 
fi

echo
echo finished!
echo
echo press Enter to quit
read reply
exit


