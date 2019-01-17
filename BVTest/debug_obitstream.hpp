#include "output_bitstream.hpp"
#include <fstream>
#include <vector>
#include <boost/shared_ptr.hpp>
#include <algorithm>

// Un debugger pour les objets output_bitstream 
namespace webgraph {

   class debug_obitstream : public obitstream {
      obitstream& obs;
      std::ofstream& out;

public:
 
	// créer un debugger de obitstream
   debug_obitstream( obitstream& os, std::ofstream& o ) : obs(os), out(o) {
      out << "[";
   }
   
   void flush() {
      out << " |";
      obs.flush();
   }

   int align() {
      out << " |";
      return obs.align();
   }

private:
   static std::string byte_to_binary( int x ) {
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
   static std::string int_to_binary( long x, int len ) {
      std::string s;

      for( int i = 0 ; i < 64; i++ ) {
         s += (char)( '0' + ( x % 2 ) );
         x >>= 1;
      }
   
      s = s.substr( 0, len );
      std::reverse( s.begin(), s.end() );

      return s;
   }


public:
    int write_int( int x, int len ) {
       out << " {" << int_to_binary( x, len ) << "}" << "\n";
       return obs.write_int( x, len );
    }
   int write_unary( int x ) {
      out << " {U:" << x << "}" << "\n";
      return obs.write_unary( x );
   }
   int write_gamma( int x ) {
      out << " {g:" << x << "}" << "\n";
      return obs.write_gamma( x );
   }
   int write_delta( int x ) {
      out << " {d:" << x << "}" << "\n";
      return obs.write_delta( x );
   }
   int write_zeta( int x, int k ) {
      out << " {z" << k << ":" << x << "}" << "\n";
      return obs.write_zeta( x, k );
   }
};

}
