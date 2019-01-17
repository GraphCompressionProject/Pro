#pragma once
#include "stdafx.h"
#include <list>
#include <stack>
#include "Module.h"
#include "SuperGraph.h"
#include <map>

using namespace std::placeholders;
//using namespace std;

class CompresssAgregation
{
	
public:


public:

	// retourne les noeuds qui ont le meme label que le noeud node1 
	THash<TInt, TNodeNetModule<TStr>::TNodeI> Modulelabel(TNodeNetModule<TStr>::TNodeI node1, SuperGraph  G, TIntSet NoModuleSet) {
		THash<TInt, TNodeNetModule<TStr>::TNodeI> HashTable;
		for (TNodeNetModule<TStr>::TNodeI NI = G.getNodeIteratorBegin(); NI < G.getNodeIteratorEnd() ; NI++)
		{ 
			if (NI != node1) {
				if (!NoModuleSet.IsKey(NI.GetId())) {
					if (NI.GetDat() == node1.GetDat()) {
						HashTable.AddDat(NI.GetId(), NI);
					}
				}
			}
		}
		//HashTable.AddDat(node1.GetId(), node1);
		return HashTable;
	}

	// verifie si deux noeuds ont les meme voisins 
	bool NodeNeignboor(int node1, int node2, SuperGraph G)
	{


		if (!G.isNode(node1) || !G.isNode(node2))
		{
			return false;
			printf("les noeuds n'appartiennent pas au graphe");
		}

		TNodeNetModule<TStr>::TNodeI NI1 = G.graph->GetNI(node1);
		TNodeNetModule<TStr>::TNodeI NI2 = G.graph->GetNI(node2);
		if (NI1.GetDeg() == 0 || NI2.GetDeg() == 0)
		{
			return false;
		}
		else
		{
			TIntSet NSet1, NSet2;

			//printf("le degré du noeud est %d", NI1.GetOutDeg());
			for (int i = 0; i < NI1.GetDeg(); i++) {

				const int nid = NI1.GetNbrNId(i);
				//printf("%d %d \n",i, nid);
				// printf("%d la boucle1\n", nid);
				if (nid != node1 && nid != node2) {
					NSet1.AddKey(nid);
				}
			}
			//printf(" %d taille de nset premier est \n", NSet1.Len());
			bool voisin = true;
			for (int i = 0; i < NI2.GetDeg() && voisin; i++) {
				//printf("%d le degre du noeud\n", NI2.GetDeg());
				const int nid = NI2.GetNbrNId(i);

				//   printf("%d la boucle2 \n", nid);
				if (nid != node1 && !(NSet1.IsKey(nid))) {
					return false;
					//printf("mafhamtch alash");

				}
				if (nid != node1)

					NSet2.AddKey(nid);
			}

			if (NSet1 != NSet2) voisin = false;
			/*if (NSet1.Len() != NSet2.Len()) {

			voisin = false;
			}*/
			return voisin;
		}

	}

	// retourne les noeuds qui ont les meme voisin avec le noeud node1
	THash<TInt, TNodeNetModule<TStr>::TNodeI> Moduleneighborhood(TNodeNetModule<TStr>::TNodeI node1, SuperGraph G, TIntSet NoModuleSet)
	{
		THash<TInt, TNodeNetModule<TStr>::TNodeI> HashTable;
		
		for (TNodeNetModule<TStr>::TNodeI NI = G.getNodeIteratorBegin(); NI < G.getNodeIteratorEnd() ; NI++)
		{
			if (NI != node1) {
				if (!(NoModuleSet.IsKey(NI.GetId()))) {
					
					if (NodeNeignboor(node1.GetId(), NI.GetId(), G)) { HashTable.AddDat(NI.GetId(), NI); }
				}
			}
		}
		
		return HashTable;
	}

	// Agrege les noeuds qui ont les memes voisins et memes labels
	THash<TInt, TNodeNetModule<TStr>::TNodeI> ModuleneighborhoodLabel(TNodeNetModule<TStr>::TNodeI node1, SuperGraph  G, TIntSet NoModuleSet) {
		THash<TInt, TNodeNetModule<TStr>::TNodeI> HashTable;
		for (TNodeNetModule<TStr>::TNodeI NI = G.getNodeIteratorBegin(); NI < G.getNodeIteratorEnd() ; NI++)
		{
			if (NI != node1) {
				if (!NoModuleSet.IsKey(NI.GetId())) {
					if (NI.GetDat() == node1.GetDat()) {
						if (NodeNeignboor(node1.GetId(), NI.GetId(), G)) {
							HashTable.AddDat(NI.GetId(), NI);
						}
					}
				}
			}
		}
		
		return HashTable;
	}

	//tirangle et label 

	
	//agrège les triangles qui ont les emmes labels 
	
	THash<TInt, TNodeNetModule<TStr>::TNodeI> ModuleTriangleLabel(TNodeNetModule<TStr>::TNodeI node1, SuperGraph  G, TIntSet NoModuleSet) {
		THash<TInt, TNodeNetModule<TStr>::TNodeI> HashTable;
		for (int i = 0; i < node1.GetDeg(); i++)
		{
			TNodeNetModule<TStr>::TNodeI node2 = G.getNode(node1.GetNbrNId(i));
			if (node1 != node2) {
				if (!NoModuleSet.IsKey(node2.GetId())) { // si le noeud voisin n'a pas été traité déja et n'a pas été mis dans la liste des noeud qui n'ont pas de module
					if (node1.GetDat() == node2.GetDat()) {
						HashTable.AddDat(node2.GetId(), node2); //sauvegarder le voisin }

					}
				}
			}

		}
		bool stop = false;
		THash<TInt, TNodeNetModule<TStr>::TNodeI>::TIter it;
		THash<TInt, TNodeNetModule<TStr>::TNodeI>::TIter iter = HashTable.BegI();
		if (!HashTable.Empty())
		{
			while (HashTable.Len() != 1 && !stop)
			{
				for (it = HashTable.BegI(); it < HashTable.EndI(); it++)
				{
					if (iter->Dat.GetId() != it->Dat.GetId())
					{
						if (iter->Dat.IsNbrNId(it->Dat.GetId()))
						{
							stop = true;
							break;
						}
					}

				}



				if (!stop)
				{
					HashTable.DelKey(iter.GetKey());
					iter = HashTable.BegI();

				}
			}
		}

		THash<TInt, TNodeNetModule<TStr>::TNodeI> newhash;
		if (stop)
		{

			newhash.AddDat(iter->Dat.GetId(), iter->Dat);
			newhash.AddDat(it->Dat.GetId(), it->Dat);

		}

		return newhash;
	}

	THash<TInt, TNodeNetModule<TStr>::TNodeI> ModuleTriangle(TNodeNetModule<TStr>::TNodeI node1, SuperGraph  G, TIntSet NoModuleSet) {
		THash<TInt, TNodeNetModule<TStr>::TNodeI> HashTable;
		for (int i = 0; i < node1.GetDeg(); i++)
		{
			TNodeNetModule<TStr>::TNodeI node2 = G.getNode(node1.GetNbrNId(i));
			if (node1 != node2) {
				if (!NoModuleSet.IsKey(node2.GetId())) { // si le noeud voisin n'a pas été traité déja et n'a pas été mis dans la liste des noeud qui n'ont pas de module

					HashTable.AddDat(node2.GetId(), node2); //sauvegarder le voisin 
				
				}
			}

		}
		bool stop = false;
		THash<TInt, TNodeNetModule<TStr>::TNodeI>::TIter it;
		THash<TInt, TNodeNetModule<TStr>::TNodeI>::TIter iter = HashTable.BegI();
		if (!HashTable.Empty())
		{
			while (HashTable.Len() != 1 && !stop)
			{
				for (it = HashTable.BegI(); it < HashTable.EndI(); it++)
				{
					if (iter->Dat.GetId() != it->Dat.GetId())
					{
						if (iter->Dat.IsNbrNId(it->Dat.GetId()))
						{
							stop = true;
							break;
						}
					}

				}



				if (!stop)
				{
					HashTable.DelKey(iter.GetKey());
					iter = HashTable.BegI();

				}
			}
		}

		THash<TInt, TNodeNetModule<TStr>::TNodeI> newhash;
		if (stop)
		{

			newhash.AddDat(iter->Dat.GetId(), iter->Dat);
			newhash.AddDat(it->Dat.GetId(), it->Dat);

		}
		
		return newhash;
	}



	
	//agrège les tiangles des quels les noeuds ont les memes label et meme voisin
		
	void copyNode(TNodeNetModule<TStr>::TNodeI node1, TNodeNetModule<TStr>::TNodeI node2)
	{
		node1.GetId() = node2.GetId() ;
		node1.GetDat()=node2.GetDat() ;
		node1.GetType() = node2.GetType();
		node1.GetModuleContent()= node2.GetModuleContent();
	}
	

	TPt<TNodeNetModule<TStr>> HashToModule(THash<TInt, TNodeNetModule<TStr>::TNodeI> HashTable) {
		TPt<TNodeNetModule<TStr>> graph= TNodeNetModule<TStr>::New();

		for (THash<TInt, TNodeNetModule<TStr>::TNodeI>::TIter t = HashTable.BegI(); t < HashTable.EndI(); t++)
		{
			TNodeNetModule<TStr>::TNodeI tnode;
			tnode = t->Dat;
			char tp = t->Dat.GetType();
			
			
			tnode.GetType() = tp;
			TPt<TNodeNetModule<TStr> > graphptit=TNodeNetModule<TStr>::New();
			TPt<TNodeNetModule<TStr> > gr = tnode.GetModuleContent();
			//cout << "je suis en haut" << "";
			if (!(gr->Empty())) {
				//cout << "il est vide" << "";
				for (TNodeNetModule<TStr>::TNodeI it = gr->BegNI(); it < gr->EndNI(); it++)
				{
					graphptit->AddNode(it.GetId(),it.GetDat(),it.GetType(),it.GetModuleContent());
				}
				for (TNodeNetModule<TStr>::TEdgeI ite = gr->BegEI(); ite < gr->EndEI(); ite++)
				{
					graphptit->AddEdge(ite);
				}
			}
		
			
			graph->AddNode(tnode.GetId(), tnode.GetDat(), tp, graphptit);
			
		}

		
		for (THash<TInt, TNodeNetModule<TStr>::TNodeI>::TIter t = HashTable.BegI(); t < HashTable.EndI(); t++)
		{
			
			
			for (int i = 0; i < t->Dat.GetDeg(); i++)
			{
				if (HashTable.IsKey(t->Dat.GetNbrNId(i))  )
				{
					
				if (!(graph->GetNI(t->Dat.GetId()).IsNbrNId(t->Dat.GetNbrNId(i))))
					{
						graph->AddEdge(t->Dat.GetId(), t->Dat.GetNbrNId(i));
				}
					
				}

			}
		}
		for (TNodeNetModule<TStr>::TNodeI it = graph->BegNI(); it < graph->EndNI(); it++)
		{
			//cout << "le type" <<  << "";
		}
		
		return graph;
	}

	
	//fonction qui détermine si le module est connexe
	bool Connexe(TNodeNetModule<TStr> & G)
	{
		vector <int> order;
		map<int, bool> a;
		//initilaisation du vecteur des noeuds et visité à faux 
		for (TNodeNetModule<TStr>::TNodeI NI = G.BegNI(); NI < G.EndNI(); NI++)
		{
			a[NI.GetId()] = false;
		}

		int node = G.BegNI().GetId();
		stack<int> stack;
		stack.push(node);
		while (!stack.empty())
		{
			node = stack.top();
			stack.pop();
			if (a[node] == false)
			{
				order.push_back(node);
				a[node] = true;
			}
			int nid;
			//successeur du noeud courant : 
			for (int i = 0; i < G.GetNI(node).GetDeg(); i++)
			{
				nid = G.GetNI(node).GetNbrNId(i);
				if (a[nid] == false) stack.push(nid);
			}

		}

		if (a.size() == order.size()) return true;
		else return false;

	}
	//fonction qui determine si un module est de type parallele
	bool GraphComplementConnexe(TNodeNetModule<TStr> &G) {

		TNodeNetModule<TStr> gr;
		for (TNodeNetModule<TStr>::TNodeI NI = G.BegNI(); NI < G.EndNI(); NI++)
		{
			for (TNodeNetModule<TStr>::TNodeI NI2 = G.BegNI(); NI2 < G.EndNI(); NI2++)
			{
				if (NI.GetId() != NI2.GetId())
				{
					gr.AddNodeUnchecked(NI.GetId());
					gr.AddNodeUnchecked(NI2.GetId());
					if (!NI.IsNbrNId(NI2.GetId()))
					{
						gr.AddEdge(NI.GetId(), NI2.GetId());
					}
				}
			}

		}
		return Connexe(gr);

	}
	//définie le type du module 
	char TypeModule(TPt<TNodeNetModule<TStr>> &G)
	{
		if (Connexe(*G)) return 'S';
		else if (GraphComplementConnexe(*G)) return 'P';
		else return 'N';
	}
	//suppression liens internes du module 
	void deleteEdges(TPt<TNodeNetModule<TStr>> & G)
	{
		for (TNodeNetModule<TStr>::TNodeI NI = G->BegNI(); NI < G->EndNI(); NI++)
		{
			for (int i = 0; i < NI.GetOutDeg(); i++)
			{
				G->DelEdge(NI.GetId(), NI.GetOutNId(i), true);
			}
		}
	}


	

	//mise à jour du graphe dans le cas ou le module utilise le voisinage
	void MajGraph_voisin(SuperGraph & G, char typeModule , TPt<TNodeNetModule<TStr>> module, THash<TInt, TNodeNetModule<TStr>::TNodeI> myModule ) {

			int nid = G.graph->AddNode(typeModule, module);
			TNodeNetModule<TStr>::TNodeI Modulei = G.graph->GetNI(nid);
			//cout << SuperGraph::ModuleToString(G.graph->GetNI(nid)).str() << endl;

			//cout << SuperGraph::ModuleToString(G.graph->GetNI() << endl;
			TNodeNetModule<TStr>::TNodeI tnode;

			for (THash<TInt, TNodeNetModule<TStr>::TNodeI>::TIter t = myModule.BegI(); t < myModule.EndI(); t++)
			{
				tnode = t->Dat;
				for (int i = 0; i < tnode.GetDeg(); i++)
				{
					if (!myModule.IsKey(tnode.GetNbrNId(i)))
					{
						//cout << "le voisin dans MAJ est " << tnode.GetNbrNId(i) << endl;
						//si le nouveau noeud n'est pas voisin le voisin courant du noeud courant appartenant au module
						if (!Modulei.IsNbrNId(tnode.GetNbrNId(i))) {
							G.graph->AddEdge(nid, tnode.GetNbrNId(i));
						}
					}

				}
				G.graph->DelNode(tnode.GetId());
				
			}

		//	cout << "dans MAJ" << endl;
			//cout << SuperGraph::ModuleToString(G.graph->GetNI(nid)).str();

	}
	
	//mise à jour du graphe dans le cas ou le module utilise le voisinage et label 
	void MajGraph_VoisinLabel(SuperGraph & G, char typeModule, TPt<TNodeNetModule<TStr>> module, THash<TInt, TNodeNetModule<TStr>::TNodeI> myModule )
	{
		//cout << "im here" << endl;
		int nid = G.graph->AddNode(typeModule, module, myModule.BegI().GetDat().GetDat());
		//cout << SuperGraph::ModuleToString(G.graph->GetNI(nid)).str() << endl;
		TNodeNetModule<TStr>::TNodeI tnode;

		for (THash<TInt, TNodeNetModule<TStr>::TNodeI>::TIter t = myModule.BegI(); t < myModule.EndI(); t++)
		{
			tnode = t->Dat;
			for (int i = 0; i < tnode.GetDeg(); i++)
			{
				if (!myModule.IsKey(tnode.GetNbrNId(i)))
				{
					G.graph->AddEdge(nid, tnode.GetNbrNId(i));
				}

			}
			G.graph->DelNode(tnode.GetId());
		}
	}

	void compressGraph(SuperGraph &G, std::function<THash<TInt, TNodeNetModule<TStr>::TNodeI>(TNodeNetModule<TStr>::TNodeI, SuperGraph, TIntSet)> getModule, std::function< void (SuperGraph & , char, TPt<TNodeNetModule<TStr>> , THash<TInt, TNodeNetModule<TStr>::TNodeI>)> MajGraph ,int &nbNode,int &nbLink,int &nbModule)
		
	{
		bool stop = false;
		TNodeNetModule<TStr>::TNodeI node;
		TIntSet NoModuleSet;
		THash<TInt, TNodeNetModule<TStr>::TNodeI> myModule;
		char type;

		while (!stop)
		{
			node = G.getNodeIteratorBegin();
			while (node < G.getNodeIteratorEnd())
			{
				if (NoModuleSet.IsKey(node.GetId())) { // si le noeud est déja traité et n'a pas de module on avance 
					
					node++;
				}
				else // sinon je sors de la boucle avec le noeud non déja traité
				{
					break;
				}
			}
			
			if (NoModuleSet.Len() == G.GetNodes()) return; // si tous les noeud sont traité et non pas de module on sort 
			
			myModule.Clr();
			myModule = getModule(node, G,NoModuleSet); // trouver le super noeud à partir d'un noeud donné ( le noeud courant) 
			//cout << "here" << endl;
			if (myModule.Empty()) { NoModuleSet.AddKey(node.GetId()); } // si le noeud n'appartient à aucun supernoeud => l'ajouter à la liste des noeuds qui n'ont aucun super noeud
			else { 
				//sinon = le noeud appartient a un super noeud 
				myModule.AddDat(node.GetId(), node); //ajouter le noeud courant au super noeud trouvé
				TPt<TNodeNetModule<TStr>> superNodeContent; 
				superNodeContent = HashToModule(myModule);

				type = TypeModule(superNodeContent);  // trouver le type du super-noeud
				//if (type=='P')
				if (type == 'S' || type == 'P') // si série ou parallel supprimer les liens ( pas besoins de les garder)
				{
					deleteEdges(superNodeContent);
				}
				MajGraph(G, type, superNodeContent, myModule); // ajouter le super noeud a graphe et supprimer les noeuds composant le super-noeud du graphe
				
			}

		}
		nbNode = G.GetNodes();
		nbLink = G.GetEdges();
		nbModule = 3;

		
	}
	void compressGraphByNeighberhood(SuperGraph &G)
	{
		 int nbNode = G.GetNodes();
		 int nbLink = G.GetEdges();
		 int nbModule = 0;
		 compressGraph(G, std::bind(&CompresssAgregation::Moduleneighborhood, this, _1, _2, _3), std::bind(&CompresssAgregation::MajGraph_voisin, this, _1, _2, _3, _4), nbNode, nbLink, nbModule);
	}
	void compressGraphByLabel(SuperGraph &G)
	{
		int nbNode = G.GetNodes();
		int nbLink = G.GetEdges();
		int nbModule = 0;
		compressGraph(G, std::bind(&CompresssAgregation::Modulelabel, this, _1, _2, _3), std::bind(&CompresssAgregation::MajGraph_VoisinLabel, this, _1, _2, _3, _4),nbNode,nbLink,nbModule);
	}
	void compressGraphByLabelNeighberhood(SuperGraph &G)
	{

		int nbNode = G.GetNodes();
		int nbLink = G.GetEdges();
		int nbModule = 0;

		compressGraph(G, std::bind(&CompresssAgregation::ModuleneighborhoodLabel, this, _1, _2, _3), std::bind(&CompresssAgregation::MajGraph_VoisinLabel, this, _1, _2, _3, _4),nbNode,nbLink,nbModule);
	}
	void compressGraphByTriangle(SuperGraph &G)
	{
		int nbNode = G.GetNodes();
		int nbLink = G.GetEdges();
		int nbModule = 0;
		compressGraph(G, std::bind(&CompresssAgregation::ModuleTriangle, this, _1, _2, _3), std::bind(&CompresssAgregation::MajGraph_voisin, this, _1, _2, _3, _4),nbNode,nbLink,nbModule);
	}
	void compressGraphByTriangleLabel(SuperGraph &G)
	{
		int nbNode = G.GetNodes();
		int nbLink = G.GetEdges();
		int nbModule = 0;
		compressGraph(G, std::bind(&CompresssAgregation::ModuleTriangleLabel, this, _1, _2, _3), std::bind(&CompresssAgregation::MajGraph_VoisinLabel, this, _1, _2, _3, _4),nbNode,nbLink,nbModule);
	}
	
	 



	
};