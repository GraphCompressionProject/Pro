#ifndef FAST_HPP
#define FAST_HPP

#include <string>
#include <sstream>

namespace utils {

// mapping entre un entier signé avec un entier positif / x : entier négatif => -2x -1  / x : entier positif => 
int int2nat( int x );

// mapping inverse pour récupérer l'entier signé négatif ou positif 
int nat2int( int x );

//convertir un octet (int) en chaine binaire 
std::string byte_to_binary( int x );

//convertir  un entier (long) en chaine binaire
std::string int_to_binary( long x, int len );

//convertir un octer en hexa
std::string byte_as_hex( int byte );

//tostring
template<class T> std::string to_string( const T& thing ) {
   std::ostringstream o; 
   o << thing;
   return o.str();
}


}

#endif

