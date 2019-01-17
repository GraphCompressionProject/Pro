#ifndef MERGED_ITERATOR_HPP_
#define MERGED_ITERATOR_HPP_

#include "utility_iterator_base.hpp"
#include "logger.hpp"
#include <cassert>
#include <string>
#include <sstream>
#include <limits>
#include <exception>
#include <stdexcept>
#include <boost/shared_ptr.hpp>

//#define HARDCORE_DEBUG_MERGED_ITOR

namespace webgraph { namespace bv_graph { namespace utility_iterators {


// un itérateur qui fusionne deux itérateurs : retourne la fusion ordonnée des deux itérateurs sans duplicats 
template<class val_type> class merged_iterator : public utility_iterator_base<val_type> {
public:

	
typedef boost::shared_ptr< utility_iterator_base<val_type> > underlying_ptr;

private:
	//les itérateurs croissant 
   underlying_ptr it0, it1;
   bool valid0, valid1;
   val_type curr0, curr1;
   // le nb max d'entier que peut énumérer l'itérateur 
   int n; 

   // initialisation des paramètres 
   void init() {
      if ( valid0 = it0->has_next() ) 
         curr0 = it0->next();
      if ( valid1 = it1->has_next() ) 
         curr1 = it1->next();
   }
   void copy( const merged_iterator<val_type>& other );

public:
   merged_iterator( underlying_ptr i0, underlying_ptr i1 ) :
      it0(i0), it1(i1), n(std::numeric_limits<int>::max()) 
      {
         init();
      }
   merged_iterator( const merged_iterator<val_type>& other ) {
      copy( other );
   }           
   merged_iterator( underlying_ptr i0, underlying_ptr i1, int sz ) :
      it0( i0 ), it1( i1 ), n(sz) {
      init();
   }
   merged_iterator<val_type>& operator = ( const merged_iterator& other ) {
      copy( other );
      return *this;
   }
   bool has_next() const {
      return n != 0 && ( valid0 || valid1 );
   }

#ifdef HARDCORE_DEBUG_MERGED_ITOR
   val_type next() {
      val_type v = next_internal();

      logs::logger( "iterators" ) << LEVEL_DEBUG 
                                  << "merged iterator next about to return " << v 
                                  << "\n";

      return v;
   }
   
   val_type next_internal();
#else
   val_type next();
#endif

   std::string as_str() const {
      std::ostringstream oss;
      
      oss << "merged iterator merging:\n"
          << it0->as_str() << "\nand\n"
          << it1->as_str() << "\n";
      
      return oss.str();
   }
   int skip( int how_many ) {
      int num_skipped = 0;

      while( num_skipped < how_many && has_next() ) {
         next();
         ++num_skipped;
      }

      return num_skipped;
   }
   merged_iterator* clone() const { 
      return new merged_iterator( *this );
   }
};


template<class val_type> void merged_iterator<val_type>::copy( const merged_iterator<val_type>& other ) {
   if( other.it0 != NULL ) {
      it0.reset( other.it0->clone() );
   } else {
      it0.reset();
   }

   if( other.it1 != NULL ) {
      it1.reset( other.it1->clone() );
   } else {
      it1.reset();
   }
   
   valid0 = other.valid0;
   valid1 = other.valid1;
   curr0 = other.curr0;
   curr1 = other.curr1;
   n = other.n;
}

template< class val_type >
#ifdef HARDCORE_DEBUG_MERGED_ITOR
val_type merged_iterator<val_type>::next_internal() {
#else
val_type merged_iterator<val_type>::next() {
#endif
   if ( !has_next() ) 
      throw std::logic_error( "Trying to dereference empty merged_iterator." );
   n--;
   
   int current;
   
   if ( !valid0 ) {
      current = curr1;
      if ( valid1 = it1->has_next() ) 
         curr1 = it1->next();
      return current;
   } 
   if ( !valid1 ) {
      current = curr0;
      if ( valid0 = it0->has_next() ) 
         curr0 = it0->next();
      return current;
   } 
   if ( curr0 < curr1 ) {
      current = curr0;
      if ( valid0 = it0->has_next() ) 
         curr0 = it0->next();
   } 
   else if ( curr0 > curr1 ) {
      current = curr1;
      if ( valid1 = it1->has_next() ) 
         curr1 = it1->next();
   } 
   else {
      current = curr0;
      if ( valid0 = it0->has_next() ) 
         curr0 = it0->next();
      if ( valid1 = it1->has_next() ) 
         curr1 = it1->next();
   }
   return current;
}
   
} } }


#endif /*MERGED_ITERATOR_HPP_*/
