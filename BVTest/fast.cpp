#include "stdafx.h"
#include <string>
#include <algorithm>
#include <sstream>

namespace utils {


// mapping entre un entier signé avec un entier positif / x : entier négatif => -2x -1  / x : entier positif => 
int int2nat( int x ) {
   return x >= 0 ? x << 1 : ( -x << 1 ) - 1;
}

// mapping inverse pour récupérer l'entier signé négatif ou positif 
int nat2int( int x ) {
   return x % 2 == 0 ? x >> 1 : -( ( x + 1 ) >> 1 );
}

//convertir un octet (int) en chaine binaire 
std::string byte_to_binary( int x ) {
  std::string s;
  
  for( int i = 0 ; i < 8; i++ ) {
    char str[] = {0,0};
    str[0] = char( '0' + (x % 2) );
    
    s += str;

    x >>= 1;
  }
  std::reverse(s.begin(), s.end());
  return s;
}

//convertir  un entier (long) en chaine binaire
std::string int_to_binary( long x, int len ) {
  std::string s;
  for( int i = 0 ; i < 64; i++ ) {
    s += (char)( '0' + ( x % 2 ) );
    x >>= 1;
      }
  s = s.substr( 0, len );
  std::reverse( s.begin(), s.end() );

  for( int i = s.size() - 4; i > 0; i -= 4 ) {
     s.insert( i, " " ); 
  }
  return s;
}

//convertir un octer en hexa
std::string byte_as_hex( int b ) {
   using namespace std;
   ostringstream oss;
   oss << hex << b;
   string str = oss.str().substr(0,2);
   return str;
}


}
