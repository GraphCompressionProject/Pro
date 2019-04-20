#!/bin/bash
# init
function pause(){
   echo ""
   read -p "$*"
   echo ""
}

echo -e "\e[32m=============================================================="
echo -e "=============== K2-Tree -dynamique- ================="
echo -e "===== Construction a partir de la matrice d'adjacence ========"
echo -e "======== Graph : teste , orientee : oui, k:2 =========="
echo -e "==============================================================\033[0m"
./BVTest.exe 3 Test IK2 2 1
pause 'Press [Enter] key to continue...'

echo -e "\e[32m=============================================================="
echo -e "=============== K2-Tree -dynamique- ================="
echo -e "===== Construction a partir de la matrice de difference ========"
echo -e "======== Graph : teste , orientee : oui, k:2 =========="
echo -e "==============================================================\033[0m"
./BVTest.exe 3 Test DIK2 2 1
pause 'Press [Enter] key to continue...'



