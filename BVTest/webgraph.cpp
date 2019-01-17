#include "stdafx.h"
#include <cassert>
#include <climits>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <iterator>
#include <boost/filesystem/operations.hpp>
#include <boost/regex.hpp>
#include <boost/program_options.hpp>
#include <boost/progress.hpp>

#include "fast.hpp"
#include "debug_obitstream.hpp"
#include "types.hpp"
#include "webgraph.hpp"
#include "offline_vertex_iterator.hpp"
#include "compression_flags.hpp"

#include "properties.hpp"

#include "interval_sequence_iterator.hpp"
#include "merged_iterator.hpp"
#include "masked_iterator.hpp"
#include "residual_iterator.hpp"
#include "utility_iterator_base.hpp"
#include "iterator_wrappers.hpp"
#include "empty_iterator.hpp"

#include "DirectedGraph.h"
#include <time.h>
#include <direct.h>

#undef min 
#undef max 

//#define HARDCORE_DEBUG

namespace webgraph { namespace bv_graph {

#ifndef CONFIG_FAST
using namespace logs;
#endif

using namespace std;


// VARIABLES LIVE HERE
const int graph::DEFAULT_MAX_REF_COUNT = std::numeric_limits<int>::max();
//const int graph::BVGRAPH_VERSION;
const int graph::DEFAULT_WINDOW_SIZE;
const int graph::DEFAULT_MIN_INTERVAL_LENGTH;
const int graph::DEFAULT_OFFSET_STEP;
const int graph::DEFAULT_ZETA_K;
const int graph::OUTDEGREES_GAMMA;
const int graph::OUTDEGREES_DELTA;
const int graph::BLOCKS_GAMMA;
const int graph::BLOCKS_DELTA;
const int graph::RESIDUALS_GAMMA;
const int graph::RESIDUALS_ZETA;
const int graph::RESIDUALS_DELTA;
const int graph::RESIDUALS_NIBBLE;
const int graph::REFERENCES_GAMMA;
const int graph::REFERENCES_DELTA;
const int graph::REFERENCES_UNARY;
const int graph::BLOCK_COUNT_GAMMA;
const int graph::BLOCK_COUNT_DELTA;
const int graph::BLOCK_COUNT_UNARY;
const int graph::OFFSETS_GAMMA;
const int graph::OFFSETS_DELTA;
const int graph::INITIAL_SUCCESSOR_LIST_LENGTH;
const int graph::NO_INTERVALS;

// getters 
long graph::get_num_nodes() const {
   return n;
}
long graph::get_num_arcs() const {
   return m;
}        
string graph::get_basename() const {
   return basename;
}
int graph::get_max_ref_count() const {
   return max_ref_count;
}
int graph::get_window_size() const {
   return window_size;
}
int graph::get_offset_step() const {
   return offset_step;
}


// lire un offset > ibs : l'offset file , retourne : le prochaine offset
int graph::read_offset( ibitstream& ibs ) const  {
   switch( offset_coding ) {
   case compression_flags::GAMMA: 
      return ibs.read_gamma(); 
   case compression_flags::DELTA: 
      return ibs.read_delta(); 
   default: 
      // TODO do something better here.
      assert(0);
   }
}

// écrit un offset > obs : offset file , x : l'offset à écrire , retourne : le nombre de bits écrits
int graph::write_offset( obitstream& obs, int x ) const {
   switch( offset_coding ) {
   case compression_flags::GAMMA: 
      return obs.write_gamma( x ); 
   case compression_flags::DELTA: 
      return obs.write_delta( x ); 
   default:
      // TODO do something more elegant here
      assert(0);
   }
}

// lit un outdegree > ibs : graphe file , retourne : le prochain outdegree 
int graph::read_outdegree( ibitstream& ibs ) const {
   switch( outdegree_coding ) {
   case compression_flags::GAMMA: 
      return ibs.read_gamma(); 
   case compression_flags::DELTA: 
      return ibs.read_delta(); 
   default:
      assert(0);
      // TODO do something better here.
   }
}

//écrit un outdegree > obs graphe file , d : l'outdegree à écrire, retourne : le nb de bits écrits 
int graph::write_outdegree( obitstream& obs, int d ) const {
   switch( outdegree_coding ) {
   case compression_flags::GAMMA: 
      return obs.write_gamma( d ); 
   case compression_flags::DELTA: 
      return obs.write_delta( d ); 
   default:
      assert(0);
      // TODO do something better here.
   }
}


// lit une référence 
int graph::read_reference( ibitstream& ibs ) const {
   int ref;

   switch( reference_coding ) {
   case compression_flags::UNARY: 
      ref = ibs.read_unary(); 
      break;
   case compression_flags::GAMMA: 
      ref = ibs.read_gamma(); 
      break;
   case compression_flags::DELTA: 
      ref = ibs.read_delta(); 
      break;
   default:
      assert(0);
      // TODO do something better here.
   }
   assert( ref <= window_size );
        
   //throw new IllegalStateException( "The required reference (" + ref + ") is incompatible with the window size (" + window_size + ")" );
   return ref;
}
        
// écrit une référence 
int graph::write_reference( obitstream& obs, int ref ) const {
   assert( ref <= window_size );
   //throw new IllegalStateException( "The required reference (" + ref + ") is incompatible with the window size (" + window_size + ")" );

   switch( reference_coding ) {
   case compression_flags::UNARY: 
      return obs.write_unary( ref );
   case compression_flags::GAMMA: 
      return obs.write_gamma( ref );
   case compression_flags::DELTA: 
      return obs.write_delta( ref );
   default:
      assert(0);
      // TODO do something better here.
   }
}

// lit un block count 
int graph::read_block_count( ibitstream& ibs ) const {
   switch( block_count_coding ) {
   case compression_flags::UNARY: 
      return ibs.read_unary();
   case compression_flags::GAMMA: 
      return ibs.read_gamma();
   case compression_flags::DELTA: 
      return ibs.read_delta();
   default:
      assert(0);
      // TODO do something better here.
   }
}

//écrit un block count 
int graph::write_block_count( obitstream& obs, int count ) const {
   switch( block_count_coding ) {
   case compression_flags::UNARY: 
      return obs.write_unary( count );
   case compression_flags::GAMMA: 
      return obs.write_gamma( count );
   case compression_flags::DELTA: 
      return obs.write_delta( count );
   default:
      assert(0);
      // TODO do something better here.
   }
}

//lit un block
int graph::read_block( ibitstream& ibs ) const {
   switch( block_coding ) {
   case compression_flags::UNARY: 
      return ibs.read_unary();
   case compression_flags::GAMMA: 
      return ibs.read_gamma();
   case compression_flags::DELTA: 
      return ibs.read_delta();
   default:
      assert(0);
      // TODO do something better here.
   }
}

//écrit un block 
int graph::write_block( obitstream& obs, int block ) const {
   switch( block_coding ) {
   case compression_flags::UNARY: 
      return obs.write_unary( block );
   case compression_flags::GAMMA: 
      return obs.write_gamma( block );
   case compression_flags::DELTA: 
      return obs.write_delta( block );
   default:
      assert(0);
      // TODO do something better here.
   }
}
        
// lit un résidu
int graph::read_residual( ibitstream& ibs ) const {
   switch( residual_coding ) {
   case compression_flags::GAMMA: 
      return ibs.read_gamma();
   case compression_flags::ZETA: 
      return ibs.read_zeta( zeta_k );
   case compression_flags::DELTA: 
      return ibs.read_delta();
   case compression_flags::NIBBLE: 
      return ibs.read_nibble();
   default:
      assert(0);
      // TODO do something better here.
   }
}

//écrit un résidu 
int graph::write_residual( obitstream& obs, int residual ) const {
#ifndef CONFIG_FAST
   lg() << "write_residual( " << residual << " )\n";
#endif

   switch( residual_coding ) {
   case compression_flags::GAMMA: 
      return obs.write_gamma( residual );
   case compression_flags::ZETA: 
      return obs.write_zeta( residual, zeta_k );
   case compression_flags::DELTA: 
      return obs.write_delta( residual );
   case compression_flags::NIBBLE: 
      return obs.write_nibble( residual );
   default:
      assert(0);
      // TODO do something better here.
   }
}

// retourner le outdegree d'un noeud aléatoire 
int graph::outdegree( int x ) const {

   assert( x >= 0 && x < n );

   // il faut que l'offset > 0 pour lire le outdegree d'un noeud aléatoire 
   assert(offset_step > 0); 

   // With all offsets, we just position and read.
   if ( offset_step == 1 ) {
      outdegree_ibs.set_position( offset[ x ] );
      return read_outdegree( outdegree_ibs );
   }

   // Otherwise, it could happen that the required outdegree is in the outdegree cache.
   if ( x >= outdegree_cache_start && x < outdegree_cache_end ) 
      return outdegree_cache[ x - outdegree_cache_start ];

   outdegree_ibs.set_position( offset[ x / offset_step ] );
        
   // We now skip x % offset_step outdegrees to get the right one.
   int i = x % offset_step;
        
   while( i-- != 0 ) 
      read_outdegree( outdegree_ibs );
                
   return read_outdegree( outdegree_ibs );
}

//skip la partie de la liste des successeurs d'un noeud qui vient après le outdegree > ibs : le flux positionné au début de la liste des successeurs
// x : le noeud dont la liste des sucesseurs va être sauté (skiped) , outd:  d(x). 
void graph::skip_node( ibitstream& ibs, int x, int outd ) const {
   int ref, block_count;
   int i, l, extra_count;
   
   assert(x > 0);
   
   // Without offsets, we just give up.
   assert(offset_step > 0);
   //if ( offset_step <= 0 ) 
   //throw new IllegalStateException( "You cannot skip a successor list without offsets" );
   
   if ( outd == 0 ) 
      return; // No successors, we're over.
   
   // First of all, we check the reference if necessary.
   if ( window_size > 0 ) 
      ref = read_reference( ibs );
   else 
      ref = -1;
   
   if ( ref > 0 ) {
      block_count = read_block_count( ibs );
      
      int copied = 0, total = 0;
      
      for( i = 0; i < block_count; i++ ) {
         l = read_block( ibs ) + ( i == 0 ? 0 : 1 );
         total += l;
         
         if ( i % 2 == 0 ) 
            copied += l;
      }
      
      if ( block_count % 2 == 0 ) 
         copied += outdegree( x - ref ) - total;
      extra_count = outd - copied;
   } 
   else 
      extra_count = outd;
   
   int intervalCount = 0; // Number of intervals
   
   if ( extra_count != 0 ) {
      if ( min_interval_length != NO_INTERVALS && ( intervalCount = ibs.read_gamma() ) != 0 ) {
         for ( i = 0; i < intervalCount; i++ ) {
            ibs.read_gamma(); // Throw away left point.
            l = ibs.read_gamma() + min_interval_length;
            extra_count -= l;
         }
      }
   }
   
   while( extra_count-- != 0 ) 
      read_residual( ibs );
}
        
// positionne le flux juste avant la liste des successeurs du noeud donnée , juste aprés son outdegree qui est retourné 
int graph::position( ibitstream& ibs, int x ) const {
   assert(x >= 0);
   
   assert( offset_step > 0 );
   
   // Without offsets, we just give up.
   //      if ( offset_step <= 0 ) 
   //              throw new IllegalStateException( "You cannot position a stream without offsets" );
   
#if 0
   cerr << "Here's the offset_step - " << offset_step << endl;
#endif
   
   if ( offset_step == 1 ) {
#if 0
      cerr << "offset_step == 1 - about to call set_position( " << offset[x] << ").\n";
#endif
      ibs.set_position( offset[ x ] );
      return read_outdegree( ibs );
   }
   
   // If we happen to be inside the offset cache, we're done.
   if ( x >= outdegree_cache_start && x < offset_cache_end ) {
      ibs.set_position( offset_cache[ x - outdegree_cache_start ] );
      return outdegree_cache[ x - outdegree_cache_start ];
   }
   
   long block_start_offset = offset[ x / offset_step ];
   
   ibs.set_position( block_start_offset );
   // We set the number of read bits so to read it later to fill the offset cache.
   ibs.set_read_bits( block_start_offset ); 
   
   int offset_in_block = x % offset_step;
   outdegree_cache_start = x - offset_in_block;
   
   int actual_step = min( offset_step, (int)(n - outdegree_cache_start) );
   outdegree_cache_end = outdegree_cache_start + actual_step;
   
   // First, we skip outdegrees so to get to the successor lists.
   int i;
   
   for( i = 0; i < actual_step; i++ ) 
      outdegree_cache[ i ] = read_outdegree( ibs );
   
   // Then, we skip the lists before the one we want.
   for( i = 0; i < offset_in_block; i++ ) {
      offset_cache[ i ] = ibs.get_read_bits();
      
      skip_node( ibs, outdegree_cache_start + i, outdegree_cache[ i ] );
   }
   
   offset_cache[ offset_in_block ] = ibs.get_read_bits();
   offset_cache_end = outdegree_cache_start + offset_in_block + 1;
   
   return outdegree_cache[ offset_in_block ];
}

//retourne un itérateur de successeurs d'un certain noeud x 
graph::succ_itor_pair graph::get_successors( int x ) const {
   // We just call successors(int, InputBitStream, int[][], int[], int[]) with
   // a newly created input bit stream and null elsewhere.
   assert(x >= 0);
   
   assert(offset_step > 0);
   //if ( offset_step <= 0 ) 
   //      throw new UnsupportedOperationException( "Random access to successor lists is not possible with sequential or offline graphs" );
   
   internal_succ_itor_ptr p = get_successors_internal( x );

   return make_pair( iterator_wrappers::java_to_cpp<int>( p ), 
                     iterator_wrappers::java_to_cpp<int>() );
}


//wrapper pour la fct get_successors_internal 
graph::internal_succ_itor_ptr graph::get_successors_internal( int x ) const {
   assert( in_memory ); // do this for now
   boost::shared_ptr<ibitstream> ibs( new ibitstream(graph_memory_ptr) );
   
   //ibitstream ibs = in_memory ? ibitstream( graph_memory ) : //ibitstream( ArrayInputStream( *graph_stream ) );
   
   vector<vector<int> > blah1(0);
   vector<int> blah(0);
   
   internal_succ_itor_ptr p = get_successors_internal( x, ibs, blah1, blah, blah );
   
   return p;
}
  
//donnant un flux qui représente un fichier graphe, la fct retourne un itérateur des sucesseurs d'un noeud x  
graph::internal_succ_itor_ptr graph::get_successors_internal( int x, boost::shared_ptr<ibitstream> ibs, vector<vector<int> >& window, vector<int>& outd, vector<int>& block_outdegrees )  const {
   int i;
   int extra_count;
   int ref, ref_index;
   vector<int> block(0);
   int block_count = 0;
   vector<int> left(0);
   vector<int> len(0);
   
#ifndef CONFIG_FAST
   lg() << LEVEL_DEBUG << "get_successors_internal()\n";
#endif

   assert(x >= 0 && x < n);
   //      if (x < 0 || x >= n) 
   //              throw new IllegalArgumentException( "Node index out of range:" + x );
   
   int d;
   int cyclic_buffer_size = window_size + 1;
   
   if ( window.size() == 0 ) { // We position on the given node.
      // Without offsets, we just give up.  if ( offset_step <= 0 ) throw new
      //IllegalStateException( "You cannot position a stream without offsets" );
      assert(offset_step > 0);
      // If window is null, we use the position method, which may modify the
      // offset/outdegree caches.
      d = position( *ibs, x );
   } else {
      if ( offset_step <= 1 ) {
         d = outd[ x % cyclic_buffer_size ] = read_outdegree( *ibs ); // We just read the outdegree.
      } else {
         // We are reading a rearranged graph file.
         if ( x % offset_step == 0 ) {
            // We are at the start of a block, so we read and cache the outdegrees.
            int actual_step = min( offset_step, (int)(n - x) );
            
            for( i = 0; i < actual_step; i++ ) 
               block_outdegrees[ i ] = read_outdegree( *ibs );
         }
         
         // We fetch the current outdegree from the block cache.
         d = outd[ x % cyclic_buffer_size ] = block_outdegrees[ x % offset_step ];
      }
   }
   
   if ( d == 0 ) {
      return internal_succ_itor_ptr(new utility_iterators::empty_iterator());
   }
   
   // We read the reference only if the actual window size is larger than one (i.e., the
   // one specified by the user is larger than 0).
   if ( window_size > 0 ) 
      ref = read_reference( *ibs );
   else 
      ref = -1;

   // The index in window[] of the node we are referring to (it makes sense only if
   // ref>0).
   ref_index = ( x - ref + cyclic_buffer_size ) % cyclic_buffer_size; 
   
   // This catches both no references at all and no reference specifically for this node.
   if ( ref > 0 ) { 
      if ( ( block_count = read_block_count( *ibs ) ) !=  0 ) 
         block.resize( block_count );
      
      // The number of successors copied, and the total number of successors specified in
      // some copy block.
      int copied = 0, total = 0; 
      
      for( i = 0; i < block_count; i++ ) {
         block[ i ] = read_block( *ibs ) + ( i == 0 ? 0 : 1 );
         total += block[ i ];
         if ( i % 2 == 0 ) 
            copied += block[ i ];
      }
      
      // If the block count is even, we must compute the number of successors copied implicitly.
      if ( block_count % 2 == 0 ) 
         copied += ( window.size() != 0 ? outd[ ref_index ] : outdegree( x - ref ) ) - total;
      
      extra_count = d - copied;
   }
   else 
      extra_count = d;
   
   int interval_count = 0; // Number of intervals
   
   if ( extra_count > 0 ) {
      // Prepare to read intervals, if any
      if ( min_interval_length != NO_INTERVALS && 
           ( interval_count = ibs->read_gamma() ) != 0 ) {
         int prev = 0; // Holds the last integer in the last interval.
         left.resize( interval_count );
         len.resize( interval_count );
         
         // Now we read intervals
         left[ 0 ] = prev = utils::nat2int(ibs->read_gamma()) + x; 
         len[ 0 ] = ibs->read_gamma() + min_interval_length;
         
         prev += len[ 0 ];
         extra_count -= len[ 0 ];
         
         for ( i = 1; i < interval_count; i++ ) {
            left[ i ] = prev = ibs->read_gamma() + prev + 1;
            len[ i ] = ibs->read_gamma() + min_interval_length;
            prev += len[ i ];
            extra_count -= len[ i ];
         }
      }
   }

   int residual_count = extra_count; // Just to be able to use an anonymous class.
   using namespace utility_iterators;

   ////////////////////////////////////////////////////////////////////////////////
   // START CONSTRUCTING THE PAIR OF ITERATORS TO REPRESENT THIS SUCCESSOR LIST.
   
   internal_succ_itor_ptr res_itor;
   
   if( residual_count != 0 ) {
      res_itor.reset( new residual_iterator<int>(x, residual_count, this, ibs) );
   } 
#ifndef CONFIG_FAST
   else {
      lg() << LEVEL_EVERYTHING << "chose NOT to create res_itor.\n";
   }
#endif
      
   // The extra part is made by the contribution of intervals, if any, and by the residuals iterator.
   internal_succ_itor_ptr extra_itor;
   
   typedef int successor_t;
   
   if( interval_count == 0 ) {
      extra_itor = res_itor;
#ifndef CONFIG_FAST
      lg() << LEVEL_EVERYTHING
           << "chose first branch.\n";
#endif
   } else if( residual_count == 0 ) {
      extra_itor.reset( new interval_sequence_iterator<successor_t>( left, len ) );
#ifndef CONFIG_FAST
      lg() << LEVEL_EVERYTHING
           << "chose second branch.\n";
#endif
   } else {
      internal_succ_itor_ptr merge_1( new interval_sequence_iterator<successor_t>(left, len) );

      // need to make a pair from the above, and the residual iterator.
      
      typedef merged_iterator< successor_t > merged_itor_t;
      
      // Place #1 where a merged iterator is used.

      assert( res_itor != NULL );
      assert( merge_1 != NULL );

      extra_itor.reset( new merged_itor_t( merge_1, res_itor ) );
   }
   
   internal_succ_itor_ptr block_iterator;
   
   if( ref > 0 ) {
      internal_succ_itor_ptr ref_list;
      
      if( window.size() > 0 ) {
         typedef iterator_wrappers::itor_capture_wrapper<vector<int>::iterator, int> capture_wrapper_t;

         ref_list.reset(new capture_wrapper_t(window[ref_index].begin(), 0, (unsigned)outd[ref_index] ));
      } else {
         // compute the reference list recursively.
         ref_list = get_successors_internal(x - ref); 
      }
      
      // finally, make the block iterator.
      block_iterator.reset( new masked_iterator<int>(block, ref_list ) );
   }
   
   if ( ref <= 0 ) {
#ifndef CONFIG_FAST
      lg() << LEVEL_EVERYTHING
           << "ref <= 0, so returning extra_itor:\n"
           << extra_itor->as_str() << "\n";
#endif
      return extra_itor;
   } else { 
      if( extra_itor == NULL ) {
#ifndef CONFIG_FAST
         lg() << LEVEL_EVERYTHING
              << "returning block iterator:\n"
              << block_iterator->as_str() << "\n";
#endif
         return block_iterator;
      } else {
         // Place #1 where a merged iterator is used.
         typedef merged_iterator<int> mi_t;

         internal_succ_itor_ptr r(new mi_t( block_iterator, extra_itor, d ) );
#ifndef CONFIG_FAST
         lg() << LEVEL_EVERYTHING
              << "Returning that complicated merged iterator:\n"
              << r->as_str() << "\n";
#endif
         return r;
      }
   }
   
   // must have returnd something by now.
#ifndef CONFIG_FAST
   lg() << LEVEL_DEBUG << "Dying!\n";
#endif

   assert( false );
} // end of method.

 // retourne un itérateur de noeuds pour scanner le graphe séquentiellement , commençant du noeud donné 
pair<node_iterator, node_iterator> graph::get_node_iterator( int from ) const {
   typedef boost::shared_ptr<ibitstream> ibs_ptr;

   if( offset_step == -1 ) {
      return make_pair( node_iterator( this, 
                                       ibs_ptr(new ibitstream(basename + ".graph", 
                                                              STD_BUFFER_SIZE) ),
                                       from, 
                                       window_size ),
                        node_iterator() );
   } else {
      if( in_memory ) {
#ifdef HARDCORE_DEBUG
         cerr << "##################################################\n"
              << "Graph in memory\n";
#endif
         return make_pair( node_iterator( this,
                                          ibs_ptr( new ibitstream(graph_memory_ptr) ),
                                          from,
                                          window_size ),
                           node_iterator() );
      } else {
         assert(0);
      }
   }
   
//    try {
//       return offset_step == -1 
//          ? new BVGraphNodeIterator( new InputBitStream( new FileInputStream( basename + ".graph" ), STD_BUFFER_SIZE ), from )
//          : new BVGraphNodeIterator( in_memory ? new InputBitStream( graph_memory ) : new InputBitStream( new FastMultiByteArrayInputStream( graphStream ) ), from );
//    } catch ( FileNotFoundException e ) {
//       throw new IllegalStateException( "The graph file \"" + basename + ".graph\" cannot be found" );
//    }
}
   
// produit une chaine qui représente les valeurs codé dans le flag mask donné 
string graph::flags_to_string( int flags ) {
   string s;
   
   if ( ( flags & 0xF ) != 0 ) {
      s += " | OUTDEGREES_";
      s += compression_flags::CODING_NAME[ flags & 0xF ];
   }
   
   if ( ( ( flags >> 4 ) & 0xF ) != 0 ) {
      s += " | BLOCKS_";
      s += compression_flags::CODING_NAME[ ( flags >> 4 ) & 0xF ];
   }
   
   if ( ( ( flags >> 8 ) & 0xF ) != 0 ) {
      s += " | RESIDUALS_";
      s += compression_flags::CODING_NAME[ ( flags >> 8 ) & 0xF ];
   }
   
   if ( ( ( flags >> 12 ) & 0xF ) != 0 ) {
      s += " | REFERENCES_";
      s += compression_flags::CODING_NAME[ ( flags >> 12 ) & 0xF ];
   }
   
   if ( ( ( flags >> 16 ) & 0xF ) != 0 ) {
      s += " | BLOCK_COUNT_";
      s += compression_flags::CODING_NAME[ ( flags >> 16 ) & 0xF ];
   }
      
   if ( ( ( flags >> 20 ) & 0xF ) != 0 ) {
      s += " | OFFSETS_";
      s += compression_flags::CODING_NAME[ ( flags >> 20 ) & 0xF ];
   }
      
   if ( s.length() != 0 ) 
      s.erase( 0, 3 );
   
   return s;
}
   
// produit un flag mask qui correspond à la chaine donnée 
int graph::string_to_flags( string flag_string ) {
   int flags = 0;
      
   if( flag_string.length() > 0 ) {
      boost::regex re( "\\s+\\|\\s+" );
      boost::sregex_token_iterator token( flag_string.begin(), flag_string.end(), re, -1 );
      boost::sregex_token_iterator token_end;
      
      while( token != token_end ) {
         // figure out which encoding this is trying to tell us
         // Regrettably, this duplicates stuff in the class definition. No other way to make
         // it static..
         /** Flag: write outdegrees using &gamma; coding (default). */
         if( *token == "OUTDEGREES_GAMMA" ) {
            flags |= OUTDEGREES_GAMMA;
         } else if( *token == "OUTDEGREES_DELTA" ) {
            flags |= OUTDEGREES_DELTA;
         } else if( *token == "BLOCKS_GAMMA" ) {
            flags |= BLOCKS_GAMMA;
         }else if( *token == "BLOCKS_DELTA" ) {
            flags |= BLOCKS_DELTA;
         }else if( *token == "RESIDUALS_GAMMA" ) {
            flags |= RESIDUALS_GAMMA;
         }else if( *token == "RESIDUALS_ZETA" ) {
            flags |= RESIDUALS_ZETA;
         }else if( *token == "RESIDUALS_DELTA" ) {
            flags |= RESIDUALS_DELTA;
         }else if( *token == "RESIDUALS_NIBBLE" ) {
            flags |= RESIDUALS_NIBBLE;
         }else if( *token == "REFERENCES_GAMMA" ) {
            flags |= REFERENCES_GAMMA;
         }else if( *token == "REFERENCES_DELTA" ) {
            flags |= REFERENCES_DELTA;
         }else if( *token == "REFERENCES_UNARY" ) {
            flags |= REFERENCES_UNARY;
         }else if( *token == "BLOCK_COUNT_GAMMA" ) {
            flags |= BLOCK_COUNT_GAMMA;
         }else if( *token == "BLOCK_COUNT_DELTA" ) {
            flags |= BLOCK_COUNT_DELTA;
         }else if( *token == "BLOCK_COUNT_UNARY" ) {
            flags |= BLOCK_COUNT_UNARY;
         }else if( *token == "OFFSETS_GAMMA" ) {
            flags |= OFFSETS_GAMMA;
         }else if( *token == "OFFSETS_DELTA" ) {
            flags |= OFFSETS_DELTA;
         } else {
            assert(0);
         }
         
         ++token;
      } // while loop
   }

   return flags;
}

//crée un nouveau graphe en chargeant le graphe compressé à partir du disque > offset_step =0 : ne pas charger les offset , offset_step=-1 ne pas charger le fichier graphe
// pm : le progress mètre utilisé lors du chargement ou bien null , retourne le graphe spécifié 
graph::graph_ptr graph::load( string basename, int offset_step, std::ostream* log ) {
   graph::graph_ptr bob( new graph() );
   bob->load_internal(basename, offset_step, log);
   
   return bob;
}
   
// crée un nouveau graphe en chargeant un graphe compressé du disque avec tous ses offsets 
graph::graph_ptr graph::load( string basename, std::ostream* log ) {
   return graph::load( basename, 1, log );
}

// crée un nouveau graphe en chargeant un graphe compressé du disque sans ses offsets  
graph::graph_ptr graph::load_sequential( string basename, std::ostream* log ) {
   return graph::load( basename, 0, log );
}

// crée un nouveau graphe en chargeant les métédata du fichier du graphe compressé 
graph::graph_ptr graph::load_offline( string basename, std::ostream* log ) {
   return graph::load( basename, -1, log );
}

//  charge le fichier du graphe compressé depuis le disque dans ce graphe. cette méthode est appelée seulement sur les graphe nouvellement créés 
void graph::load_internal( string basename, int offset_step, std::ostream* log )  {
   int i;
   
   this->offset_step = offset_step;
   this->basename = basename;
   
   // First of all, we read the property file to get the relevant data.
   ifstream property_file( (basename + ".properties").c_str() );
   properties props;
   props.load( property_file );
   property_file.close();
   
   // We parse the properties and perform some consistency check and assignments.
   set_flags( string_to_flags( props.get_property( "compressionflags" ) ) );
   
   //assert( props.has_property( "version" ) );
   // assert( atoi( props.get_property( "version" ).c_str() ) <= BVGRAPH_VERSION );
   //throw new IOException( "This graph uses format " + props.getProperty( "version" ) + ",
   //but this class can understand only graphs up to format " + BVGRAPH_VERSION );;
   
   n = atol( props.get_property( "nodes" ).c_str() );
   m = atol( props.get_property( "arcs" ).c_str() );
   
   // TODO spurious replacement here?
   window_size = atoi( props.get_property( "windowsize" ).c_str() ); 
   max_ref_count = atoi( props.get_property( "maxrefcount" ).c_str() );
   min_interval_length = atoi( props.get_property( "minintervallength" ).c_str() );
     
   if ( props.has_property( "zetak" ) ) 
      zeta_k = atoi( props.get_property( "zetak" ).c_str() );
   
   // Soft check due to previous usage of toString() instead of getName() This will
   // fail if the graph is not anything but a bv graph, but whatever.  //if ( !
   // props.get_property( "graphclass" ).endsWith( this.getClass().getName() ) )
   // //throw new IOException( "This class (" + this.getClass().getName() + ") cannot
   // load a graph stored using " + props.getProperty( "graphclass" ) );
   
//    ibitstream offset_ibs;
   
   ibitstream offset_ibs( basename + ".offsets", STD_BUFFER_SIZE );

//    if ( offset_step > 0 ) 
//       offset_ibs = ibitstream( basename + ".offsets", STD_BUFFER_SIZE );
   
   if ( offset_step == 0 || offset_step == 1 ) {
      // No permutation is required: the graph file is loaded as such
      ifstream fis( (basename + ".graph").c_str() );
      
      // read the whole graph into memory
      //pm.print( "Loading graph..." );
      //pm.items_name( "bytes" );
      //pm.start();
      
      // Going to take the (somewhat risky) position that we'll be able to fit this in
      // memory - that is, that the (compressed) file is less than 4 GB (on a 32-bit
      // system).  This seems safe.

      // TODO - change this later?
      
      unsigned file_size = boost::filesystem::file_size( basename + ".graph" );
      
      //if ( fis.getChannel().size() <= Integer.MAX_VALUE ) {
      graph_memory.resize( file_size );
  
      for( unsigned pos = 0; pos < file_size; pos++ ) {
         graph_memory[pos] = fis.get();
      }
    
//      copy( istream_iterator<unsigned char>( fis ), istream_iterator<unsigned char>(), 
//            graph_memory.begin() );
      
#ifdef HARDCORE_DEBUG
      cerr << "==================================================\n";
      cerr << "GRAPH MEMORY LOADED; first 50 bytes : \n";
      for( int j = 0; j < 50; j++ ) {
         cerr << utils::int_to_binary( graph_memory[j], 8 ) << " ";
         if( (j + 1) % 10 == 0 )
            cerr << "\n";
      }
      cerr << "\n";
      for( int j = 0; j < 50; j++ ) {
         cerr << utils::byte_as_hex( graph_memory[j] ) << " ";
         if( (j + 1) % 10 == 0 )
            cerr << "\n";
      }
      cerr << "\n";
#endif
      fis.close();
      in_memory = true;
//         graph_stream = NULL;
      //}
      //else graphStream = new FastMultiByteArrayInputStream( fis, fis.getChannel().size() );
      
      //pm.stop();
      //pm.count( in_memory ? graph_memory.length : graphStream.length );
      //pm.count( graph_memory.size() ); // TODO fix this.
      //pm.print( "Done." + pm.as_str() );
      
      
      if ( offset_step == 1 ) {
         // read offsets, if required
         
         offset.resize( n + 1 );
         
         //pm.print( "Loading offsets..." );
         //pm.items_name( "deltas" );
         //pm.start();
         
         long off = 0;
         for( i = 0; i <= n; i++ ) {
            offset[ i ] = off = read_offset( offset_ibs ) + off;
            cerr << "offset[" << i << "] = " << offset[i] << endl;
         }
            
         //pm.stop();
         //pm.count( offset.size() );
         //pm.print( "Done. " + pm.as_str() );
      }
   }
   else if ( offset_step > 1 ) {
      in_memory = true;
         
      assert( false ); // can't happen, for now.
      // We must permute the graph file, so to load offsets only partially    
     
   }
      
   //if ( offsetIbs != null ) offsetIbs.close();
      
   // We finally create the outdegreeIbs and, if needed, the two caches
   if ( offset_step >= 0 ) {
      //outdegreeIbs = in_memory ? new InputBitStream( graph_memory ): new InputBitStream( new FastMultiByteArrayInputStream( graphStream ) );
      outdegree_ibs.attach( graph_memory_ptr );
   }
      
   if ( offset_step > 1 ) {
      outdegree_cache.resize( offset_step );
      offset_cache.resize( offset_step );
   }
      
   //return this;
}

// mise à jour des flags et des attributs de compression 
void graph::set_flags( int flags ) {
   this->flags = flags;
   if ( ( flags & 0xF ) != 0 ) outdegree_coding = flags & 0xF;
   // ALL of the following used to be >>>
   if ( ( ( unsigned(flags) >> 4 ) & 0xF ) != 0 ) block_coding = ( unsigned(flags) >> 4 ) & 0xF;
   if ( ( ( unsigned(flags) >> 8 ) & 0xF ) != 0 ) residual_coding = ( unsigned(flags) >> 8 ) & 0xF;
   if ( ( ( unsigned(flags) >> 12 ) & 0xF ) != 0 ) reference_coding = ( unsigned(flags) >> 12 ) & 0xF;
   if ( ( ( unsigned(flags) >> 16 ) & 0xF ) != 0 ) block_count_coding = ( unsigned(flags) >> 16 ) & 0xF;
   if ( ( ( unsigned(flags) >> 20 ) & 0xF ) != 0 ) offset_coding = ( unsigned(flags) >> 20 ) & 0xF;
}


//methode de compression par intervalle ( extraire les listes d'intervales des lstes des noeuds x 
int graph::intervalize( const vector<int>& x, int min_interval, vector<int>& left, vector<int>& len, vector<int>& residuals ) {
   int n_interval = 0;
   int x_size = x.size();
   int i, j;
   
   left.clear(); 
   len.clear(); 
   residuals.clear();
   
   for( i = 0; i < x_size; i++ ) {
      j = 0;
      if ( i < x_size - 1 && x[ i ] + 1 == x[ i + 1 ] ) {
         do 
            j++; 
         while( i + j < x_size - 1 && x[ i + j ] + 1 == x[ i + j + 1 ] );
         j++;
         // Now j is the number of integers in the interval.
         if ( j >= min_interval ) {
            left.push_back( x[ i ] );
            len.push_back( j );
            n_interval++;
            i += j - 1;
         }
      }
      if ( j < min_interval ) residuals.push_back( x[ i ] );
   }
   return n_interval;
}

// compression differentielle de la liste des successeurs d'un noeud donnée  
int graph::differentially_compress( obitstream& obs, int curr_node, int ref, vector<unsigned int>& ref_list, int ref_len, vector<unsigned int>& curr_list,  int curr_len, bool for_real )
{
#ifndef CONFIG_FAST
   lg() << LEVEL_DEBUG << "differentially_compress( " << curr_node << ", " << ref << ", "
        << ref_len << ", " << curr_len << ", " << for_real << " )\n";
#endif

   // Bits already written onto the output bit stream
   long written_bits_at_start = obs.get_written_bits();

   // We build the list of blocks copied and skipped (alternatively) from the previous list.
   int i, j = 0, k = 0, prev = 0, curr_block_len = 0;
   bool copying = true;

   // This guarantees that we will not try to differentially compress when ref == 0.
   if ( ref == 0 ) 
      ref_len = 0; 

   extras.clear();
   blocks.clear();

   // j is the index of the next successor of the current node we must examine
   // k is the index of the next successor of the reference node we must examine
   // copying is true iff we are producing a copy block (instead of an ignore block)
   // curr_block_len is the number of entries (in the reference list) we have already
   // copied/ignored (in the current block)

   while( j < curr_len && k < ref_len ) {
#ifndef CONFIG_FAST
      lg() << "Top of loop.\n";
#endif

      if ( copying ) { // First case: we are currectly copying entries from the reference list
         if ( curr_list[ j ] > ref_list[ k ] ) {
            /* If while copying we trespass the current element of the reference list,
               we must stop copying. */
            blocks.push_back( curr_block_len );
            copying = false;
            curr_block_len = 0;
         }
         else if ( curr_list[ j ] < ref_list[ k ] ) {
            /* If while copying we find a non-matching element of the reference list which
               is larger than us, we can just add the current element to the extra list
               and move on. j gets increased. */
            extras.push_back( curr_list[ j++ ] );
         }
         else { // currList[ j ] == refList[ k ]
            /* If the current elements of the two lists are equal, we just increase the
               block length.  both j and k get increased. */
            j++;
            k++;
            curr_block_len++;
         }
      }
      else { // Second case: we are currently skipping entries from the reference list
         if ( curr_list[ j ] < ref_list[ k ] ) {
            /* If we did not trespass the current element of the reference list, we just
               push_back the current element to the extra list and move on. j gets increased. */
            extras.push_back( curr_list[ j++ ] );
         }
         else if ( curr_list[ j ] > ref_list[ k ] ) {
            /* If we trespassed the currented element of the reference list, we
               increase the block length. k gets increased. */
            k++;
            curr_block_len++;
         }
         else { // currList[ j ] == refList[ k ]
            /* If we found a match we flush the current block and start a new copying phase. */
            blocks.push_back( curr_block_len );
            copying = true;
            curr_block_len = 0;
         }
      }
   } // end while
     
   /* We do not record the last block. The only case when we have to enqueue the last
    * block's length is when we were copying and we did not copy up to the end of the
    * reference list.
    */
   if ( copying && k < ref_len ) 
      blocks.push_back( curr_block_len );

   // If there are still missing elements, we add them to the extra list.
   while( j < curr_len ) extras.push_back( curr_list[ j++ ] );

   // We store locally the resulting arrays for faster access.
   int block_count = blocks.size(), extra_count = extras.size();

   // If we have a nontrivial reference window we write the reference to the reference list.
   if ( window_size > 0 ) 
      write_reference( obs, ref );

#ifndef CONFIG_FAST
   lg() << LEVEL_EVERYTHING << "Just called write_reference().\n";
#endif

//   if ( STATS && for_real ) reference_stats.println( ref );

   // Then, if the reference is not void we write the length of the copy list.
   if ( ref != 0 ) {
      write_block_count( obs, block_count );
             
//      if ( STATS && forReal ) blockCountStats.println( blockCount );

      // Then, we write the copy list; all lengths except the first one are decremented.
      if ( block_count > 0 ) {
         write_block( obs, blocks[ 0 ] );
         for( i = 1; i < block_count; i++ ) 
            write_block( obs, blocks[ i ] - 1 );

//          if ( STATS && forReal ) {
//             blockStats.println( block[ 0 ] ); 
//             for( i = 1; i < blockCount; i++ ) blockStats.println( block[ i ] - 1 );
//          }
      }
   }

#ifndef CONFIG_FAST
   lg() << LEVEL_EVERYTHING << "Got to this point, about to write extras if necessary.\n";
#endif

   // Finally, we write the extra list.
   if ( extra_count > 0 ) {

      vector<int> residual;
      int residual_count;

      if ( min_interval_length != NO_INTERVALS ) {
         // If we are to produce interval, we first compute them.
         int interval_count = intervalize( extras, min_interval_length, left, len, residuals );
                     
         // We write the number of intervals.
         obs.write_gamma( interval_count );
                     
//         if ( STATS && forReal ) intervalCountStats.println( intervalCount );
                     
         // We write out the intervals.
         for( i = 0; i < interval_count; i++ ) {
            if ( i == 0 ) {
               prev = left[i];
               obs.write_gamma( utils::int2nat( prev - curr_node ) );
            }
            else 
               obs.write_gamma( left[i] - prev - 1 );

            prev = left[i] + len[i];
            obs.write_gamma( len[ i ] - min_interval_length );
         }
                     
//          if ( STATS && forReal ) 
//             for( i = 0; i < intervalCount; i++ ) {
//                if ( i == 0 ) leftStats.println( Fast.int2nat( ( prev = left.getInt( i ) ) - currNode ) );
//                else leftStats.println( left.getInt( i ) - prev - 1 );
//                prev = left.getInt( i ) + len.getInt( i );
//                lenStats.println( len.getInt( i ) - min_interval_length );
//             }
                     
                     
         residual = residuals;
         residual_count = residuals.size();
      }
      else {
         residual = extras;
         residual_count = extras.size();
      }

#ifndef CONFIG_FAST
      lg() << LEVEL_EVERYTHING << "Done writing extras.\n";
#endif
                              
//      if ( STATS && forReal ) residualCountStats.println( residualCount );

      // Now we write out the residuals, if any
      if ( residual_count != 0 ) {
         prev = residual[0];
#ifndef CONFIG_FAST
         lg() << LEVEL_EVERYTHING << "about to write residual "
              << utils::int2nat( prev ) << " (used int2nat)\n";
#endif
         write_residual( obs, utils::int2nat( prev - curr_node ) );

         for( i = 1; i < residual_count; i++ ) {
//             if ( residual[ i ] == prev ) 
//                throw new IllegalArgumentException( "Repeated successor " + prev + " in successor list of node " + currNode );
            assert( residual[i] != prev );

#ifndef CONFIG_FAST
            lg() << LEVEL_EVERYTHING << "about to write residual "
                 << residual[i] - prev - 1 << "\n";
#endif
            write_residual( obs, residual[ i ] - prev - 1 );
            prev = residual[ i ];
         }
             
//          if ( STATS && forReal ) {
//             residualStats.println( Fast.int2nat( ( prev = residual[ 0 ] ) - currNode ) );
//             for( i = 1; i < residualCount; i++ ) {
//                residualStats.println( residual[ i ] - prev - 1 );
//                prev = residual[ i ];
//             }
//          }
      }             
   }

#ifndef CONFIG_FAST
   lg() << "Returning...\n";
#endif

   return (int)( obs.get_written_bits() - written_bits_at_start );
}

// écrire un graphe offline en tulisant son nom 'basename' 
void graph::store_offline_graph( webgraph::ascii_graph::offline_graph g, double fileSize,std::string prefix_compress, std::string prefix_order,string basename, int window_size, int max_ref_count, int min_interval_length, int zeta_k, int flags, ostream* log ) {

#ifndef CONFIG_FAST      
   register_logger( "webgraph", LEVEL_MAX );

   lg() << LEVEL_DEBUG << "store_offline_graph( " << prefix_compress<< prefix_order <<basename << ".txt" << ", " << window_size << ", "
        << max_ref_count << ", " << min_interval_length << ", " 
        << zeta_k << ", " << flags << ") " << "\n";
#endif

   boost::shared_ptr<graph> me( new graph() );  
    
   if ( window_size != -1 ) 
      me->window_size = window_size;
      
   if ( max_ref_count != -1 ) 
      me->max_ref_count = max_ref_count;
      
   if ( min_interval_length != -1 ) 
      me->min_interval_length = min_interval_length;
    
   if ( zeta_k != -1 ) 
      me->zeta_k = zeta_k;
      
   me->set_flags( flags );
   // TODO change this
   me->store_offline_graph_internal( g, fileSize ,prefix_compress , prefix_order,basename, log );
}


// écrire le graphe donné en utilisant son nom "basename", les paramètres de compression et les flags 
void graph::store_offline_graph_internal( webgraph::ascii_graph::offline_graph olg,double fileSize,std::string prefix_compress, std::string prefix_order, string basename, ostream* log ) {
   
   
   boost::shared_ptr<ostream> nos( new ofstream("/dev/null") );

#ifndef CONFIG_FAST
   lg() << LEVEL_DEBUG << "store_offline_graph_internal( " << prefix_compress << prefix_order <<basename <<".txt"<< " )\n";
#endif
  
   obitstream bit_count( nos, 0  );

   typedef webgraph::ascii_graph::offline_graph graph_type;

   unsigned int outd;
   int curr_node, curr_index, j, best, best_index, cand, t = 0, n = olg.get_num_nodes();
   long bit_offset = 0;
   

   string folder1 = "..\\BVTest\\data\\" + basename + "\\"+ prefix_compress + prefix_order;
   

   int val = DirectedGraph::dirExists(folder1);
   if (val != 0) {
	   if (val == -1) { cerr << "Path Error ..." << endl; exit(0); }
	   else { mkdir(folder1.c_str()); }
   }

   obitstream graph_obs(folder1+ "\\" + prefix_compress + prefix_order + basename + ".graph", STD_BUFFER_SIZE );
   obitstream offset_obs(folder1 + "\\" + prefix_compress + prefix_order + basename + ".offsets", STD_BUFFER_SIZE );


   int cyclic_buffer_size = window_size + 1;

   // Cyclic array of previous lists.
   vector<vector<unsigned int> > lst( cyclic_buffer_size );
   
   for( vector<vector<unsigned int> >::iterator i = lst.begin(); i != lst.end(); i++ ) 
      i->resize( INITIAL_SUCCESSOR_LIST_LENGTH );
   
   // For each list, its length.
   vector<int> list_len( cyclic_buffer_size );

   // For each list, the depth of its references.
   vector<int> ref_count( cyclic_buffer_size );
   
   long tot_ref = 0, tot_dist = 0, tot_links = 0;

   boost::shared_ptr<boost::progress_display> pp;

   if( log != NULL ) {
      *log << "Compressing graph...\n";
      pp.reset( new boost::progress_display( olg.get_num_nodes(), *log ) );
   }

   clock_t tStart = clock(); 

   // We iterate over the nodes of graph
   graph_type::node_iterator node_itor, node_itor_end;
   for ( tie( node_itor, node_itor_end) = olg.get_vertex_iterator(); node_itor != node_itor_end;  ++node_itor ) {
      // curr_node is the currently examined node, of outdegree outd, with index currIndex  (within the cyclic array)
      curr_node = *node_itor;

	 // cout << "curr node : " << curr_node << endl;
   
#ifndef CONFIG_FAST
      lg() << LEVEL_EVERYTHING << "Current node : " << curr_node;
#endif

      // TODO get this to work with both asciigraph, bvgraph, and BGL
      // find the lcd and stick with it....
      outd = webgraph::ascii_graph::outdegree(node_itor); // get the number of successors of currNode
      curr_index = curr_node % cyclic_buffer_size;

#ifndef CONFIG_FAST
      lg() << LEVEL_EVERYTHING << ", which has " << outd << " outlinks.\n";
      
      // We write the current offset to the offset stream
      lg() << LEVEL_EVERYTHING << "About to write the offset - "
           << (int)( graph_obs.get_written_bits() - bit_offset )
           << "( " << graph_obs.get_written_bits() << ", " << bit_offset << " )\n";
#endif

      write_offset( offset_obs, (int)( graph_obs.get_written_bits() - bit_offset ) );

      //      if ( STATS ) offsetStats.println( graphObs.written_bits() - bit_offset );

      bit_offset = graph_obs.get_written_bits();

      // We write the node outdegree
      write_outdegree( graph_obs, outd );

      // if ( STATS ) outdegreeStats.println( outd );

      if ( outd > lst[ curr_index ].size() ) 
         lst[ curr_index ].resize( outd );

      // The successor list we are going to compress and write out
      lst[curr_index] = ascii_graph::successors( node_itor );
      
      list_len[ curr_index ] = outd;

      if ( outd > 0 ) {
   
         // Now we check the best candidate for compression.
         best = std::numeric_limits<int>::max();
         best_index = -1;
   
         ref_count[ curr_index ] = -1;
   
         for( j = 0; j < cyclic_buffer_size; j++ ) {
            cand = ( curr_node - j + cyclic_buffer_size ) % cyclic_buffer_size;
            if ( ref_count[ cand ] < max_ref_count && list_len[ cand ] != 0
                 && ( t = differentially_compress( bit_count, curr_node, j, lst[ cand ], 
                                                   list_len[ cand ], lst[ curr_index ], 
                                                   list_len[ curr_index ], false ) ) < best ) {
               best = t;
               best_index = cand;
            }
         }
#ifndef CONFIG_FAST
         lg() << LEVEL_EVERYTHING << "best = " << best << ", best_index = " << best_index << "\n";
#endif

         assert( best_index >= 0 );
      
         ref_count[ curr_index ] = ref_count[ best_index ] + 1;
      
         differentially_compress( graph_obs, curr_node, ( curr_node - best_index + cyclic_buffer_size ) % 
                                  cyclic_buffer_size, lst[ best_index ], 
                                  list_len[ best_index ], lst[ curr_index ], list_len[ curr_index], true );
                             
         tot_links += outd;
         tot_ref += ref_count[ curr_index ];
         tot_dist += ( curr_node - best_index + cyclic_buffer_size ) % cyclic_buffer_size;
      }
    
      
      if ( ( curr_node + 1 ) % 1000000 == 0 ) 
         *log << "["
              << "bits/link=" << double(graph_obs.get_written_bits()) / ((tot_links != 0) ? tot_links : 1 )
              << ", bits/node=" << (double)graph_obs.get_written_bits() / ( curr_node )
              << ", avgref=" << ( double )tot_ref / curr_node
              << ", avgdist=" << ( double )tot_dist / curr_node
              << "]" << endl;

      //if ( pm != null ) pm.update();
      if( pp != NULL )
         ++(*pp);
   }
   double executionTime = (double)(clock() - tStart) / CLOCKS_PER_SEC;
   pp.reset();
  
   // We write the final offset to the offset stream.
   write_offset( offset_obs, (int)( graph_obs.get_written_bits() - bit_offset ) );
  
   // Finally, we save all data related to this graph in a property file.
   // TODO check these names
   properties props;
   props.set_property( "basename", basename );
   props.set_property( "nodes", utils::to_string(n) );
   props.set_property( "arcs", utils::to_string(tot_links) );
   props.set_property( "windowsize", utils::to_string(window_size) );
   props.set_property( "maxrefcount", utils::to_string(max_ref_count) );
   props.set_property( "minintervallength", utils::to_string(min_interval_length) );
   if ( residual_coding == webgraph::compression_flags::ZETA )  props.set_property( "zetak", utils::to_string(zeta_k) );
   props.set_property( "compressionflags", flags_to_string( flags ) );
   props.set_property( "avgref", utils::to_string( (double)tot_ref / n )  );
   props.set_property( "avgdist", utils::to_string(double(tot_dist)/n ) );
   props.set_property( "bitsperlink", utils::to_string( ( double )graph_obs.get_written_bits() / tot_links ) );
   props.set_property( "bitspernode", utils::to_string( ( double )graph_obs.get_written_bits() / n ) );
   props.set_property("executionTime_(s)", utils::to_string(executionTime));
   props.set_property("taille initiale", utils::to_string((double)(fileSize)  ));
   props.set_property("taille finale", utils::to_string((double)(graph_obs.get_written_bits() / 8)));
   
   props.set_property("CompressionRatio_(%)", utils::to_string((double)(graph_obs.get_written_bits() / (fileSize * 8)) * 100));

   //props.set_property( "version", utils::to_string(BVGRAPH_VERSION) );
   
   ofstream property_file( (folder1 + "\\" + prefix_compress + prefix_order + basename + ".properties").c_str() );
   props.store( property_file, "BVGraph properties" );
   
   property_file.close();
   
}
   
// Ecrire le fichier offset dans un flux obs, obs : le flux dans lequel on écrit les offset, pm : le progresse-metre 
void graph::write_offsets( obitstream& obs, ostream* log ) {
   node_iterator node_itor, end;
   
   tie( node_itor, end ) = get_node_iterator( 0 );
   
   int n = get_num_nodes();
   
   long last_offset = 0;
   while( n-- != 0 ) {
      // We fetch the current position of the underlying input bit stream, which is at the
      // start of the next node.
      write_offset( obs, (int)( node_itor.ibs->get_read_bits() - last_offset ) );
      last_offset = node_itor.ibs->get_read_bits();
      ++node_itor;
      bv_graph::outdegree( node_itor );
      successor_vector( node_itor );
//      if ( pm != null ) pm.update();
   }
   write_offset( obs, (int)( node_itor.ibs->get_read_bits() - last_offset ) );
}
   

} 
} 





