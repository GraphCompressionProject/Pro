#ifndef RESIDUAL_ITERATOR_HPP_
#define RESIDUAL_ITERATOR_HPP_

#include "utility_iterator_base.hpp"
#include "fast.hpp"
#include "input_bitstream.hpp"
#include <boost/iterator/iterator_facade.hpp>
#include <boost/shared_ptr.hpp>
#include <string>
#include <sstream>
#include "webgraph.hpp"
#include "logger.hpp"

//#define HARDCORE_DEBUG_RESID_ITOR

namespace webgraph { namespace bv_graph { 

//class graph;   
   
namespace utility_iterators {

// itérateur de résidus 
template<class val_type>
class residual_iterator : public utility_iterator_base<int> {
private:
   unsigned int node;
   int prev;
   unsigned int i;
   const webgraph::bv_graph::graph* owner;
   boost::shared_ptr<ibitstream> ibs;
   
public:
   residual_iterator( int n, int res_left, const webgraph::bv_graph::graph* o, boost::shared_ptr<ibitstream> i ) :
      node(n), prev(-1), i( res_left ), owner(o), ibs(i) {}
   
   // vrai s'il y'a encore des éléments
   bool has_next() const  {
      return i != 0;
   }
   

#ifdef HARDCORE_DEBUG_RESID_ITOR
   val_type next() {
      val_type n = next_internal();
      logs::logger( "iterators" ) << LEVEL_EVERYTHING << "residual_iterator next about to return "
                                  << n << "\n";
      return n;
   }

   val_type next_internal();
#else
   val_type next();
#endif

   //to_string
   std::string as_str() const {
      std::ostringstream oss;
      oss << "residual iterator:\n"
          << "node = " << node << ", prev = " << prev << ", i = " << i << "\n";
      return oss.str();
   }

   //cloner l'itérateur
   residual_iterator* clone() const {
      return new residual_iterator( *this );
   }

   //passer un certain nombre d'éléments 
   int skip( int how_many ) {
      int num_skipped = 0;
       while( has_next() && num_skipped < how_many ) {
          next();
          num_skipped++;
       }
       return num_skipped;
   }
};

template<class val_type>
#ifdef HARDCORE_DEBUG_RESID_ITOR
val_type residual_iterator<val_type>::next_internal() {
#else
// retourner le prochain élément 
val_type residual_iterator<val_type>::next() {
#endif
   if ( ! has_next() )
      throw logic_error( "Trying to dereference empty residual_iterator." );
   i--;
   if ( prev == -1 ) 
      return prev = node + utils::nat2int( owner->read_residual( *ibs ) );
   else 
      return prev = owner->read_residual( *ibs ) + prev + 1;
} 

} } }
#endif /*RESIDUAL_ITERATOR_HPP_*/
