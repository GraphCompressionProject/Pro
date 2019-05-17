#!/bin/bash
# init
function pause(){
   echo ""
   read -p "$*"
   echo ""
}

echo -e "\e[32m=============================================================="
echo -e "============= bio ===================="

./BVTest.exe 5 bio k2 2 0

echo -e "======================================="
./BVTest.exe 5 bio k2 2 1
echo -e "======================================="
./BVTest.exe 5 bio k2 2 2
echo -e "======================================="
./BVTest.exe 5 bio k2 2 3
echo -e "======================================="
./BVTest.exe 5 bio k2 2 4
echo -e "======================================="



echo -e "\e[32m=============================================================="
echo -e "============= web-edu===================="


./BVTest.exe 5 web-edu k2 2 0

echo -e "======================================="
./BVTest.exe 5 web-edu k2 2 1
echo -e "======================================="
./BVTest.exe 5 web-edu k2 2 2
echo -e "======================================="
./BVTest.exe 5 web-edu k2 2 3
echo -e "======================================="
./BVTest.exe 5 web-edu k2 2 4
echo -e "======================================="


echo -e "\e[32m=============================================================="
echo -e "============= web-polblogs===================="


./BVTest.exe 5 web-polblogs k2 2 0

echo -e "======================================="
./BVTest.exe 5 web-polblogs k2 2 1
echo -e "======================================="
./BVTest.exe 5 web-polblogs k2 2 2
echo -e "======================================="
./BVTest.exe 5 web-polblogs k2 2 3
echo -e "======================================="
./BVTest.exe 5 web-polblogs k2 2 4
echo -e "======================================="

echo -e "\e[32m=============================================================="
echo -e "============= wiki-Vote ===================="


./BVTest.exe 5 wiki-Vote k2 2 0

echo -e "======================================="
./BVTest.exe 5 wiki-Vote k2 2 1
echo -e "======================================="
./BVTest.exe 5 wiki-Vote k2 2 2
echo -e "======================================="
./BVTest.exe 5 wiki-Vote k2 2 3
echo -e "======================================="
./BVTest.exe 5 wiki-Vote k2 2 4
echo -e "======================================="



pause 'Press [Enter] key to continue...'
pause 'Press [Enter] key to continue...'
pause 'Press [Enter] key to continue...'
pause 'Press [Enter] key to continue...'