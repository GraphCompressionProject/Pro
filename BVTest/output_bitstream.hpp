#ifndef OUTPUT_BITSTREAM_HPP
#define OUTPUT_BITSTREAM_HPP

#include <iostream>
#include <vector>
#include <fstream>
#include <cassert>
#include <boost/shared_ptr.hpp>
#include "logger.hpp"

namespace webgraph {

class obitstream {
public:
   
	// taille par d�faut du buffer de bytes 
   const static int DEFAULT_BUFFER_SIZE = 16 * 1024;
   typedef unsigned char byte;

protected:

   // le flux de sortie (�criture) 
   boost::shared_ptr<std::ostream> os;

   //le nombre de bits �crits dans ce flux 
   long written_bits;

   //buffer de bit courrant
   int current;
   
   // le buffer du stream 
   boost::shared_ptr<std::vector<byte> > buffer;

   // le nombre de bits libre dans le buffer de bits 
   int free;

   // la position courrante dans le buffer de bytes
   int pos;

   // la position courrante dans le flux de sortie 
   long position;

   // le nombre de bits disponibles dans le buffer de bytes
   int avail;

   // la taille du buffer temporaire 
   const static int TEMP_BUFFER_SIZE = 128;

   // vrai si la donn�e en entr�e est un vecteur 
   bool wrapping;

   // vrai si on utilise pas de buffer
   bool no_buffer;

private: 
  
   // petit buffer pour un usage temporaire 
   std::vector<byte> temp_buffer;

   // initialisation des param�tres 
   void init() {
      written_bits = 0;
      current = 0;
      free = 8;
      pos = 0;
      position = 0;
      avail = 0;
      wrapping = false;
      no_buffer = true;
      temp_buffer.resize( TEMP_BUFFER_SIZE );

#ifndef CONFIG_FAST
      logs::register_logger( "obs", logs::LEVEL_NONE );
#endif
   }
   void init( const boost::shared_ptr<std::ostream>& os, const int buf_size ) {
      init();
      
      this->os = os;
      
      if ( buf_size != 0 ) {
         buffer->resize( buf_size );
         avail = buf_size;
         no_buffer = false;
      } else {
        	avail = 0;
        	no_buffer = true;
      }
   }

protected:
	// Constructeur : valeurs par d�faut 
   obitstream() {
      init();
   }
   
public:

	// Constructeur : cr�e un nouveau stream � partir d'un flux de donn�e et une taille de buffer
   obitstream( const boost::shared_ptr<std::ostream>& os, const int buf_size = DEFAULT_BUFFER_SIZE ) :
      buffer( (buf_size == 0) ? NULL : new std::vector<byte>(buf_size) ) {
      init(os, buf_size );
   }

	// Constructeur : cr�e un nouveau stream � partir d'un vecteur de byte 
   obitstream( boost::shared_ptr<std::vector<byte> >& a ) : buffer(a) {
      init();

      free = 8;
      avail = a->capacity();
      wrapping = true;
   }

   //Constructeur : cr�e un nouveau stream � partir d'un fichier de donn�e 
   obitstream( const std::string name, const int buf_size = DEFAULT_BUFFER_SIZE ) :
      buffer( (buf_size == 0) ? NULL : new std::vector<byte>( buf_size ) ) {
      boost::shared_ptr<std::ostream> o( new std::ofstream( name.c_str() ) );
      init(o, buf_size);
   }
   
   //Destructeur 
    ~obitstream() {
	    // make sure everything makes it to the file.
    	flush();	
    }

	// vider le flux 
   void flush() {
#ifndef CONFIG_FAST
      logs::logger( "obs" ) << "Flush called.\n";
#endif
      
      align();
      if ( os != NULL ) {
         if ( !no_buffer ) {
            os->write( (char*)&(*buffer)[0], pos );
            position += pos;
            pos = 0;
            avail = buffer->size();
         }
         os->flush();
      }
   }

 
   // getter et setter de written_bits 
   long get_written_bits() {
      return written_bits;
   }
   void set_written_bits( const long written_bits ) {
      this->written_bits = written_bits;
   }

private:
   
	// �crit un byte dans le flux de sortie 
   void write( const int b );
   
   // �crit des bits dans le buffer de bit, possibilit� de vider le buffer, b : les bits � �crire, len : le nombre de bits � �crire
   int write_in_current( const int b, const int len );

public:
 
	// r�organiser le flux de donn�e 
   int align() {
#ifndef CONFIG_FAST
      logs::logger("obs") << "Align called\n";
#endif
      if ( free != 8 ) 
         return write_in_current( 0, free );
      else 
         return 0;
   }

   //setter de position 
   void set_position(const long position);
   

   // �crire une s�quence de bits, bits : les bits � �crire 
   int write( const byte bits[], const int len ) {
      return write_byte_offset( bits, 0, len );
   }
  
   // �crit une s�quence de bits, en commen�ant par un offset donn� 
   int write( const byte bits[], const int offset, const int len );

protected:
  
   // �crit une s�quence de bits, en commen�ant par un offset donn� 
   int write_byte_offset( const byte bits[], const int offset, int len );

   
   // �crit un bit 
   int write_bit( const bool bit ) {
      return write_in_current( bit ? 1 : 0, 1 ); 
   }

   // �crit un bit 
   int write_bit( const int  bit ) {
      assert( bit == 0 || bit == 1 );
      return write_in_current( bit, 1 ); 
   }

public:
   //�crit un entier 
   int write_int( int x, const int len );


private:
   
	//retourne le bit le plus significatif d'un entier 
   static int most_significant_bit( const int x );


public:
   
   //�crit un entier en utilisant le codage unary
   int write_unary( int x );

   //�crit un entier en utilisant le codage gamma
   int write_gamma( int x );

   //�crit un entier en utilisant le codage delta
   int write_delta( int x );

   //�crit un entier en utilisant le codage zeta
   int write_zeta( int x, const int k );

   //�crit un entier en utilisant le codage nibble
   int write_nibble( const int x );
};


}

#endif
