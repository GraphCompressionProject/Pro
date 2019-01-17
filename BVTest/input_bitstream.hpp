#ifndef INPUT_BITSTREAM_HPP
#define INPUT_BITSTREAM_HPP

#include <iostream>
#include <vector>
#include <fstream>
#include <cassert>
#include "fast.hpp"
#include <exception>
#include <boost/shared_ptr.hpp>


//#define LOGGING

namespace webgraph {

// Tutilis�e par read() et refill()
struct eof_exception : public std::exception {
   eof_exception() {}
};


class ibitstream {

public:
   //La taille de #unget(boolean) buffer en bytes 
   const static int UNGET_BUFFER_SIZE = 16;
   const static int DEFAULT_BUFFER_SIZE = 16 * 1024;

private:

	//le nombre de bits actuallement lu depuis ce stream
   long read_bits;
   // buffer de bit courrant
   int current;
  
	
protected:
   typedef unsigned char byte;

   boost::shared_ptr< std::vector<byte> > buffer;
   // si buffer de bytes utilis� 
   bool no_buffer;

   // nombre courant de bits dans le buffer de bits
   unsigned int fill;

   // position courrante dans le buffer de bytes 
   unsigned int pos;

   // nombre de bytes courrants disponibles dans le buffer de bytes
   unsigned int avail;
   
   // position courrante du premier byte dans le buffer de bytes
   unsigned long position;

   
   //Buffer de bytes pour les ungetting buts : allou� � la demande 
   std::vector<byte> unget_bytes;

   
   // Combien de bytes le unget_bytes buffer contient 
   unsigned int unget_count;

   
   // si d�bordement du flux 
   bool overflow;
   // dans le cas de d�bordemment, si on a d�pass� le EOF
   bool past_eof;
   
   //vrai si on �crit un vecteur (l'entr�e est un vecteur) 
   bool wrapping;

private:

   // le flux de bits
   boost::shared_ptr<std::istream> is; 

   // byte parser pour le codage gamma. les bits 0 -7 contiennent la valeur, 8-15 contiennent la taille du code, 0 veut dire que le parsing va en dessous de 8 bits
   const static int GAMMA[];


	// byte parser pour le codage delta, les bits 0 -7 contiennent la valeur, 8-15 contiennent la taille du code, 0 veut dire que le parsing va en dessous de 8 bits
   const static int DELTA[]; 
   
   // byte parser pour le codage zeta, les bits 0 -7 contiennent la valeur, 8-15 contiennent la taille du code, 0 veut dire que le parsing va en dessous de 8 bits
   const static int ZETA_3[]; 
   
   // tableau pr�calcul� des bits les plus significatifs sachant les bytes
   static const int BYTEMSB[];

   //initialisation des param�tres 
   void init() {
      read_bits = 0;
      current = 0;
      no_buffer = true;
      fill = 0;
      pos = 0;
      avail = 0;
      position = 0;
      unget_count = 0;
      overflow = false;
      past_eof = false;
      wrapping = false;
   } 
   void init( const boost::shared_ptr<std::istream>& is, int buf_size ) {
      init();
      this->is = is;
      assert( this->is->good() );
      
      no_buffer = (buf_size == 0);
  
      assert( buffer->size() == (unsigned)buf_size );
//      if ( !no_buffer ) 
//         buffer->resize(buf_size);  			  	
   }
   


public:
   // Constructeur : valeurs par d�faut 
   ibitstream() {
      init();
   }
   
   // Constructeur : cr�e un nouveau flux � partir d'un flux de donn�e et une taille de buffer, taille du buffer peut �tre 0 => pas de buffer n�cessaire
   ibitstream( const boost::shared_ptr<std::istream>& is, int buf_size = DEFAULT_BUFFER_SIZE ) :
      buffer( (buf_size == 0) ? NULL : new std::vector<byte>(buf_size) )
   {
	  init(is, buf_size );
   }

   // NOTE: almost unavoidably this copies A. Any way to prevent? Pass in a pointer
   // Constructeur : cr�e un nouveau flux � partir d'un vecteur de bytes 
   ibitstream( const boost::shared_ptr< std::vector<byte> >& a ) : buffer(a) {
      init(); 
      // is = NullInputStrema.getInstance();
      avail = a->size();
      no_buffer = false;
      wrapping = true;
#ifdef LOGGING
      std::cerr << "##################################################\n"
                << "new ibitstream created from buffer.\n"
                << "first 50 bytes of buffer:\n";
      for( int i = 0; i < 50; i++ ) {
         std::cerr << utils::int_to_binary( (*a)[i], 8 ) << " ";
         if( i + 1 % 10 == 0 )
            std::cerr << "\n";
      }
      std::cerr << "\n";
#endif
   }

   // Constructeur : cr�e un nouveau flux � partir d'un fichier 
   ibitstream( std::string file_name, int buf_size = DEFAULT_BUFFER_SIZE ) : buffer( (buf_size == 0) ? NULL : new std::vector<byte>(buf_size) ) {
      boost::shared_ptr<std::istream> i( new std::ifstream( file_name.c_str() ) );
      init( i, buf_size );
   }

    // destructeur 
   ~ibitstream() {}

   // attacher le bitstream au buffer donn� 
   void attach( boost::shared_ptr<std::vector<unsigned char> > buf ) {
      buffer = buf;
      init();
      avail = buf->size();
      no_buffer = false;
      wrapping = true;
#ifdef LOGGING
      std::cerr << "##################################################\n"
           << "new ibitstream created by ATTACHING to buffer.\n"
           << "first 50 bytes of buffer:\n";
      for( int i = 0; i < 50; i++ ) {
         std::cerr << utils::int_to_binary( (*buffer)[i], 8 ) << " ";
         if( i + 1 % 10 == 0 )
            std::cerr << "\n";
      }
      std::cerr << "\n";
#endif
   }

   // Remise � z�ro du bitstream (flush) 
  void flush() {
      if ( ! wrapping ) {
         position += pos;
         avail = 0;
         pos = 0;
      }
      fill = 0;
      unget_count = 0;
   }
   
   // setter de overflow
   void set_overflow( bool overflow ) {
      this->overflow = overflow;
   }
   // getter de overflow 
   bool get_overflow() {
      return overflow;
   }
   // getter de past_eof
   bool get_past_eof() {
      return past_eof;
   }
   // getter de read_bits
   long get_read_bits() {
      return read_bits;
   }
   //setter de read_bits
   void set_read_bits( long read_bits ) {
      this->read_bits = read_bits;
   }
   
   
private:


    // lire le prochain byte du flux de donn�e 
   int read();

   // recharer fill : essaye de remplit l'octer le plus faible dans le cas o� fill=0, puis essaye de remplir un autre octet dans ce cas le eof n'est pas consid�r� comme erreur et donc l'exception non retenue
   void refill() {
      assert( fill < 8 );
		
      if ( fill == 0 ) {
         current = read();
         fill = 8;
#ifdef LOGGING
         std::cerr << "\tRefilled from empty; current = " << utils::int_to_binary( current, 8 )
                   << "\n";
#endif
      }
      
      // WATCH watch this part
      if( avail > 0 ) {
         int r = read();
#ifdef LOGGING
         std::cerr << "\tIn the process of refilling; just fetched " 
                   << utils::int_to_binary(r & 0xFF, 8)
                   << "\n";
#endif
         current = ( current << 8 ) | r;
         fill += 8;
#ifdef LOGGING
         std::cerr << "Refilled from partially full; current = " 
                   << utils::int_to_binary( current, fill )
                   << "\n";
#endif
      } else {
         // we know we have to read a file, so catch that pesky eof exception
         try {
            int r = read();
#ifdef LOGGING
            std::cerr << "\tIn the process of refilling; just fetched " 
                      << utils::int_to_binary(r & 0xFF, 8)
                      << "\n";
#endif
            current = ( current << 8 ) | r;
            fill += 8;
#ifdef LOGGING
            std::cerr << "Refilled from partially full; current = " 
                      << utils::int_to_binary( current, fill )
                      << "\n";
#endif      
         } catch( eof_exception e ) {
            // don't care.
         }
      }
   }


   // lire des bits depuis le buffer de bits avec possibilit� de le remplir, len : nombre de bits � lire/  retourne le nbr de bits lu dans la faible position 
   int read_from_current( unsigned int len );
   

   // r�organisation du stream
	void align() {
#ifdef LOGGING
      std::cerr << "Align called.\n";
#endif
      if ( ( fill & 7 ) == 0 ) return;
      read_bits += fill & 7;
      fill &= ~7;
   }



public:	 
 
	// Lire une s�quence de bits :  bits: un vecteur de bytes pour stocker le r�sultat / len: le nombre de bits � lire
	void read( byte bits[], unsigned int len );

   // lit un bit et retourne le prochain
   int read_bit() {
      return read_from_current( 1 );
   }

    // lit un entier , len : nombre de bits
    int read_int( unsigned int len );

    // passe 'n' nombre de bits ( ne les lit pas) 
	long skip( unsigned long n );
   
	// setter de position 
	void set_position( unsigned long position );

   // lit un entier cod� en unary 
   int read_unary();

   //lit un entier cod� en gamma
   int read_gamma();

   //lit un entier cod� en delta
   int read_delta();

   //lit un eniter cod� en zeta
   int read_zeta(int k);

   //lit un entier code en nibble
   int read_nibble();
};

} 

#endif /*INPUT_BITSTREAM_HPP_*/
