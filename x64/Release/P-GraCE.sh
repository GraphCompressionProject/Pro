#!/bin/bash
# init
function pause(){
   echo ""
   read -p "$*"
   echo ""
}



echo -e "\e[32m=============================================================="
echo -e "============= Motifs dans la matrice d'adjacence===================="
echo -e "===== Graph : web-polblogs, Motif : 1, taille du motif: 32 ========="
echo -e "==============================================================\033[0m"
./BVTest.exe 4 web-polblogs GCUPM 32 0
pause 'Press [Enter] key to continue...'


echo -e "\e[32m=============================================================="
echo -e "============= Motifs dans la matrice d'adjacence===================="
echo -e "===== Graph : web-polblogs, Motif : 2, taille du motif: 32 ========="
echo -e "==============================================================\033[0m"
./BVTest.exe 4 web-polblogs GCUPM 32 1
pause 'Press [Enter] key to continue...'


echo -e "\e[32m=============================================================="
echo -e "============= Motifs dans la matrice d'adjacence===================="
echo -e "===== Graph : web-polblogs, Motif : 3, taille du motif: 32 ========="
echo -e "==============================================================\033[0m"
./BVTest.exe 4 web-polblogs GCUPM 32 2
pause 'Press [Enter] key to continue...'


echo -e "\e[32m=============================================================="
echo -e "============= Beam Search avec evaluation MDL ======================"
echo -e "=================== Graph : de teste ==============================="
echo -e "==============================================================\033[0m"
./BVTest.exe 4 web-polblogs Subdue -compress "C:\Users\hafsa\Desktop\Pro\BVTest\data\graphs\sample.g"
pause 'Press [Enter] key to continue...'

