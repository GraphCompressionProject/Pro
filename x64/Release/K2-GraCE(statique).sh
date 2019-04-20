#!/bin/bash
# init
function pause(){
   echo ""
   read -p "$*"
   echo ""
}



echo -e "\e[32m=============================================================="
echo -e "=============== K2-Tree -algorithme de Base- ================="
echo -e "===== Construction a partir de la matrice d'adjacence ========"
echo -e "======== Graph : web-polblogs , orientee : oui, k:2 =========="
echo -e "==============================================================\033[0m"
./BVTest.exe 3 web-polblogs K2_Matrix 2 1
pause 'Press [Enter] key to continue...'


echo -e "\e[32m=============================================================="
echo -e "=============== K2-Tree -algorithme de Base- ================="
echo -e "===== Construction a partir de la liste d'adjacence =========="
echo -e "======== Graph : web-polblogs , orientee : oui, k:2 =========="
echo -e "==============================================================\033[0m"
./BVTest.exe 3 web-polblogs K2_List 2 1
pause 'Press [Enter] key to continue...'

echo -e "\e[32m=============================================================="
echo -e "=============== K2-Tree -algorithme de Base- ================="
echo -e "===== Construction a partir de du graphe TNGraph ============="
echo -e "======== Graph : web-polblogs , orientee : oui, k:2 =========="
echo -e "==============================================================\033[0m"
./BVTest.exe 3 web-polblogs Graph 2 1
pause 'Press [Enter] key to continue...'

echo -e "\e[32m=============================================================="
echo -e "=============== K2-Tree -algorithme de Base- ================="
echo -e "===== Construction a partir de du graphe TNGraph ============="
echo -e "======== Graph : web-polblogs , orientee : oui, k:2 =========="
echo -e "======================== Ordre: BFS ==========================\033[0m"
./BVTest.exe 5 web-polblogs 2 1
pause 'Press [Enter] key to continue...'

echo -e "\e[32m=============================================================="
echo -e "=============== K2-Tree -algorithme de Base- ================="
echo -e "===== Construction a partir de du graphe TNGraph ============="
echo -e "======== Graph : web-polblogs , orientee : oui, k:2 =========="
echo -e "======================== Ordre: DFS ==========================\033[0m"
./BVTest.exe 5 web-polblogs 2 2
pause 'Press [Enter] key to continue...'

echo -e "\e[32m=============================================================="
echo -e "=============== K2-Tree -algorithme de Base- ================="
echo -e "===== Construction a partir de du graphe TNGraph ============="
echo -e "======== Graph : web-polblogs , orientee : oui, k:2 =========="
echo -e "======================== Ordre: Gray =========================\033[0m"
./BVTest.exe 5 web-polblogs 2 3
pause 'Press [Enter] key to continue...'

echo -e "\e[32m=============================================================="
echo -e "=============== K2-Tree -algorithme de Base- ================="
echo -e "===== Construction a partir de du graphe TNGraph ============="
echo -e "======== Graph : web-polblogs , orientee : oui, k:2 =========="
echo -e "======================== Ordre: Lexicale =====================\033[0m"
./BVTest.exe 5 web-polblogs 2 4
pause 'Press [Enter] key to continue...'

echo -e "\e[32m=============================================================="
echo -e "=============== K2-Tree -algorithme de Base- ================="
echo -e "===== Construction a partir de du graphe TNGraph ============="
echo -e "======== Graph : web-polblogs , orientee : oui, k:2 =========="
echo -e "======================== Ordre: aleatoire ====================\033[0m"
./BVTest.exe 5 web-polblogs 2 5
pause 'Press [Enter] key to continue...'

