#include "stdafx.h"
#include "input_bitstream.hpp"

#include <iostream>
#include <fstream>
#include <exception>
#include "fast.hpp"

#define LOGGING

namespace webgraph {
using namespace std;


const int ibitstream::GAMMA[] = {
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      7<<8 | 7, 7<<8 | 7, 7<<8 | 8, 7<<8 | 8, 7<<8 | 9, 7<<8 | 9, 7<<8 | 10, 7<<8 | 10, 7<<8 | 11, 7<<8 | 11, 7<<8 | 12, 7<<8 | 12, 7<<8 | 13, 7<<8 | 13, 7<<8 | 14, 7<<8 | 14,
      5<<8 | 3, 5<<8 | 3, 5<<8 | 3, 5<<8 | 3, 5<<8 | 3, 5<<8 | 3, 5<<8 | 3, 5<<8 | 3, 5<<8 | 4, 5<<8 | 4, 5<<8 | 4, 5<<8 | 4, 5<<8 | 4, 5<<8 | 4, 5<<8 | 4, 5<<8 | 4,
      5<<8 | 5, 5<<8 | 5, 5<<8 | 5, 5<<8 | 5, 5<<8 | 5, 5<<8 | 5, 5<<8 | 5, 5<<8 | 5, 5<<8 | 6, 5<<8 | 6, 5<<8 | 6, 5<<8 | 6, 5<<8 | 6, 5<<8 | 6, 5<<8 | 6, 5<<8 | 6,
      3<<8 | 1, 3<<8 | 1, 3<<8 | 1, 3<<8 | 1, 3<<8 | 1, 3<<8 | 1, 3<<8 | 1, 3<<8 | 1, 3<<8 | 1, 3<<8 | 1, 3<<8 | 1, 3<<8 | 1, 3<<8 | 1, 3<<8 | 1, 3<<8 | 1, 3<<8 | 1,
      3<<8 | 1, 3<<8 | 1, 3<<8 | 1, 3<<8 | 1, 3<<8 | 1, 3<<8 | 1, 3<<8 | 1, 3<<8 | 1, 3<<8 | 1, 3<<8 | 1, 3<<8 | 1, 3<<8 | 1, 3<<8 | 1, 3<<8 | 1, 3<<8 | 1, 3<<8 | 1,
      3<<8 | 2, 3<<8 | 2, 3<<8 | 2, 3<<8 | 2, 3<<8 | 2, 3<<8 | 2, 3<<8 | 2, 3<<8 | 2, 3<<8 | 2, 3<<8 | 2, 3<<8 | 2, 3<<8 | 2, 3<<8 | 2, 3<<8 | 2, 3<<8 | 2, 3<<8 | 2,
      3<<8 | 2, 3<<8 | 2, 3<<8 | 2, 3<<8 | 2, 3<<8 | 2, 3<<8 | 2, 3<<8 | 2, 3<<8 | 2, 3<<8 | 2, 3<<8 | 2, 3<<8 | 2, 3<<8 | 2, 3<<8 | 2, 3<<8 | 2, 3<<8 | 2, 3<<8 | 2,
      1<<8 | 0, 1<<8 | 0, 1<<8 | 0, 1<<8 | 0, 1<<8 | 0, 1<<8 | 0, 1<<8 | 0, 1<<8 | 0, 1<<8 | 0, 1<<8 | 0, 1<<8 | 0, 1<<8 | 0, 1<<8 | 0, 1<<8 | 0, 1<<8 | 0, 1<<8 | 0,
      1<<8 | 0, 1<<8 | 0, 1<<8 | 0, 1<<8 | 0, 1<<8 | 0, 1<<8 | 0, 1<<8 | 0, 1<<8 | 0, 1<<8 | 0, 1<<8 | 0, 1<<8 | 0, 1<<8 | 0, 1<<8 | 0, 1<<8 | 0, 1<<8 | 0, 1<<8 | 0,
      1<<8 | 0, 1<<8 | 0, 1<<8 | 0, 1<<8 | 0, 1<<8 | 0, 1<<8 | 0, 1<<8 | 0, 1<<8 | 0, 1<<8 | 0, 1<<8 | 0, 1<<8 | 0, 1<<8 | 0, 1<<8 | 0, 1<<8 | 0, 1<<8 | 0, 1<<8 | 0,
      1<<8 | 0, 1<<8 | 0, 1<<8 | 0, 1<<8 | 0, 1<<8 | 0, 1<<8 | 0, 1<<8 | 0, 1<<8 | 0, 1<<8 | 0, 1<<8 | 0, 1<<8 | 0, 1<<8 | 0, 1<<8 | 0, 1<<8 | 0, 1<<8 | 0, 1<<8 | 0,
      1<<8 | 0, 1<<8 | 0, 1<<8 | 0, 1<<8 | 0, 1<<8 | 0, 1<<8 | 0, 1<<8 | 0, 1<<8 | 0, 1<<8 | 0, 1<<8 | 0, 1<<8 | 0, 1<<8 | 0, 1<<8 | 0, 1<<8 | 0, 1<<8 | 0, 1<<8 | 0,
      1<<8 | 0, 1<<8 | 0, 1<<8 | 0, 1<<8 | 0, 1<<8 | 0, 1<<8 | 0, 1<<8 | 0, 1<<8 | 0, 1<<8 | 0, 1<<8 | 0, 1<<8 | 0, 1<<8 | 0, 1<<8 | 0, 1<<8 | 0, 1<<8 | 0, 1<<8 | 0,
      1<<8 | 0, 1<<8 | 0, 1<<8 | 0, 1<<8 | 0, 1<<8 | 0, 1<<8 | 0, 1<<8 | 0, 1<<8 | 0, 1<<8 | 0, 1<<8 | 0, 1<<8 | 0, 1<<8 | 0, 1<<8 | 0, 1<<8 | 0, 1<<8 | 0, 1<<8 | 0,
      1<<8 | 0, 1<<8 | 0, 1<<8 | 0, 1<<8 | 0, 1<<8 | 0, 1<<8 | 0, 1<<8 | 0, 1<<8 | 0, 1<<8 | 0, 1<<8 | 0, 1<<8 | 0, 1<<8 | 0, 1<<8 | 0, 1<<8 | 0, 1<<8 | 0, 1<<8 | 0,
   };

const int ibitstream::DELTA[] = {
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      8<<8 | 7, 8<<8 | 8, 8<<8 | 9, 8<<8 | 10, 8<<8 | 11, 8<<8 | 12, 8<<8 | 13, 8<<8 | 14, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      4<<8 | 1, 4<<8 | 1, 4<<8 | 1, 4<<8 | 1, 4<<8 | 1, 4<<8 | 1, 4<<8 | 1, 4<<8 | 1, 4<<8 | 1, 4<<8 | 1, 4<<8 | 1, 4<<8 | 1, 4<<8 | 1, 4<<8 | 1, 4<<8 | 1, 4<<8 | 1,
      4<<8 | 2, 4<<8 | 2, 4<<8 | 2, 4<<8 | 2, 4<<8 | 2, 4<<8 | 2, 4<<8 | 2, 4<<8 | 2, 4<<8 | 2, 4<<8 | 2, 4<<8 | 2, 4<<8 | 2, 4<<8 | 2, 4<<8 | 2, 4<<8 | 2, 4<<8 | 2,
      5<<8 | 3, 5<<8 | 3, 5<<8 | 3, 5<<8 | 3, 5<<8 | 3, 5<<8 | 3, 5<<8 | 3, 5<<8 | 3, 5<<8 | 4, 5<<8 | 4, 5<<8 | 4, 5<<8 | 4, 5<<8 | 4, 5<<8 | 4, 5<<8 | 4, 5<<8 | 4,
      5<<8 | 5, 5<<8 | 5, 5<<8 | 5, 5<<8 | 5, 5<<8 | 5, 5<<8 | 5, 5<<8 | 5, 5<<8 | 5, 5<<8 | 6, 5<<8 | 6, 5<<8 | 6, 5<<8 | 6, 5<<8 | 6, 5<<8 | 6, 5<<8 | 6, 5<<8 | 6,
      1<<8 | 0, 1<<8 | 0, 1<<8 | 0, 1<<8 | 0, 1<<8 | 0, 1<<8 | 0, 1<<8 | 0, 1<<8 | 0, 1<<8 | 0, 1<<8 | 0, 1<<8 | 0, 1<<8 | 0, 1<<8 | 0, 1<<8 | 0, 1<<8 | 0, 1<<8 | 0,
      1<<8 | 0, 1<<8 | 0, 1<<8 | 0, 1<<8 | 0, 1<<8 | 0, 1<<8 | 0, 1<<8 | 0, 1<<8 | 0, 1<<8 | 0, 1<<8 | 0, 1<<8 | 0, 1<<8 | 0, 1<<8 | 0, 1<<8 | 0, 1<<8 | 0, 1<<8 | 0,
      1<<8 | 0, 1<<8 | 0, 1<<8 | 0, 1<<8 | 0, 1<<8 | 0, 1<<8 | 0, 1<<8 | 0, 1<<8 | 0, 1<<8 | 0, 1<<8 | 0, 1<<8 | 0, 1<<8 | 0, 1<<8 | 0, 1<<8 | 0, 1<<8 | 0, 1<<8 | 0,
      1<<8 | 0, 1<<8 | 0, 1<<8 | 0, 1<<8 | 0, 1<<8 | 0, 1<<8 | 0, 1<<8 | 0, 1<<8 | 0, 1<<8 | 0, 1<<8 | 0, 1<<8 | 0, 1<<8 | 0, 1<<8 | 0, 1<<8 | 0, 1<<8 | 0, 1<<8 | 0,
      1<<8 | 0, 1<<8 | 0, 1<<8 | 0, 1<<8 | 0, 1<<8 | 0, 1<<8 | 0, 1<<8 | 0, 1<<8 | 0, 1<<8 | 0, 1<<8 | 0, 1<<8 | 0, 1<<8 | 0, 1<<8 | 0, 1<<8 | 0, 1<<8 | 0, 1<<8 | 0,
      1<<8 | 0, 1<<8 | 0, 1<<8 | 0, 1<<8 | 0, 1<<8 | 0, 1<<8 | 0, 1<<8 | 0, 1<<8 | 0, 1<<8 | 0, 1<<8 | 0, 1<<8 | 0, 1<<8 | 0, 1<<8 | 0, 1<<8 | 0, 1<<8 | 0, 1<<8 | 0,
      1<<8 | 0, 1<<8 | 0, 1<<8 | 0, 1<<8 | 0, 1<<8 | 0, 1<<8 | 0, 1<<8 | 0, 1<<8 | 0, 1<<8 | 0, 1<<8 | 0, 1<<8 | 0, 1<<8 | 0, 1<<8 | 0, 1<<8 | 0, 1<<8 | 0, 1<<8 | 0,
      1<<8 | 0, 1<<8 | 0, 1<<8 | 0, 1<<8 | 0, 1<<8 | 0, 1<<8 | 0, 1<<8 | 0, 1<<8 | 0, 1<<8 | 0, 1<<8 | 0, 1<<8 | 0, 1<<8 | 0, 1<<8 | 0, 1<<8 | 0, 1<<8 | 0, 1<<8 | 0,
   };

const int ibitstream::ZETA_3[] = {
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      7<<8 | 7, 7<<8 | 7, 7<<8 | 8, 7<<8 | 8, 7<<8 | 9, 7<<8 | 9, 7<<8 | 10, 7<<8 | 10, 7<<8 | 11, 7<<8 | 11, 7<<8 | 12, 7<<8 | 12, 7<<8 | 13, 7<<8 | 13, 7<<8 | 14, 7<<8 | 14,
      8<<8 | 15, 8<<8 | 16, 8<<8 | 17, 8<<8 | 18, 8<<8 | 19, 8<<8 | 20, 8<<8 | 21, 8<<8 | 22, 8<<8 | 23, 8<<8 | 24, 8<<8 | 25, 8<<8 | 26, 8<<8 | 27, 8<<8 | 28, 8<<8 | 29, 8<<8 | 30,
      8<<8 | 31, 8<<8 | 32, 8<<8 | 33, 8<<8 | 34, 8<<8 | 35, 8<<8 | 36, 8<<8 | 37, 8<<8 | 38, 8<<8 | 39, 8<<8 | 40, 8<<8 | 41, 8<<8 | 42, 8<<8 | 43, 8<<8 | 44, 8<<8 | 45, 8<<8 | 46,
      8<<8 | 47, 8<<8 | 48, 8<<8 | 49, 8<<8 | 50, 8<<8 | 51, 8<<8 | 52, 8<<8 | 53, 8<<8 | 54, 8<<8 | 55, 8<<8 | 56, 8<<8 | 57, 8<<8 | 58, 8<<8 | 59, 8<<8 | 60, 8<<8 | 61, 8<<8 | 62,
      3<<8 | 0, 3<<8 | 0, 3<<8 | 0, 3<<8 | 0, 3<<8 | 0, 3<<8 | 0, 3<<8 | 0, 3<<8 | 0, 3<<8 | 0, 3<<8 | 0, 3<<8 | 0, 3<<8 | 0, 3<<8 | 0, 3<<8 | 0, 3<<8 | 0, 3<<8 | 0,
      3<<8 | 0, 3<<8 | 0, 3<<8 | 0, 3<<8 | 0, 3<<8 | 0, 3<<8 | 0, 3<<8 | 0, 3<<8 | 0, 3<<8 | 0, 3<<8 | 0, 3<<8 | 0, 3<<8 | 0, 3<<8 | 0, 3<<8 | 0, 3<<8 | 0, 3<<8 | 0,
      4<<8 | 1, 4<<8 | 1, 4<<8 | 1, 4<<8 | 1, 4<<8 | 1, 4<<8 | 1, 4<<8 | 1, 4<<8 | 1, 4<<8 | 1, 4<<8 | 1, 4<<8 | 1, 4<<8 | 1, 4<<8 | 1, 4<<8 | 1, 4<<8 | 1, 4<<8 | 1,
      4<<8 | 2, 4<<8 | 2, 4<<8 | 2, 4<<8 | 2, 4<<8 | 2, 4<<8 | 2, 4<<8 | 2, 4<<8 | 2, 4<<8 | 2, 4<<8 | 2, 4<<8 | 2, 4<<8 | 2, 4<<8 | 2, 4<<8 | 2, 4<<8 | 2, 4<<8 | 2,
      4<<8 | 3, 4<<8 | 3, 4<<8 | 3, 4<<8 | 3, 4<<8 | 3, 4<<8 | 3, 4<<8 | 3, 4<<8 | 3, 4<<8 | 3, 4<<8 | 3, 4<<8 | 3, 4<<8 | 3, 4<<8 | 3, 4<<8 | 3, 4<<8 | 3, 4<<8 | 3,
      4<<8 | 4, 4<<8 | 4, 4<<8 | 4, 4<<8 | 4, 4<<8 | 4, 4<<8 | 4, 4<<8 | 4, 4<<8 | 4, 4<<8 | 4, 4<<8 | 4, 4<<8 | 4, 4<<8 | 4, 4<<8 | 4, 4<<8 | 4, 4<<8 | 4, 4<<8 | 4,
      4<<8 | 5, 4<<8 | 5, 4<<8 | 5, 4<<8 | 5, 4<<8 | 5, 4<<8 | 5, 4<<8 | 5, 4<<8 | 5, 4<<8 | 5, 4<<8 | 5, 4<<8 | 5, 4<<8 | 5, 4<<8 | 5, 4<<8 | 5, 4<<8 | 5, 4<<8 | 5,
      4<<8 | 6, 4<<8 | 6, 4<<8 | 6, 4<<8 | 6, 4<<8 | 6, 4<<8 | 6, 4<<8 | 6, 4<<8 | 6, 4<<8 | 6, 4<<8 | 6, 4<<8 | 6, 4<<8 | 6, 4<<8 | 6, 4<<8 | 6, 4<<8 | 6, 4<<8 | 6,
   };

const int ibitstream::BYTEMSB[]  = {
      -1, 0, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 
      4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 
      5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 
      5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 
      6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 
      6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 
      6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 
      6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 
      7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 
      7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 
      7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 
      7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 
      7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
      7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 
      7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 
      7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7
};   


// lire le prochain byte du flux de donnée
int ibitstream::read() {
#ifdef LOGGING
   cerr << "Read called. unget_count = " << unget_count << endl;
#endif

   if ( unget_count > 0 ) 
//      return unget_bytes[ --unget_count ] & 0xFF;
      return unget_bytes.at( --unget_count ) & 0xff;

   if ( past_eof ) 
      return 0;
      
   // simple case
   if ( no_buffer ) {
      int t = is->get();
      if( !is->good() ) {
         assert( overflow );
         past_eof = true;
         return 0;
      } else 
         position++;
#ifdef LOGGING      
      cerr << "About to return byte " << (unsigned int)((unsigned char)t) 
           << " just read from file.\n";
#endif
      return t & 0xFF ;
   }

   // deal with reading into the buffer.
   if ( avail == 0 ) {
      // then the buffer is empty. attempt to fill it again.
      if( is != NULL )
         assert( buffer->size() == buffer->capacity() );
         avail = is->readsome( (char*)&(*buffer).at(0), buffer->size() ); // used to be capacity
#ifdef LOGGING         
      cerr << "==================================================\n";
      cerr << "BUFFER REFILLED; first 50 bytes : \n";
      for( int i = 0; i < 50; i++ ) {
         cerr << utils::int_to_binary( (*buffer).at(0), 8 ) << " ";
         if( i + 1 % 10 == 0 )
            cerr << "\n";
      }
      cerr << "\n";
#endif
      if ( avail == 0 ) {
         if( overflow ) {
            past_eof = true;
            return 0;
         } else {
            throw eof_exception();
         }
      } else {
         position += pos;
         pos = 0;
      }
   }
      
   avail--;

   int retval = (*buffer).at(pos++) & 0xFF;
#ifdef LOGGING
   cerr << "read() - About to return byte " << utils::int_to_binary( retval, 8 )
        << " just read from buffer (pos now = " << pos << ").\n";
#endif
   return retval;
}


// lire des bits depuis le buffer de bits avec possibilité de le remplir, len : nombre de bits à lire/  retourne le nbr de bits lu dans la faible position 
int ibitstream::read_from_current( unsigned int len ) {
//   assert( len <= 8 );
   if ( len == fill ) {
      // We just empty everything.
      read_bits += len;
      fill = 0;
      int retval = current & ( 1 << len ) - 1;
#ifdef LOGGING
      cerr << "\tread_from_current(" << len << ") = " << retval << " "
           << "(current=" << utils::int_to_binary(current,fill) << ")\n";
#endif
      return retval;
   }

   if ( fill == 0 ) {
      current = read();
#ifdef LOGGING
      cerr << "\tread_from_current - just read " << utils::int_to_binary( current, 8 )
           << "into current.\n";
#endif
      fill = 8;
   }

   assert( len <= fill );

   read_bits += len;
   // used to be (unsigned)current on following line.
   int retval = (current >> ( fill -= len )) & (( 1 << len ) - 1);
#ifdef LOGGING
   cerr << "read_from_current(" << len << ") = " << retval << " "
        << "(current=" << utils::int_to_binary(current,fill) << ")\n";
#endif
   return retval;
}

// Lire une séquence de bits :  bits: un vecteur de bytes pour stocker le résultat / len: le nombre de bits à lire
void ibitstream::read( byte bits[], unsigned int len ) {
   if ( fill < 8 ) 
      refill();
      
   if ( len <= fill ) {
      if ( len <= 8 ) {
         bits[ 0 ] = (byte)( read_from_current( len ) << 8 - len );
         return;
      } else {
         bits[ 0 ] = (byte)( read_from_current( 8 ) );
         bits[ 1 ] = (byte)( read_from_current( len - 8 ) << 16 - len );
         return;
      }
   } else {
      int i, j = 0, b;
         
      if ( fill >= 8 ) {
         bits[ j++ ] = (byte)( read_from_current( 8 ) );
         len -= 8;
      } 
         
      const unsigned int shift = fill;

      bits[ j ] = (byte)( read_from_current( shift ) << 8 - shift );
      len -= shift;

      i = len >> 3;
      while( i-- != 0 ) {
         b = read();
         // used to be >>>
         bits[ j ] |= (( b & 0xFF ) >> shift) & 0xFF; // the last & 0xFF is mine.
         bits[ ++j ] = (byte)( b << 8 - shift );
      }
         
      read_bits += len & ~7;

      len &= 7;
      if ( len != 0 ) {
         if ( len <= 8 - shift ) {
            bits[ j ] |= (byte)( read_from_current( len ) << 8 - shift - len );
         } else {
            bits[ j ] |= (byte)( read_from_current( 8 - shift ) );
            bits[ j + 1 ] = (byte)( read_from_current( len + shift - 8 ) << 16 - shift - len );
         }
      }
   }
}

// lit un entier , len : nombre de bits
int ibitstream::read_int( unsigned int len ) {
   int i, x = 0;
      
#ifdef LOGGING
   cerr << "read_int( " << len << " ) called... " << endl;
#endif

   assert( len >= 0 && len <= 32 );

   if ( len <= fill ) {
      int retval = read_from_current( len );
#ifdef LOGGING
      cerr << "read_int_1(" << len << ") = " << retval << "\n";
#endif
      return retval;
   }
      
   len -= fill;
   x = read_from_current( fill );
      
   i = len >> 3;
   while( i-- != 0 ) x = x << 8 | read();
   read_bits += len & ~7;

   len &= 7;

   int retval = ( x << len ) | read_from_current( len );

#ifdef LOGGING
   cerr << "read_int_2( " << len << ") = " << retval << "\n";
#endif

   return retval;
}

// passe 'n' nombre de bits ( ne les lit pas) 
long ibitstream::skip( unsigned long n ) {
#ifdef LOGGING
   cerr << "skip(n)......." << "\n";
#endif

   if ( n <= fill ) {
      assert( n >= 0 );
      fill -= n;
      read_bits += n;
      return n;
   } else {
      const long prev_read_bits = read_bits;
         
      n -= fill;
      read_bits += fill;
      fill = 0;
         
      unsigned long nb = n >> 3;
         
      if ( !no_buffer && nb > avail && nb < avail + buffer->size() ) {
         /* If we can skip by simply filling the buffer and skipping some bytes,
            we do it. Usually the next block has already been fetched by a read-ahead logic. */
         read_bits += ( avail + 1 ) << 3;
         n -= ( avail + 1 ) << 3;
         nb -= avail + 1;
         position += pos + avail;
         pos = avail = 0;
         read();
      }
         
      if ( nb <= avail ) {
         // We skip bytes directly inside the buffer.
         pos += (int)nb;
         avail -= (std::streamoff)nb;
         read_bits += n & ~7;
      } else {
         // No way, we have to pass the byte skip to the underlying stream.
         n -= avail << 3;
         read_bits += avail << 3;
            
         const long to_skip = nb - avail;
         const long prev_pos = is->tellg();
         is->ignore( to_skip );
         const long skipped = is->tellg() - (std::streamoff)prev_pos;
            
         position += ( avail + pos ) + skipped;
         pos = 0;
         avail = 0;
            
         read_bits += skipped << 3;
            
         if ( skipped != to_skip ) 
            return read_bits - prev_read_bits;
      }
         
      const int residual = (int)( n & 7 );
      if ( residual != 0 ) {
         current = read();
         fill = 8 - residual;
         read_bits += residual;
      }
      return read_bits - prev_read_bits;
   }
}

// setter de position 
void ibitstream::set_position( unsigned long position ) {
#ifdef LOGGING
   cerr << "set_position called...\n";
#endif
   
   assert( position >= 0 );

   // removed "unsigned"
   const int delta = ( position >> 3 ) - ( this->position + pos );

   // Once graph memory is attached, we should be looking there.
   // But for some reason, we're looking in the (null) istream instead.
      
   int pos = (int)this->pos;

   if ( delta <= avail && delta >= - pos ) {
      // We can reposition just by moving into the buffer.
      avail -= delta;
      pos += delta;
      fill = unget_count = 0;
   } else {
      flush();
      is->seekg( position >> 3 );
      this->position = position >> 3;
   }

   int residual = (int)( position & 7 );

   if ( residual != 0 ) {
      current = read();
      fill = 8 - residual;
   }
}

// lit un entier codé en unary 
int ibitstream::read_unary() {
   int x;

   if ( fill < 8 ) 
      refill();
      
#if 1
   cerr << "current: " << current << endl
        << "fill: " << fill << endl;
#endif

   unsigned int currentLeftAligned = current << ( 16 - fill ) & 0xFFFF;
      
   if ( currentLeftAligned != 0 ) {
      // used to be >>>
      if ( ( currentLeftAligned & 0xFF00 ) != 0 ) 
         x = 7 - BYTEMSB[ currentLeftAligned >> 8 ];
      else 
         x = 15 - BYTEMSB[ currentLeftAligned & 0xFF ];

      read_bits += x + 1;
      fill -= x + 1;
#if 1
      cerr << "1. read_unary() = " << x << endl;
#endif
      return x;
   }
      
   x = fill;
   while( ( current = read() ) == 0 ) 
      x += 8;
   x += 7 - ( fill = BYTEMSB[ current ] );
   read_bits += x + 1;

#if 1
   cerr << "2. read_unary() = " << x << endl;
#endif
   return x;
}

// lit un entier codé en gamma
int ibitstream::read_gamma() {
   if ( fill < 8 ) 
      refill();

   if ( fill >= 8 ) {
      const int pre_comp = GAMMA[ current >> ( fill == 16 ? 8 : ( fill & 7 ) ) & 0xFF ];
      if ( pre_comp != 0 ) {
         read_bits += pre_comp >> 8;
         fill -= pre_comp >> 8;
#ifdef LOGGING
         cerr << "read_gamma() = " << (pre_comp & 0xFF) << endl;
#endif
         return pre_comp & 0xFF;
      }
   }
      
   const int msb = read_unary();

   int retval = ( ( 1 << msb ) | read_int( msb ) ) - 1;

#ifdef LOGGING
   cerr << "read_gamma() = " << retval << endl;
#endif

   return retval;
}
   
// lit un entier codé en delta
int ibitstream::read_delta() {
   if ( fill < 8 ) 
      refill();
   if ( fill >= 8 ) {
      const int pre_comp = DELTA[ current >> ( fill == 16 ? 8 : ( fill & 7 ) ) & 0xFF ];
      if ( pre_comp != 0 ) {
         read_bits += pre_comp >> 8;
         fill -= pre_comp >> 8;
         int retval = pre_comp & 0xFF;
#ifdef LOGGING
         cerr << "\tread_delta returning " << retval << endl;
#endif
         return retval;
      }
   }

   const int msb = read_gamma();

   int retval = ( ( 1 << msb ) | read_int( msb ) ) - 1;

#ifdef LOGGING
   cerr << "\tread_delta returning " << retval << endl;
#endif

   return retval;
}

// lit un entier codé en unary 
int ibitstream::read_zeta( int k ) {
   assert( k > 0 );
      
   if ( k == 3 ) {
      if ( fill < 8 ) 
         refill();
      if ( fill >= 8 ) {
         const int pre_comp = ZETA_3[ current >> ( fill == 16 ? 8 : ( fill & 7 ) ) & 0xFF ];
         if ( pre_comp != 0 ) {
            read_bits += pre_comp >> 8;
            fill -= pre_comp >> 8;
#if 1
            cerr << "\t1. read_zeta returning " << (pre_comp & 0xFF) << endl;
#endif
            return pre_comp & 0xFF;
         }
      }
   }

   const int h = read_unary();
   const int left = 1 << h * k;
   const int m = read_int( h * k + k - 1 );
   if ( m < left ) {
      int retval = m + left - 1;
#if 1
      cerr << "\t2. read_zeta returning " << retval << endl;
#endif
      return retval;
   }

   int retval = ( m << 1 ) + read_bit() - 1;

#if 1
   cerr << "\t3. read_zeta returning " << retval << endl;
#endif
   
   return retval;
}

// lit un entier codé en nibble
int ibitstream::read_nibble() {
   int b;
   int x = 0;
      
   do {
      x <<= 3;
      b = read_bit();
      x |= read_int( 3 );
   } while( b == 0 );

#ifdef LOGGING
   cerr << "\tread_nibble returning " << x << endl;
#endif
      
   return x;
}
	





}
