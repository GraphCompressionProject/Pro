

#ifndef MASKED_ITERATOR_HPP_
#define MASKED_ITERATOR_HPP_

#include "utility_iterator_base.hpp"
#include "logger.hpp"
#include <vector>
#include <cassert>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <exception>
#include <boost/shared_ptr.hpp>

//#define HARDCORE_DEBUG_MI

namespace webgraph { namespace bv_graph { namespace utility_iterators {


// itérateur du vecteur masque (masquer un itérateur de base) 
template<class val_type>
class masked_iterator : public utility_iterator_base<val_type> {
private:
   typedef boost::shared_ptr< utility_iterator_base<val_type> > underlying_ptr;
   // l'itérateur de base 
   underlying_ptr underlying;
   
   // le vecteur masque 
   std::vector<int> mask;

   // taille du masque 
   unsigned mask_len;
   
   // indice dans le masque qui représente un bloc d'exclusion 
   unsigned curr_mask;

   // combien d'entier restent dans le bloc d'inclusion courrant : si 0 : tout ce qui reste doit être rejeté / si -1 tout doit être retenu 
   int left;

   // si y'a plus d'élément à itérer 
   bool not_over;
   
   // initialisation des paramètres 
   void init() {
      curr_mask = 0;

      if ( curr_mask < mask_len ) {
         left = mask[ curr_mask++ ];
         advance();
      }
      else 
         left = -1;
      not_over = underlying->has_next();	
   }

   // copier un other dans l'itérateur courrant
   void copy( const masked_iterator& other ) {
      if( other.underlying != NULL ) 
         underlying.reset( other.underlying->clone() );
      else
         underlying.reset();

      mask = other.mask;
      mask_len = other.mask_len;
      curr_mask = other.curr_mask;
      left = other.left;
      not_over = other.not_over;
   }

 public:
   
    //Constructeurs 
   masked_iterator( const masked_iterator& other ) {
      copy( other );
   }
   masked_iterator( const std::vector<int>& m, underlying_ptr ul ) : underlying(ul), mask( m ), mask_len( m.size() )
      {
         init();
      }
   masked_iterator( const std::vector<int>& m,  const int ml, underlying_ptr ul ) : underlying( ul ), mask( m ), mask_len( ml )
      {
         init();
      }
   

#ifdef HARDCORE_DEBUG_MI
   val_type next() {
      val_type val = next_internal();
      logs::logger( "iterators" ) << LEVEL_DEBUG
                                  << "masked iterator next about to return " << val
                                  << "\n";
      return val;
   }

   val_type next_internal();
#else
   val_type next();
#endif
   bool has_next() const;
   int skip( int n );
   masked_iterator& operator = ( const masked_iterator& other ) {
      copy( other );
   }
   masked_iterator* clone() const {
      return new masked_iterator( *this );
   }   
   std::string as_str() const {
      std::ostringstream oss;

      oss << "masked_iterator masking:\n"
          << "\t" << underlying->as_str();

      return oss.str();
   }   
private:
   void advance();
};

template<class val_type>
#ifdef HARDCORE_DEBUG_MI
val_type masked_iterator<val_type>::next_internal() {
#else
val_type masked_iterator<val_type>::next() {
#endif
   if ( ! has_next() ) 
      throw std::logic_error( "Trying to dereference empty masked_iterator." );

   const int nxt = underlying->next();
   if ( left > 0 ) {
      left--;
      advance();
   }
   not_over = underlying->has_next();
   return nxt;
}
template<class val_type> bool masked_iterator<val_type>::has_next() const {
   if ( left == 0 ) 
      return false;
   if ( left < 0 ) {
      if( not_over ) assert( underlying->has_next() );
      return not_over;
   }

   assert( underlying->has_next() );
   return true;
}
template<class val_type> void masked_iterator<val_type>::advance() {
   if ( left == 0 && curr_mask < mask_len ) {
      underlying->skip( mask[ curr_mask++ ] );
      if ( curr_mask < mask_len ) 
         left = mask[ curr_mask++ ];
      else left = -1;	
   }
}
template<class val_type> int masked_iterator<val_type>::skip( int n ) {
   int skipped = 0;
   
   while( skipped < n && ( left > 0 || left == -1 && not_over ) ) {
      if ( left == -1 ) 
         skipped += underlying->skip( n - skipped );
      else {
         if ( n - skipped < left ) {
            underlying->skip( n - skipped );
            not_over = underlying->has_next();
            left -= ( n - skipped );
            return n;
         }
         else {
            underlying->skip( left );
            skipped += left;
            left = 0;
            advance();
            not_over = underlying->has_next();
         }
      }
   }
   
   return skipped;
}

} } }
#endif /*MASKED_ITERATOR_HPP_*/
