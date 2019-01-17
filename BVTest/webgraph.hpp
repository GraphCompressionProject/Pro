#ifndef WEBGRAPH_H_
#define WEBGRAPH_H_

#include <string>
#include <vector>
#include <utility>
#include <climits>

#include <boost/progress.hpp>
#include <boost/shared_ptr.hpp>
#include "types.hpp"
#include "offline_graph.hpp"
#include "compression_flags.hpp"
#include "utility_iterator_base.hpp"
#include "iterator_wrappers.hpp"
#include "node_iterator.hpp"
#include "input_bitstream.hpp"
#include "output_bitstream.hpp"
#include "logger.hpp"


namespace webgraph { namespace bv_graph {

   namespace utility_iterators {
      template<class val_type>
      class residual_iterator;
   }

 // le graphe � compresser 
class graph  
{
  
private:
   

   const static int STD_BUFFER_SIZE = 1024 * 1024;
        
   // les falgs utilis� pour la compression 
   int flags;
   
   //pointeur d'un it�rateur interne de successeurs : les it�rateur interne sont polymorphiques et peuvent �tre manipul� tous � l'aide d'un pointeur 
   typedef boost::shared_ptr<utility_iterators::utility_iterator_base<int> > internal_succ_itor_ptr;

public:
   friend class node_iterator;
   friend class utility_iterators::residual_iterator<int>;
   
   typedef webgraph::bv_graph::node_iterator node_iterator;
   typedef std::pair<node_iterator, node_iterator> node_itor_pair;

   typedef webgraph::bv_graph::iterator_wrappers::java_to_cpp<int> successor_iterator;
   typedef std::pair<successor_iterator, successor_iterator> succ_itor_pair;
   
   const static int BVGRAPH_VERSION = 0;

   //nombre par d�faut des r�f�rences 
   const static int DEFAULT_MAX_REF_COUNT; // initialized in .cpp
   
   // taille par d�faut de la fen�tre "window"
   const static int DEFAULT_WINDOW_SIZE = 10; //7; TODO CHANGE THIS
   
   //taille par d�faut d'un intervalle 
   const static int DEFAULT_MIN_INTERVAL_LENGTH = 3;
  
   //valeur par d�faut de l'offset
   const static int DEFAULT_OFFSET_STEP = 1;
        
   //valeur par d�faut du facteur de codage z�ta 
   const static int DEFAULT_ZETA_K = 5; //3; TODO CHANGE THIS

   //�crire le outdegree en utilisant le codage gamma 
   const static int OUTDEGREES_GAMMA = compression_flags::GAMMA;

   //�crire le outdegree en utilisant delta
   const static int OUTDEGREES_DELTA = compression_flags::DELTA;

   //�crire les copy-block en utilisant gamma 
   const static int BLOCKS_GAMMA = compression_flags::GAMMA << 4;

   //�crire les copy-block en utilisant delta
   const static int BLOCKS_DELTA = compression_flags::DELTA << 4;

   //�crire les r�sidus en utilisant gamma
   const static int RESIDUALS_GAMMA = compression_flags::GAMMA << 8;

   //�crire les r�sidus en utilisant le codage zeta 
   const static int RESIDUALS_ZETA = compression_flags::ZETA << 8;

   //�crire les r�sidus en utilisant le codage delta 
   const static int RESIDUALS_DELTA = compression_flags::DELTA << 8;

   //�crire les r�sidus en utilisant le codage nibble
   const static int RESIDUALS_NIBBLE = compression_flags::NIBBLE << 8;

   //�crire les r�f�rences en utilisant le codage gamma 
   const static int REFERENCES_GAMMA = compression_flags::GAMMA << 12;

   //�crire les r�f�rences en utilisant le codage delta 
   const static int REFERENCES_DELTA = compression_flags::DELTA << 12;

   //�crire les r�f�rences en utilisant le codage unaire 
   const static int REFERENCES_UNARY = compression_flags::UNARY << 12;

   // �crire le block count en utilisant le codage gamma 
   const static int BLOCK_COUNT_GAMMA = compression_flags::GAMMA << 16;

   //�crire le block  count en utilisant le codage delta 
   const static int BLOCK_COUNT_DELTA = compression_flags::DELTA << 16;

   //�crire le block count en utilsant le codage unaire
   const static int BLOCK_COUNT_UNARY = compression_flags::UNARY << 16;

   //�crire les offset en utilisant le codage gamma
   const static int OFFSETS_GAMMA = compression_flags::GAMMA << 20;

   //�crire les offsets en utilisant le codage delta 
   const static int OFFSETS_DELTA = compression_flags::DELTA << 20;


protected:
   
	// taille initial du tableau qui va contenir une liste de successeurs 
   const static int INITIAL_SUCCESSOR_LIST_LENGTH = 1024;
        
   // une valeur pour MIN_INTERVAL _LENGTH qui signifie que la taille est infinie 
   const static int NO_INTERVALS = 0;

   //nom du graphe : en chargeant le graphe avec un offset -1 => causera une exception 
   std::string basename;
        
   // le nombre de noeuds dans le graphe
   long n;

   //le nombre de liens dans le graphe
   long m;

   //si le offsestep n'est pas -1 : le graphe est charg� directement en m�moire
   bool in_memory;

   // le vecteur qui sauvegarde le graphe compress� si in_m�mory= true et offsetStep != -1 
   // This variable is loaded with a copy of the graph file, or with a rearrangement of the latter, depending on whether {@link #offsetStep} 
   // is smaller than or equal to one. If {@link #offsetStep} is -1, this variable is <code>null</code>, and node iterators are generated 
   // by opening streams directly on the graph file.
   boost::shared_ptr< std::vector<byte> > graph_memory_ptr;
   std::vector<byte>& graph_memory;

   //cette valeur est null ssi offset_step <=0 ( i.e : pas de chargement d'offset), 
   // sinon : chaque entr�e i repr�sente l'offset (d�placement) qui repr�sente le d�but du i'�me bloc de listes de successeurs 
   //la derni�re entr�e repr�sente la taille en bits du fichier graphe 
   std::vector<long> offset;

   //nombre de ref�rence max 
   int max_ref_count;

   // taille de "window" : valeur == 0 veut dire pas de codage par r�f�rences 
   int window_size;

   // taille minimale des intervalles 
   int min_interval_length;

   // le pas de l'offset, valeurs : 0 : pour pas d'offset et -1 : pour pas d'offset et pas de graph_memory
   int offset_step;

   // le facteur k pour le codage zeta 
   int zeta_k;

   //le codage des outdegree, par d�faut : gamma
   int outdegree_coding;

   //le codage des copy-block, par d�faut : gamma
   int block_coding;

   //le codage des r�sidus, par d�faut : zeta
   int residual_coding;

   //le codage des r�f�rences, par d�faut : unary
   int reference_coding;

   //le codage des block_count, par d�faut : gamma
   int block_count_coding;

   //le codage des offset , par d�faut : gamma
   int offset_coding;

   //un flux qui encapsule graph_memory ou bien graph_stream. utilis� par la fct outdegree
   mutable ibitstream outdegree_ibs;

   //un cache qui contient les outdegrees [outdegree_cache_start .. outdegree_cache_end[
   mutable std::vector<int> outdegree_cache;
   mutable int outdegree_cache_start; // premier noeud dans le cache 
   mutable int outdegree_cache_end; // dernier noeud + 1 

   // un cache qui contient les offset : [outdegree_cache_start .. offset_cache_end[ /  l'acc�s se fait pas position 
   mutable std::vector<int> offset_cache;
   mutable int offset_cache_end; // dernier noeud dans le cache +1 < outdegree_cache_end 
   
   // vecteurs utilis� par la m�thode difrentially_compress 
   std::vector<int> extras;
   std::vector<int> blocks;
   std::vector<int> len;
   std::vector<int> left;
   std::vector<int> residuals;

#ifndef CONFIG_FAST
   static logs::module_logger& lg() {
      return logs::logger( "webgraph" );
   }
#endif

protected:
   //constructeur : initialisation du graphe 
   graph() : graph_memory_ptr( new std::vector<byte> ), graph_memory( *graph_memory_ptr ), max_ref_count(DEFAULT_MAX_REF_COUNT),
             window_size(DEFAULT_WINDOW_SIZE), min_interval_length(DEFAULT_MIN_INTERVAL_LENGTH), offset_step(DEFAULT_OFFSET_STEP),
             zeta_k(DEFAULT_ZETA_K), outdegree_coding(webgraph::compression_flags::GAMMA), block_coding(webgraph::compression_flags::GAMMA),
             residual_coding(webgraph::compression_flags::ZETA), reference_coding(webgraph::compression_flags::UNARY), block_count_coding(webgraph::compression_flags::GAMMA),
             offset_coding(webgraph::compression_flags::GAMMA), outdegree_cache_start(INT_MAX), outdegree_cache_end(INT_MAX), offset_cache_end(INT_MAX) 
		   {
		#ifndef CONFIG_FAST
			  logs::register_logger( "webgraph", logs::LEVEL_MAX );
		#endif
		   }
        
public:
   //Destructeur 
   virtual ~graph() {
   }
   // getters 
   long get_num_nodes() const;
   long get_num_arcs() const;  
   std::string get_basename() const;
   int get_max_ref_count() const;
   int get_window_size() const;
   int get_offset_step() const;

protected:

	// lire un offset > ibs : l'offset file , retourne : le prochaine offset
   int read_offset( ibitstream& ibs ) const;

   // �crit un offset > obs : offset file , x : l'offset � �crire , retourne : le nombre de bits �crits 
   int write_offset( obitstream& obs, int x ) const;
   
   // lit un outdegree > ibs : graphe file , retourne : le prochain outdegree 
   int read_outdegree( ibitstream& ibs ) const;

   //�crit un outdegree > obs graphe file , d : l'outdegree � �crire, retourne : le nb de bits �crits 
   int write_outdegree( obitstream& obs, int d ) const;
   
   // lit une r�f�rence 
   int read_reference( ibitstream& ibs ) const;

   //�crit une r�f�rence 
   int write_reference( obitstream& obs, int ref ) const;

   //lit un block count 
   int read_block_count( ibitstream& ibs ) const;
   
   //�crit un block count 
   int write_block_count( obitstream& obs, int count ) const;
   
   //lit un block 
   int read_block( ibitstream& ibs ) const;

   //�crit un block
   int write_block( obitstream& obs, int block ) const;

   //lit un r�sidu 
   int read_residual( ibitstream& ibs ) const;

   //�crit un r�sidu 
   int write_residual( obitstream& obs, int residual ) const;

public:
	// retourner le outdegree d'un noeud al�atoire 
   int outdegree( int x ) const;

protected:

	//skip la partie de la liste des successeurs d'un noeud qui vient apr�s le outdegree > ibs : le flux positionn� au d�but de la liste des successeurs
	// x : le noeud dont la liste des sucesseurs va �tre saut� (skiped) , outd:  d(x). 
   void skip_node( ibitstream& ibs, int x, int outd ) const;

   // positionne le flux juste avant la liste des successeurs du noeud donn�e , juste apr�s son outdegree qui est retourn� 
   int position( ibitstream& ibs, int x ) const;

public:

	//retourne un it�rateur de successeurs d'un certain noeud x
   succ_itor_pair get_successors( int x ) const;
   
private:

	//wrapper pour la fct get_successors_internal 
   internal_succ_itor_ptr get_successors_internal( int x ) const;

   //donnant un flux qui repr�sente un fichier graphe, la fct retourne un it�rateur des sucesseurs d'un noeud x 
   internal_succ_itor_ptr get_successors_internal( int x, boost::shared_ptr<ibitstream> ibs,  std::vector< std::vector<int> >& window, std::vector<int>& outd,  std::vector<int>& blockOutdegrees ) const;

public:
	// retourne un it�rateur de noeuds pour scanner le graphe s�quentiellement , commen�ant du noeud donn� 
   std::pair<node_iterator, node_iterator> get_node_iterator( int from ) const;
        
private:

	// mise � jour des flags et des attributs de compressio
   void set_flags( int flags );

   // produit une chaine qui repr�sente les valeurs cod� dans le flag mask donn� 
   static std::string flags_to_string( int flags );

   // produit un flag mask qui correspond � la chaine donn�e 
   static int string_to_flags( std::string flag_string );

public:

   typedef boost::shared_ptr<graph> graph_ptr;
   //cr�e un nouveau graphe en chargeant le graphe compress� � partir du disque > offset_step =0 : ne pas charger les offset , offset_step=-1 ne pas charger le fichier graphe
   // pm : le progress m�tre utilis� lors du chargement ou bien null , retourne le graphe sp�cifi� 
   static graph_ptr load( std::string basename, int offset_step, std::ostream* log = NULL );

   // cr�e un nouveau graphe en chargeant un graphe compress� du disque avec tous ses offsets
   static graph_ptr load( std::string basename, std::ostream* log = NULL );

   // cr�e un nouveau graphe en chargeant un graphe compress� du disque sans ses offsets
   static graph_ptr load_sequential( std::string basename, std::ostream* log = NULL );

   // cr�e un nouveau graphe en chargeant les m�t�data du fichier du graphe compress�
   static graph_ptr load_offline( std::string basename, std::ostream* log = NULL );

protected:
	//  charge le fichier du graphe compress� depuis le disque dans ce graphe. cette m�thode est appel�e seulement sur les graphe nouvellement cr��s 
   void load_internal( std::string basename, int offset_step, std::ostream* log = NULL );

   //methode de compression par intervalle ( extraire les listes d'intervales des lstes des noeuds x 
   static int intervalize( const std::vector<int>& x, int min_interval, std::vector<int>& left, std::vector<int>& len,  std::vector<int>& residuals );
                
private: 
	// compression differentielle de la liste des successeurs d'un noeud donn�e 
   int differentially_compress( obitstream& obs, int current_node, int ref, std::vector<unsigned int>& ref_list, int ref_length,  std::vector<unsigned int>& current_list,  int current_len, bool for_real );
        
public:
	// �crire un graphe offline en tulisant son nom 'basename' 
   static void store_offline_graph( webgraph::ascii_graph::offline_graph graph, double fileSize, std::string prefix_compress, std::string prefix_order, std::string basename, int window_size, int max_ref_count, int min_interval_length, int zeta_k, int flags, std::ostream* log = NULL );

private:
	// �crire le graphe donn� en utilisant son nom "basename", les param�tres de compression et les flags 
   void store_offline_graph_internal( webgraph::ascii_graph::offline_graph graph, double fileSize, std::string prefix_compress, std::string prefix_order ,std::string basename, std::ostream* log = NULL );
        
public:
	// Ecrire le fichier offset dans un flux obs, obs : le flux dans lequel on �crit les offset, pm : le progresse-metre 
   void write_offsets( obitstream& obs, std::ostream* log = NULL );


   
};

} }
#endif 
