#include "stdafx.h"
#include "output_bitstream.hpp"
#include <iostream>

namespace webgraph 
{
	
using namespace std;
#ifndef CONFIG_FAST
using namespace logs;
#endif


// écrit un byte dans le flux de sortie 
void obitstream::write( const int b ) {
#ifndef CONFIG_FAST
   logger( "obs" ) << LEVEL_DEBUG << "Write called on byte " << b << "\n";
#endif

   if ( avail-- == 0 ) {
      assert( os != NULL );
         
      if ( no_buffer ) {
         os->write( (char*)&b, sizeof(b) );
         position++;
         avail = 0;
         return;
      }
      
      // just to make the following a little cleaner..
      vector<byte>& buf = *buffer;
      
      os->write( (char*)&buf[0], buf.size() );
      position += buf.size();
      avail = buf.size() - 1;
      pos = 0;
   }
      
   (*buffer)[ pos++ ]  = (byte)b;
}


// écrit des bits dans le buffer de bit, possibilité de vider le buffer, b : les bits à écrire, len : le nombre de bits à écrire
int obitstream::write_in_current( const int b, const int len ) {
#ifndef CONFIG_FAST
   logger("obs") << LEVEL_DEBUG << "write_in_current( " << b << ", " << len << " )\n";
#endif
   current |= ( b & ( ( 1 << len ) - 1 ) ) << ( free -= len );
   if ( free == 0 ) {
      write( current );
      free = 8;
      current = 0;
   }
      
   written_bits += len;
   return len;
}

// setter de position 
void obitstream::set_position( const long position ) {
#ifndef CONFIG_FAST
   logger( "obs" ) << LEVEL_DEBUG << "set_position( " << position << " )\n";
#endif
   assert( position >= 0 );
   assert( ( position & 7 ) == 0 );
      
   if ( wrapping ) {
      assert( (unsigned long)position <= buffer->size() );
      flush();
      free = 8;
      pos = (int)position;
      avail = buffer->size() - pos;
   } else {
      flush();
      if ( position >> 3 != this->position ) 
         os->seekp( this->position = position >> 3 );
   }
}


// écrit une séquence de bits, en commençant par un offset donné 
int obitstream::write( const byte bits[], const int offset, const int len ) {
#ifndef CONFIG_FAST
   logger( "obs" ) << LEVEL_DEBUG << "write( bits, offset, len )\n";
#endif

   const int initial = 8 - ( offset & 0x7 );
   if ( initial == 8 ) 
      return write_byte_offset( bits, offset / 8, len );
   if ( len <= initial ) 
      /// used to be >>>
      return write_int( ( 0xFF & bits[ offset / 8 ] ) >> ( initial - len ), len );
   else
      return write_int( bits[ offset / 8 ], initial ) + 
         write_byte_offset( bits, offset / 8 + 1, len - initial );
}



//écrit une séquence de bits, en commençant par un offset donné 
int obitstream::write_byte_offset( const byte bits[],  const int offset, int len ) {
#ifndef CONFIG_FAST
   logger( "obs" ) << LEVEL_DEBUG << "write_byte_offset( bits, offset, len )\n";
#endif

//    	for( int i = 0; i < len/8; i++ )
//    		cerr << (int)bits[i] << " ";
   		
//    	cerr << endl;
      
   if ( len == 0 ) return 0;
   if ( len <= free ) {
      /// used to be >>>
      // this is okay because bits[] is unsigned, so 0's will be shifted in
      return write_in_current( bits[ offset ] >> 8 - len, len );
   }
   else {
      const int shift = free;
      int i, j;
         
//      cerr << "shift = " << shift << endl;
         
      // used to be >>>
      write_in_current( bits[ offset ] >> 8 - shift, shift );
         
      len -= shift;
         
      j = offset;
      i = len >> 3;
      while( i-- != 0 ) {
         // used to be >>>
         write( bits[ j ] << shift | ( bits[ j + 1 ] & 0xFF ) >> 8 - shift );
         written_bits += 8;
         j++;
      }
         
      const int q = len & 7;
      if ( q != 0 ) {
         if ( q <= 8 - shift ) {
            /// used to be >>>
            write_in_current( bits[ j ] >> 8 - shift - q, q );
         }
         else {
            write_in_current( bits[ j ], 8 - shift );
            /// used to be >>>
            write_in_current( bits[ j + 1 ] >> 16 - q - shift, q + shift - 8 );
         }
      }
         
      return len + shift;
   }
}


// écrit un entier 
int obitstream::write_int( int x, const int len ) {
#ifndef CONFIG_FAST
   logger( "obs" ) << LEVEL_DEBUG << "write_int( " << x << ", " << len << " )\n";
#endif

   assert( len >= 0 && len <= 32 );

   if ( len <= free ) 
      return write_in_current( x, len );

   const int q = ( len - free ) & 7, blocks = ( len - free ) >> 3;
   int i = blocks;

   if ( q != 0 ) {
      temp_buffer[ blocks ] = (byte)x;
      x >>= q;
   }

   while( i-- != 0 ) {
      temp_buffer[ i ] = (byte)x;
      // used to be >>>
      x >>= 8;
   }

   write_in_current( x, free );

   for( i = 0; i < blocks; i++ ) 
//      write( &temp_buffer[ i ], sizeof( temp_buffer[i] )  );
      write( temp_buffer[i] );

   written_bits += blocks << 3;

   if ( q != 0 ) 
      write_in_current( temp_buffer[ blocks ], q );

   return len;
}

// retourne le bit le plus significatif d'un entier 
int obitstream::most_significant_bit(const int x) {
	return
		(x < 1 << 15 ?
		(x < 1 << 7 ?
			(x < 1 << 3 ?
			(x < 1 << 1 ?
				(x < 1 << 0 ?
					x < 0 ? 31 : -1 /* 6 */
					:
					0 /* 5 */
					)
				:
				(x < 1 << 2 ?
					1 /* 5 */
					:
					2 /* 5 */
					)
				)
				:
				(x < 1 << 5 ?
				(x < 1 << 4 ?
					3 /* 5 */
					:
					4 /* 5 */
					)
					:
					(x < 1 << 6 ?
						5 /* 5 */
						:
						6 /* 5 */
						)
					)
				)
			:
	(x < 1 << 11 ?
		(x < 1 << 9 ?
		(x < 1 << 8 ?
			7 /* 5 */
			:
			8 /* 5 */
			)
			:
			(x < 1 << 10 ?
				9 /* 5 */
				:
				10 /* 5 */
				)
			)
		:
		(x < 1 << 13 ?
		(x < 1 << 12 ?
			11 /* 5 */
			:
			12 /* 5 */
			)
			:
			(x < 1 << 14 ?
				13 /* 5 */
				:
				14 /* 5 */
				)
			)
		)
		)
			:
	(x < 1 << 23 ?
		(x < 1 << 19 ?
		(x < 1 << 17 ?
			(x < 1 << 16 ?
				15 /* 5 */
				:
				16 /* 5 */
				)
			:
			(x < 1 << 18 ?
				17 /* 5 */
				:
				18 /* 5 */
				)
			)
			:
			(x < 1 << 21 ?
			(x < 1 << 20 ?
				19 /* 5 */
				:
				20 /* 5 */
				)
				:
				(x < 1 << 22 ?
					21 /* 5 */
					:
					22 /* 5 */
					)
				)
			)
		:
	(x < 1 << 27 ?
		(x < 1 << 25 ?
		(x < 1 << 24 ?
			23 /* 5 */
			:
			24 /* 5 */
			)
			:
			(x < 1 << 26 ?
				25 /* 5 */
				:
				26 /* 5 */
				)
			)
		:
		(x < 1 << 29 ?
		(x < 1 << 28 ?
			27 /* 5 */
			:
			28 /* 5 */
			)
			:
			(x < 1 << 30 ?
				29 /* 5 */
				:
				30 /* 5 */
				)
			)
		)
		)
		);
}


//écrit un entier en utilisant le codage unary
int obitstream::write_unary( int x ) {
#ifndef CONFIG_FAST
   logger( "obs" ) << LEVEL_DEBUG << "write_unary( " << x << " )\n";
#endif

   assert( x >= 0 );
      
   if ( x < free ) {
#ifndef CONFIG_FAST
      logger("obs") << LEVEL_EVERYTHING << "Doing simple unary write.\n";
#endif
      return write_in_current( 1, x + 1 );
   }

#ifndef CONFIG_FAST      
   logger("obs") << LEVEL_EVERYTHING << "Doing more complicated unary write.\n";
#endif

   const int shift = free;
   x -= shift;
      
   written_bits += shift;
   write( current );
   free = 8;
   current = 0;
      
   int i = x >> 3;
      
   written_bits += ( x & 0x7FFFFFF8 );
      
   while( i-- != 0 ) 
      write( 0 );
      
   write_in_current( 1, ( x & 7 ) + 1 );
      
   return x + shift + 1;
}

//écrit un entier en utilisant le codage gamma
int obitstream::write_gamma( int x ) {
#ifndef CONFIG_FAST
   logger( "obs" ) << LEVEL_DEBUG << "write_gamma( " << x << " )\n";
#endif
   assert( x >= 0 );
   
   const int msb = most_significant_bit( ++x );

#ifndef CONFIG_FAST
   logger("obs") << LEVEL_EVERYTHING << "\tmsb = " << msb << "\n";
#endif

   const int l = write_unary( msb );
   return l + ( msb != 0 ? write_int( x, msb ) : 0 );
}

//écrit un entier en utilisant le codage delta
int obitstream::write_delta( int x ) {
#ifndef CONFIG_FAST
   logger( "obs" ) << LEVEL_DEBUG << "write_delta( " << x << " )\n";
#endif

   assert( x >= 0 );
   
   const int msb = most_significant_bit( ++x );
   const int l = write_gamma( msb );
   return l + ( msb != 0 ? write_int( x, msb ) : 0 );
}

//écrit un entier en utilisant le codage zeta
int obitstream::write_zeta( int x, const int k ) {
#ifndef CONFIG_FAST
   logger( "obs" ) << LEVEL_DEBUG << "write_zeta( " << x << ", " << k << " )\n";
#endif

   assert( x >= 0 );
   assert( k > 0 );

   const int msb = most_significant_bit( ++x );
   const int h = msb / k;
   const int l = write_unary( h );
   const int left = 1 << h * k;
   return l + ( x - left < left 
                ? write_int( x - left, h * k + k - 1 ) 
                : write_int( x, h * k + k ) );
}

//écrit un entier en utilisant le codage nibble
int obitstream::write_nibble( const int x ) {
#ifndef CONFIG_FAST
   logger( "obs" ) << LEVEL_DEBUG << "write_nibble( " << x << " )\n";
#endif 

   assert( x > 0 );

   if ( x == 0 ) 
      return write_int( 8, 4 );

   const int msb = most_significant_bit( x );
   int h = msb / 3;
   do {
      write_bit( h == 0 );
      write_int( x >> h * 3 , 3 );
   } while( h-- != 0 );
   return ( ( msb / 3 ) + 1 ) << 2;
}
	

}



