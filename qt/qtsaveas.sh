#!/bin/bash   
# qtsaveas.sh
# qt5 creator: save project as...
# start from current project folder!

# ask for new project name
read -p "Enter new project name: " -e -i "${PWD##*/}" xxxx  
echo "your new project name: $xxxx"       # echo entered name
while [ -e ../$xxxx/ ]  # repeat while dir xxxx already exists
do
echo "name already exists"
read -p "Enter new project name: " -e -i "${PWD##*/}" xxxx  
done

mkdir ../$xxxx/     # make new destination dir by entered name
cp  * ../$xxxx/     # copy all source files to new destination dir 

rm ../$xxxx/*.pro.user                 # delete .pro.user file in destination
mv ../$xxxx/*.pro  ../$xxxx/$xxxx.pro  # rename old .pro project file name by new name 

cd ../$xxxx/    # go to new dir
ls -l           # list all project files in new dir
