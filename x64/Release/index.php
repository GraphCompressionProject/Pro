<?php
// Affiche le nom d'utilisateur qui fait tourner le processus php/http
// (sur un syst�me ayant "whoami" dans le chemin d'ex�cutables)
 exec('BVTest.exe 4 Test_stat DSM 3 > webdictionary.txt');
 $myfile = fopen("webdictionary.txt", "r") or die("Unable to open file!");
 echo fread($myfile,filesize("webdictionary.txt"));
 fclose($myfile);
?>
