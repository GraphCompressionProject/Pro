#pragma once
#include "Snap.h"
#include <string>
#include <fstream>
#include <iostream>
//#include <sstream>

template <class TNodeData>
class TNodeNetModule {
public:
	typedef TNodeData TNodeDat;
	typedef TNodeNetModule<TNodeData> TNet;
	typedef TPt<TNet> PNet;
public:
	class TNode {
	private:
		TInt Id;
		TNodeData NodeDat;
		char Type;
		TPt<TNodeNetModule<TStr>> ModuleContents;
	public:
		TIntV InNIdV, OutNIdV;



	public:
		TNode() : Id(-1), NodeDat(), InNIdV(), OutNIdV() {
			ModuleContents = TNodeNetModule<TStr>::New();
		}
		TNode(const int& NId) : Id(NId), NodeDat(), InNIdV(), OutNIdV() { ModuleContents = TNodeNetModule<TStr>::New(); }
		TNode(const int& NId, const TNodeData& NodeData) : Id(NId), NodeDat(NodeData), InNIdV(), OutNIdV() { ModuleContents = TNodeNetModule<TStr>::New(); }

		//here
		TNode(const int& NId, const TNodeData& NodeData, char moduletType) : Id(NId), NodeDat(NodeData), InNIdV(), OutNIdV(), Type(moduletType) { ModuleContents = TNodeNetModule<TStr>::New(); }


		//here 

		TNode(const int& NId, const TNodeData& NodeData, char moduletType, TPt<TNodeNetModule<TStr>> myModuleContents) : Id(NId), NodeDat(NodeData), InNIdV(), OutNIdV(), Type(moduletType) {
			ModuleContents = TNodeNetModule<TStr>::New();
			ModuleContents = myModuleContents;
		}

		//here
		//here
		TNode(const int& NId, char moduletType, TPt<TNodeNetModule<TNodeData>> myModuleContents) : Id(NId), InNIdV(), OutNIdV(), Type(moduletType) {
			//ModuleContents = TNodeNetModule<TStr>::New();
			ModuleContents = myModuleContents;
		}

		TNode(const int& NId, char moduletType, TPt<TNodeNetModule<TNodeData>> myModuleContents, TNodeData myLabel) : Id(NId), NodeDat(myLabel), InNIdV(), OutNIdV(), Type(moduletType) {
			ModuleContents = TNodeNetModule<TStr>::New();
			ModuleContents = myModuleContents;
		}

		TNode(const TNode& Node) : Id(Node.Id), NodeDat(Node.NodeDat), InNIdV(Node.InNIdV), OutNIdV(Node.OutNIdV) { }
		TNode(TSIn& SIn) : Id(SIn), NodeDat(SIn), InNIdV(SIn), OutNIdV(SIn) { }

		void Save(TSOut& SOut) const { Id.Save(SOut);  NodeDat.Save(SOut);  InNIdV.Save(SOut);  OutNIdV.Save(SOut); }

		int GetId() const { return Id; }
		TInt& GetId() { return Id; }

		int GetDeg() const { return GetInDeg() + GetOutDeg(); }
		int GetInDeg() const { return InNIdV.Len(); }
		int GetOutDeg() const { return OutNIdV.Len(); }
		const TNodeData& GetDat() const { return NodeDat; }
		TNodeData& GetDat() { return NodeDat; }

		//here
		const char& GetType() const { return Type; }
		char& GetType() { return Type; }
		const TPt<TNodeNetModule<TStr>>& GetModuleContent() const { return ModuleContents; }
		TPt<TNodeNetModule<TStr>>& GetModuleContent() { return ModuleContents; }


		int GetInNId(const int& NodeN) const { return InNIdV[NodeN]; }
		int GetOutNId(const int& NodeN) const { return OutNIdV[NodeN]; }
		int GetNbrNId(const int& NodeN) const { return NodeN < GetOutDeg() ? GetOutNId(NodeN) : GetInNId(NodeN - GetOutDeg()); }
		bool IsInNId(const int& NId) const { return InNIdV.SearchBin(NId) != -1; }
		bool IsOutNId(const int& NId) const { return OutNIdV.SearchBin(NId) != -1; }
		bool IsNbrNId(const int& NId) const { return IsOutNId(NId) || IsInNId(NId); }
		void LoadShM(TShMIn& MStream) {
			Id = TInt(MStream);
			NodeDat = TNodeData(MStream);
			InNIdV.LoadShM(MStream);
			OutNIdV.LoadShM(MStream);
		}
		bool operator < (const TNode& Node) const { return NodeDat < Node.NodeDat; }
		friend class TNodeNetModule<TNodeData>;
	};

	/// Node iterator. Only forward iteration (operator++) is supported.
	class TNodeI {
	private:
		typedef typename THash<TInt, TNode>::TIter THashIter;
		THashIter NodeHI;
		TNodeNetModule *Net;
	public:
		TNodeI() : NodeHI(), Net(NULL) { }
		TNodeI(const THashIter& NodeHIter, const TNodeNetModule* NetPt) : NodeHI(NodeHIter), Net((TNodeNetModule *)NetPt) { }
		TNodeI(const TNodeI& NodeI) : NodeHI(NodeI.NodeHI), Net(NodeI.Net) { }
		TNodeI& operator = (const TNodeI& NodeI) { NodeHI = NodeI.NodeHI; Net = NodeI.Net; return *this; }
		/// Increment iterator.
		TNodeI& operator++ (int) { NodeHI++;  return *this; }
		bool operator < (const TNodeI& NodeI) const { return NodeHI < NodeI.NodeHI; }
		bool operator == (const TNodeI& NodeI) const { return NodeHI == NodeI.NodeHI; }

		/// Returns ID of the current node.
		int GetId() const { return NodeHI.GetDat().GetId(); }
		TInt& GetId() { return NodeHI.GetDat().GetId(); }

		/// Returns degree of the current node.
		int GetDeg() const { return NodeHI.GetDat().GetDeg(); }
		/// Returns in-degree of the current node.
		int GetInDeg() const { return NodeHI.GetDat().GetInDeg(); }
		/// Returns out-degree of the current node.
		int GetOutDeg() const { return NodeHI.GetDat().GetOutDeg(); }
		/// Returns ID of NodeN-th in-node (the node pointing to the current node). ##TNodeNetModule::TNodeI::GetInNId
		int GetInNId(const int& NodeN) const { return NodeHI.GetDat().GetInNId(NodeN); }
		/// Returns ID of NodeN-th out-node (the node the current node points to). ##TNodeNetModule::TNodeI::GetOutNId
		int GetOutNId(const int& NodeN) const { return NodeHI.GetDat().GetOutNId(NodeN); }
		/// Returns ID of NodeN-th neighboring node. ##TNodeNetModule::TNodeI::GetNbrNId
		int GetNbrNId(const int& NodeN) const { return NodeHI.GetDat().GetNbrNId(NodeN); }
		/// Tests whether node with ID NId points to the current node.
		bool IsInNId(const int& NId) const { return NodeHI.GetDat().IsInNId(NId); }
		/// Tests whether the current node points to node with ID NId.
		bool IsOutNId(const int& NId) const { return NodeHI.GetDat().IsOutNId(NId); }
		/// Tests whether node with ID NId is a neighbor of the current node.
		bool IsNbrNId(const int& NId) const { return IsOutNId(NId) || IsInNId(NId); }
		const TNodeData& operator () () const { return NodeHI.GetDat().NodeDat; }
		TNodeData& operator () () { return NodeHI.GetDat().GetDat(); }
		const TNodeData& GetDat() const { return NodeHI.GetDat().GetDat(); }
		TNodeData& GetDat() { return NodeHI.GetDat().GetDat(); }



		//here 
		const char& GetType() const { return NodeHI.GetDat().GetType(); }
		char& GetType() { return NodeHI.GetDat().GetType(); }
		const TPt<TNodeNetModule<TStr>>& GetModuleContent() const { return  NodeHI.GetDat().GetModuleContent(); }
		TPt<TNodeNetModule<TStr>>& GetModuleContent() { return  NodeHI.GetDat().GetModuleContent(); }


		const TNodeData& GetInNDat(const int& NodeN) const { return Net->GetNDat(GetInNId(NodeN)); }
		TNodeData& GetInNDat(const int& NodeN) { return Net->GetNDat(GetInNId(NodeN)); }
		const TNodeData& GetOutNDat(const int& NodeN) const { return Net->GetNDat(GetOutNId(NodeN)); }
		TNodeData& GetOutNDat(const int& NodeN) { return Net->GetNDat(GetOutNId(NodeN)); }
		const TNodeData& GetNbrNDat(const int& NodeN) const { return Net->GetNDat(GetNbrNId(NodeN)); }
		TNodeData& GetNbrNDat(const int& NodeN) { return Net->GetNDat(GetNbrNId(NodeN)); }
		friend class TNodeNetModule<TNodeData>;
	};

	/// Edge iterator. Only forward iteration (operator++) is supported.
	class TEdgeI {
	private:
		TNodeI CurNode, EndNode;
		int CurEdge;
	public:
		TEdgeI() : CurNode(), EndNode(), CurEdge(0) { }
		TEdgeI(const TNodeI& NodeI, const TNodeI& EndNodeI, const int& EdgeN = 0) : CurNode(NodeI), EndNode(EndNodeI), CurEdge(EdgeN) { }
		TEdgeI(const TEdgeI& EdgeI) : CurNode(EdgeI.CurNode), EndNode(EdgeI.EndNode), CurEdge(EdgeI.CurEdge) { }
		TEdgeI& operator = (const TEdgeI& EdgeI) { if (this != &EdgeI) { CurNode = EdgeI.CurNode;  EndNode = EdgeI.EndNode;  CurEdge = EdgeI.CurEdge; }  return *this; }
		/// Increment iterator.
		TEdgeI& operator++ (int) {
			CurEdge++; if (CurEdge >= CurNode.GetOutDeg()) {
				CurEdge = 0;  CurNode++;
				while (CurNode < EndNode && CurNode.GetOutDeg() == 0) { CurNode++; }
			}  return *this;
		}
		bool operator < (const TEdgeI& EdgeI) const { return CurNode < EdgeI.CurNode || (CurNode == EdgeI.CurNode && CurEdge < EdgeI.CurEdge); }
		bool operator == (const TEdgeI& EdgeI) const { return CurNode == EdgeI.CurNode && CurEdge == EdgeI.CurEdge; }
		/// Gets edge ID. Always returns -1 since only edges in multigraphs have explicit IDs.
		int GetId() const { return -1; }
		/// Gets the source node of an edge.
		int GetSrcNId() const { return CurNode.GetId(); }
		/// Gets the destination node of an edge.
		int GetDstNId() const { return CurNode.GetOutNId(CurEdge); }
		const TNodeData& GetSrcNDat() const { return CurNode.GetDat(); }
		TNodeData& GetDstNDat() { return CurNode.GetOutNDat(CurEdge); }
		friend class TNodeNetModule<TNodeData>;
	};

protected:
	TNode & GetNode(const int& NId) { return NodeH.GetDat(NId); }

protected:
	TCRef CRef;
	TInt MxNId;
	THash<TInt, TNode> NodeH;

private:
	class TNodeFunctor {
	public:
		TNodeFunctor() {}
		void operator() (TNode* n, TShMIn& ShMIn) { n->LoadShM(ShMIn); }
	};
private:
	void LoadNetworkShM(TShMIn& ShMIn) {
		MxNId = TInt(ShMIn);
		TNodeFunctor f;
		NodeH.LoadShM(ShMIn, f);
	}

public:
	TNodeNetModule() : CRef(), MxNId(0), NodeH() { }
	/// Constructor that reserves enough memory for a network of Nodes nodes and Edges edges.
	explicit TNodeNetModule(const int& Nodes, const int& Edges) : MxNId(0) { Reserve(Nodes, Edges); }
	TNodeNetModule(const TNodeNetModule& NodeNet) : MxNId(NodeNet.MxNId), NodeH(NodeNet.NodeH) { }
	/// Constructor that loads the network from a (binary) stream SIn.
	TNodeNetModule(TSIn& SIn) : MxNId(SIn), NodeH(SIn) { }
	virtual ~TNodeNetModule() { }
	/// Saves the network to a (binary) stream SOut.
	virtual void Save(TSOut& SOut) const { MxNId.Save(SOut);  NodeH.Save(SOut); }
	/// Static constructor that returns a pointer to the network. Call: TPt <TNodeNetModule<TNodeData> > Net = TNodeNetModule<TNodeData>::New().
	static PNet New() { return PNet(new TNodeNetModule()); }
	/// Static constructor that loads the network from a stream SIn and returns a pointer to it.
	static PNet Load(TSIn& SIn) { return PNet(new TNodeNetModule(SIn)); }
	/// Static constructor that loads the network from shared memory. ##TNodeNetModule::LoadShM(TShMIn& ShMIn)
	static PNet LoadShM(TShMIn& ShMIn) {
		TNodeNetModule* Network = new TNodeNetModule();
		Network->LoadNetworkShM(ShMIn);
		return PNet(Network);
	}
	/// Allows for run-time checking the type of the network (see the TGraphFlag for flags).
	bool HasFlag(const TGraphFlag& Flag) const;
	TNodeNetModule& operator = (const TNodeNetModule& NodeNet) {
		if (this != &NodeNet) { NodeH = NodeNet.NodeH;  MxNId = NodeNet.MxNId; }  return *this;
	}
	// nodes
	/// Returns the number of nodes in the network.
	int GetNodes() const { return NodeH.Len(); }
	/// Adds a node of ID NId to the network. ##TNodeNetModule::AddNode

	//---------------------------------------------
	int AddNode(int NId = -1);
	/// Adds a node of ID NId to the network, noop if the node already exists. ##TNodeNetModule::AddNodeUnchecked
	int AddNodeUnchecked(int NId = -1);
	/// Adds a node of ID NId and node data NodeDat to the network. ##TNodeNetModule::AddNode-1
	int AddNode(int NId, const TNodeData& NodeDat);
	/// Adds a node NodeI and its node data to the network.
	int AddNode(const TNodeI& NodeI) { return AddNode(NodeI.GetId(), NodeI.GetDat()); }

	//here
	int AddNode(int NId, const TNodeData& NodeDat, char Type);


	int AddNode(int NId, const TNodeData& NodeDat, char Type, TPt<TNodeNetModule<TStr>>  ModuleContents);
	//here : cas voisins 
	int AddNode(char moduleType, TPt<TNodeNetModule<TNodeData>> superNodeContent);

	// lable ou label+ voisins
	int AddNode(char moduleType, TPt<TNodeNetModule<TNodeData>> superNodeContent, TNodeData label);

	/// Deletes node of ID NId from the network. ##TNodeNetModule::DelNode
	virtual void DelNode(const int& NId);
	/// Deletes node of ID NodeI.GetId() from the network.
	void DelNode(const TNode& NodeI) { DelNode(NodeI.GetId()); }
	//-----------------------------------------------------------


	/// Tests whether ID NId is a node.
	bool IsNode(const int& NId) const { return NodeH.IsKey(NId); }
	/// Returns an iterator referring to the first node in the network.
	TNodeI BegNI() const { return TNodeI(NodeH.BegI(), this); }
	/// Returns an iterator referring to the past-the-end node in the network.
	TNodeI EndNI() const { return TNodeI(NodeH.EndI(), this); }
	/// Returns an iterator referring to the node of ID NId in the network.
	TNodeI GetNI(const int& NId) const { return TNodeI(NodeH.GetI(NId), this); }
	/// Returns node element for the node of ID NId in the network.
	const TNode& GetNode(const int& NId) const { return NodeH.GetDat(NId); }
	/// Sets node data for the node of ID NId in the network.
	void SetNDat(const int& NId, const TNodeData& NodeDat);
	/// Returns node data for the node of ID NId in the network.
	TNodeData& GetNDat(const int& NId) { return NodeH.GetDat(NId).NodeDat; }
	/// Returns node data for the node of ID NId in the network.
	const TNodeData& GetNDat(const int& NId) const { return NodeH.GetDat(NId).NodeDat; }
	/// Returns an ID that is larger than any node ID in the network.
	int GetMxNId() const { return MxNId; }

	// edges
	/// Returns the number of edges in the network.
	int GetEdges() const;

	//---------------------------------------------------------
	/// Adds an edge from node SrcNId to node DstNId to the network. ##TNodeNetModule::AddEdge
	int AddEdge(const int& SrcNId, const int& DstNId);
	int 	AddEdge2(const int& SrcNId, const int& DstNId);
	/// Adds an edge from EdgeI.GetSrcNId() to EdgeI.GetDstNId() to the network.
	int AddEdge(const TEdgeI& EdgeI) { return AddEdge(EdgeI.GetSrcNId(), EdgeI.GetDstNId()); }
	/// Deletes an edge from node IDs SrcNId to DstNId from the network. ##TNodeNetModule::DelEdge
	void DelEdge(const int& SrcNId, const int& DstNId, const bool& IsDir = true);
	/// Tests whether an edge from node IDs SrcNId to DstNId exists in the network.
	//-------------------------------------------------------------------


	bool IsEdge(const int& SrcNId, const int& DstNId, const bool& IsDir = true) const;
	/// Returns an iterator referring to the first edge in the network.
	TEdgeI BegEI() const { TNodeI NI = BegNI();  while (NI < EndNI() && NI.GetOutDeg() == 0) NI++;  return TEdgeI(NI, EndNI()); }
	/// Returns an iterator referring to the past-the-end edge in the network.
	TEdgeI EndEI() const { return TEdgeI(EndNI(), EndNI()); }
	/// Not supported/implemented!
	TEdgeI GetEI(const int& EId) const;
	/// Returns an iterator referring to edge (SrcNId, DstNId) in the network.
	TEdgeI GetEI(const int& SrcNId, const int& DstNId) const;

	/// Returns an ID of a random node in the network.
	int GetRndNId(TRnd& Rnd = TInt::Rnd) { return NodeH.GetKey(NodeH.GetRndKeyId(Rnd, 0.8)); }
	/// Returns an interator referring to a random node in the network.
	TNodeI GetRndNI(TRnd& Rnd = TInt::Rnd) { return GetNI(GetRndNId(Rnd)); }
	/// Gets a vector IDs of all nodes in the network.
	void GetNIdV(TIntV& NIdV) const;

	/// Tests whether the network is empty (has zero nodes).
	bool Empty() const { return GetNodes() == 0; }
	/// Deletes all nodes and edges from the network.
	void Clr(const bool& DoDel = true, const bool& ResetDat = true) {
		MxNId = 0;  NodeH.Clr(DoDel, -1, ResetDat);
	}
	/// Reserves memory for a network of Nodes nodes and Edges edges.
	void Reserve(const int& Nodes, const int& Edges) { if (Nodes > 0) { NodeH.Gen(Nodes / 2); } }
	/// Sorts nodes by node IDs.
	void SortNIdById(const bool& Asc = true) { NodeH.SortByKey(Asc); }
	/// Sorts nodes by node data.
	void SortNIdByDat(const bool& Asc = true) { NodeH.SortByDat(Asc); }
	/// Defragments the network. ##TNodeNetModule::Defrag
	void Defrag(const bool& OnlyNodeLinks = false);
	/// Checks the network data structure for internal consistency. ##TNodeNetModule::IsOk
	bool IsOk(const bool& ThrowExcept = true) const;

	friend class TPt<TNodeNetModule<TNodeData> >;
};

// set flags
namespace TSnap {
	template <class TNodeData> struct IsDirected<TNodeNetModule<TNodeData> > { enum { Val = 1 }; };
	template <class TNodeData> struct IsNodeDat<TNodeNetModule<TNodeData> > { enum { Val = 1 }; };
}

template <class TNodeData>
bool TNodeNetModule<TNodeData>::HasFlag(const TGraphFlag& Flag) const {
	return HasGraphFlag(typename TNet, Flag);
}

template <class TNodeData>
int TNodeNetModule<TNodeData>::AddNode(int NId) {
	if (NId == -1) {
		NId = MxNId;  MxNId++;
	}
	else {
		IAssertR(!IsNode(NId), TStr::Fmt("NodeId %d already exists", NId));
		MxNId = TMath::Mx(NId + 1, MxNId());
	}
	NodeH.AddDat(NId, TNode(NId));
	return NId;
}

template <class TNodeData>
int TNodeNetModule<TNodeData>::AddNodeUnchecked(int NId) {
	if (NId == -1) {
		NId = MxNId;  MxNId++;
	}
	else {
		if (IsNode(NId)) { return -1; }
		MxNId = TMath::Mx(NId + 1, MxNId());
	}
	NodeH.AddDat(NId, TNode(NId));
	return NId;
}


// ajouter un nouveau noeud (super noeud)
template <class TNodeData>
int TNodeNetModule<TNodeData>::AddNode(char moduleType, TPt<TNodeNetModule<TNodeData>> superNodeContent)
{
	int NId = MxNId;  MxNId++;
	NodeH.AddDat(NId, TNode(NId, moduleType, superNodeContent));
	return NId;
}

// ajouter un nouveau noeud (super noeud)
template <class TNodeData>
int TNodeNetModule<TNodeData>::AddNode(char moduleType, TPt<TNodeNetModule<TNodeData>> superNodeContent, TNodeData label)
{
	int NId = MxNId;  MxNId++;
	NodeH.AddDat(NId, TNode(NId, moduleType, superNodeContent, label));
	return NId;
}


template <class TNodeData>
int TNodeNetModule<TNodeData>::AddNode(int NId, const TNodeData& NodeDat) {
	if (NId == -1) {
		NId = MxNId;  MxNId++;
	}
	else {
		IAssertR(!IsNode(NId), TStr::Fmt("NodeId %d already exists", NId));
		MxNId = TMath::Mx(NId + 1, MxNId());
	}
	NodeH.AddDat(NId, TNode(NId, NodeDat));
	return NId;
}

template <class TNodeData>
int TNodeNetModule<TNodeData>::AddNode(int NId, const TNodeData& NodeDat, char Type) {
	if (NId == -1) {
		NId = MxNId;  MxNId++;
	}
	else {
		IAssertR(!IsNode(NId), TStr::Fmt("NodeId %d already exists", NId));
		MxNId = TMath::Mx(NId + 1, MxNId());
	}
	NodeH.AddDat(NId, TNode(NId, NodeDat, Type));
	return NId;
}

template <class TNodeData>
int TNodeNetModule<TNodeData>::AddNode(int NId, const TNodeData& NodeDat, char Type, TPt<TNodeNetModule<TStr>> ModuleContents) {
	if (NId == -1) {
		NId = MxNId;  MxNId++;
	}
	else {
		IAssertR(!IsNode(NId), TStr::Fmt("NodeId %d already exists", NId));
		MxNId = TMath::Mx(NId + 1, MxNId());
	}
	NodeH.AddDat(NId, TNode(NId, NodeDat, Type, ModuleContents));
	return NId;

}

template <class TNodeData>
void TNodeNetModule<TNodeData>::DelNode(const int& NId) {
	{ TNode& Node = GetNode(NId);
	for (int e = 0; e < Node.GetOutDeg(); e++) {
		const int nbr = Node.GetOutNId(e);
		if (nbr == NId) { continue; }
		TNode& N = GetNode(nbr);
		int n = N.InNIdV.SearchBin(NId);
		if (n != -1) { N.InNIdV.Del(n); }
	}
	for (int e = 0; e < Node.GetInDeg(); e++) {
		const int nbr = Node.GetInNId(e);
		if (nbr == NId) { continue; }
		TNode& N = GetNode(nbr);
		int n = N.OutNIdV.SearchBin(NId);
		if (n != -1) { N.OutNIdV.Del(n); }
	} }
	NodeH.DelKey(NId);
}

template <class TNodeData>
void TNodeNetModule<TNodeData>::SetNDat(const int& NId, const TNodeData& NodeDat) {
	IAssertR(IsNode(NId), TStr::Fmt("NodeId %d does not exist.", NId).CStr());
	NodeH.GetDat(NId).NodeDat = NodeDat;
}

template <class TNodeData>
int TNodeNetModule<TNodeData>::GetEdges() const {
	int edges = 0;
	for (int N = NodeH.FFirstKeyId(); NodeH.FNextKeyId(N);) {
		edges += NodeH[N].GetOutDeg();
	}
	return edges;
}

template <class TNodeData>
int TNodeNetModule<TNodeData>::AddEdge(const int& SrcNId, const int& DstNId) {
	IAssertR(IsNode(SrcNId) && IsNode(DstNId), TStr::Fmt("%d or %d not a node.", SrcNId, DstNId).CStr());
	if (IsEdge(SrcNId, DstNId)) { return -2; }
	GetNode(SrcNId).OutNIdV.AddSorted(DstNId);
	GetNode(DstNId).InNIdV.AddSorted(SrcNId);
	return -1; // no edge id
}

template <class TNodeData>
int TNodeNetModule<TNodeData>::AddEdge2(const int& SrcNId, const int& DstNId) {
	//int rand = random() % 100;
	const int MAX = 26;
	std::string alphabet;
	
	for (int k = 0; k < MAX; k++) {
		alphabet.push_back( 'a' + k);
	}
	std::string res;
	res = alphabet[rand() % MAX];

	//TStr randstr = boost::lexical_cast<string>(rand);
	if (IsEdge(SrcNId, DstNId)) { return -2; }
	if (IsNode(SrcNId)) {
		if (IsNode(DstNId))
		{
			AddEdge(SrcNId, DstNId);
		}
		else {
			AddNode(DstNId, res.c_str(), 'X');
			AddEdge(SrcNId, DstNId);
		}
	}
	else

	{
		std::string res2;
		res2 = alphabet[rand() % MAX];
		AddNode(SrcNId, res2.c_str(), 'X');
		if (IsNode(DstNId))
		{
			AddEdge(SrcNId, DstNId);
		}
		else {
			std::string res3;
			res3 = alphabet[rand() % MAX];
			AddNode(DstNId, res3.c_str(), 'X');
			AddEdge(SrcNId, DstNId);
		}
	}

	return -1; // no edge id
}

template <class TNodeData>
void TNodeNetModule<TNodeData>::DelEdge(const int& SrcNId, const int& DstNId, const bool& IsDir) {
	IAssertR(IsNode(SrcNId) && IsNode(DstNId), TStr::Fmt("%d or %d not a node.", SrcNId, DstNId).CStr());
	GetNode(SrcNId).OutNIdV.DelIfIn(DstNId);
	GetNode(DstNId).InNIdV.DelIfIn(SrcNId);
	if (!IsDir) {
		GetNode(DstNId).OutNIdV.DelIfIn(SrcNId);
		GetNode(SrcNId).InNIdV.DelIfIn(DstNId);
	}
}

template <class TNodeData>
bool TNodeNetModule<TNodeData>::IsEdge(const int& SrcNId, const int& DstNId, const bool& IsDir) const {
	if (!IsNode(SrcNId) || !IsNode(DstNId)) { return false; }
	if (IsDir) { return GetNode(SrcNId).IsOutNId(DstNId); }
	else { return GetNode(SrcNId).IsOutNId(DstNId) || GetNode(DstNId).IsOutNId(SrcNId); }
}

template <class TNodeData>
void TNodeNetModule<TNodeData>::GetNIdV(TIntV& NIdV) const {
	NIdV.Reserve(GetNodes(), 0);
	for (int N = NodeH.FFirstKeyId(); NodeH.FNextKeyId(N); ) {
		NIdV.Add(NodeH.GetKey(N));
	}
}

template <class TNodeData>
typename TNodeNetModule<TNodeData>::TEdgeI  TNodeNetModule<TNodeData>::GetEI(const int& SrcNId, const int& DstNId) const {
	const TNodeI SrcNI = GetNI(SrcNId);
	const int NodeN = SrcNI.NodeHI.GetDat().OutNIdV.SearchBin(DstNId);
	if (NodeN == -1) { return EndEI(); }
	return TEdgeI(SrcNI, EndNI(), NodeN);
}

template <class TNodeData>
void TNodeNetModule<TNodeData>::Defrag(const bool& OnlyNodeLinks) {
	for (int n = NodeH.FFirstKeyId(); NodeH.FNextKeyId(n); ) {
		TNode& Node = NodeH[n];
		Node.InNIdV.Pack();  Node.OutNIdV.Pack();
	}
	if (!OnlyNodeLinks && !NodeH.IsKeyIdEqKeyN()) {
		NodeH.Defrag();
	}
}

template <class TNodeData>
bool TNodeNetModule<TNodeData>::IsOk(const bool& ThrowExcept) const {
	bool RetVal = true;
	for (int N = NodeH.FFirstKeyId(); NodeH.FNextKeyId(N); ) {
		const TNode& Node = NodeH[N];
		if (!Node.OutNIdV.IsSorted()) {
			const TStr Msg = TStr::Fmt("Out-neighbor list of node %d is not sorted.", Node.GetId());
			if (ThrowExcept) { EAssertR(false, Msg); }
			else { ErrNotify(Msg.CStr()); } RetVal = false;
		}
		if (!Node.InNIdV.IsSorted()) {
			const TStr Msg = TStr::Fmt("In-neighbor list of node %d is not sorted.", Node.GetId());
			if (ThrowExcept) { EAssertR(false, Msg); }
			else { ErrNotify(Msg.CStr()); } RetVal = false;
		}
		// check out-edges
		int prevNId = -1;
		for (int e = 0; e < Node.GetOutDeg(); e++) {
			if (!IsNode(Node.GetOutNId(e))) {
				const TStr Msg = TStr::Fmt("Out-edge %d --> %d: node %d does not exist.",
					Node.GetId(), Node.GetOutNId(e), Node.GetOutNId(e));
				if (ThrowExcept) { EAssertR(false, Msg); }
				else { ErrNotify(Msg.CStr()); } RetVal = false;
			}
			if (e > 0 && prevNId == Node.GetOutNId(e)) {
				const TStr Msg = TStr::Fmt("Node %d has duplidate out-edge %d --> %d.",
					Node.GetId(), Node.GetId(), Node.GetOutNId(e));
				if (ThrowExcept) { EAssertR(false, Msg); }
				else { ErrNotify(Msg.CStr()); } RetVal = false;
			}
			prevNId = Node.GetOutNId(e);
		}
		// check in-edges
		prevNId = -1;
		for (int e = 0; e < Node.GetInDeg(); e++) {
			if (!IsNode(Node.GetInNId(e))) {
				const TStr Msg = TStr::Fmt("In-edge %d <-- %d: node %d does not exist.",
					Node.GetId(), Node.GetInNId(e), Node.GetInNId(e));
				if (ThrowExcept) { EAssertR(false, Msg); }
				else { ErrNotify(Msg.CStr()); } RetVal = false;
			}
			if (e > 0 && prevNId == Node.GetInNId(e)) {
				const TStr Msg = TStr::Fmt("Node %d has duplidate in-edge %d <-- %d.",
					Node.GetId(), Node.GetId(), Node.GetInNId(e));
				if (ThrowExcept) { EAssertR(false, Msg); }
				else { ErrNotify(Msg.CStr()); } RetVal = false;
			}
			prevNId = Node.GetInNId(e);
		}
	}
	return RetVal;
}