# find path -name 
# https://www.linux-magazin.de/ausgaben/2020/01/best-practices-12/

# start search from root, correct up/lo case [drop errors]
# find / -name $1 2>/dev/null 

# start search from root, ignore up/lo case [drop errors]
find / -iname $1 2>/dev/null