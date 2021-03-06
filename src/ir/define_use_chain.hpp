#pragma once

#include "../meow/core.hpp"
#include "define_arrival.hpp"
#include "var_range.hpp"

namespace buaac {

	
	

	
	class Chain : implement fmt::Display {
		using Node = DefineUseNode;
	public:
		Chain(Node def): def(def) {}
		
		Node def;
		vector<Node> uses;

		std::string toString() {
			std::ostringstream buf;
			buf << "{" << def;
			for (int i = 0; i < uses.size(); i++) {
				buf << ", " << uses[i];
			}
			buf << "}";
			return buf.str();
		}

		bool conflict(Chain chain) {
			auto rhs = getDefAndUsesSet();
			auto lhs = chain.getDefAndUsesSet();
			for (auto i = rhs.begin(); i != rhs.end(); i++) {
				if (found(lhs, *i)) {
					return true;
				}
			}
			return false;
		}

		set<Node> getDefAndUsesSet() {
			set<Node> ans;
			ans.insert(ans.begin(), def);
			for (int i = 0; i < uses.size(); i++) {
				ans.insert(uses[i]);
			}
			return ans;
		}

		void sort() {
			std::sort(uses.begin(), uses.end());
			// assert(uses.empty() || def.block_index <= uses[0].block_index);
		}

		void merge(Chain rhs) {
			// Chain new_chain(std::min(lhs.def, rhs.def));
			// if (lhs > rhs) {
				// new_chain.uses.push_back(lhs.def);
			// } else {
				uses.push_back(rhs.def);
			// }

			// for (auto node: lhs.uses) {
				// new_chain.uses.push_back(node);
			// }
			for (auto node: rhs.uses) {
				uses.push_back(node);
			}
			sort();
			// return new_chain;
		}
		
		friend bool operator==(const Chain& lhs, const Chain& rhs) {
			return lhs.def == rhs.def
				&& lhs.uses == rhs.uses;
		}

		friend bool operator!=(const Chain& lhs, const Chain& rhs) {
			return !(lhs == rhs);
		}

		friend bool operator<(const Chain& lhs, const Chain& rhs) {
			if (lhs.def < rhs.def)
				return true;
			if (rhs.def < lhs.def)
				return false;
			return lhs.uses < rhs.uses;
		}

		friend bool operator<=(const Chain& lhs, const Chain& rhs) {
			return !(rhs < lhs);
		}

		friend bool operator>(const Chain& lhs, const Chain& rhs) {
			return rhs < lhs;
		}

		friend bool operator>=(const Chain& lhs, const Chain& rhs) {
			return !(lhs < rhs);
		}
	};

	class Web : implement fmt::Display  {
	public:
		set<Chain> chains;

		std::string toString() {
			std::ostringstream buf;
			buf << "{";
			for (auto i = chains.begin(); i != chains.end(); i++) {
				buf << TOSTR(*i) << ", ";
			}
			buf << "}";
			return buf.str();
		}

		bool canAdd(Chain chain) {
			for (auto i = chains.begin(); i != chains.end(); i++) {
				Chain this_chain = *i;
				if (this_chain.conflict(chain)) {
					return true;
				}
			}
			return false;
		}

		bool canMerge(Web &lhs) {
			for (auto lhs_chain : chains) {
				for (auto rhs_chain: lhs.chains) {
					if (!setAnd(lhs_chain.getDefAndUsesSet(), rhs_chain.getDefAndUsesSet()).empty()) {
						return true;
					}
				}
			}
			return false;
		}

		void merge(Web &lhs) {
			for (auto chain : lhs.chains) {
				chains.insert(chain);
			}
		}

		void sort() {
			set<Chain> new_chains;
			for (auto chain: chains) {
				chain.sort();
				new_chains.insert(chain);
			}
			chains = new_chains;
		}
		
		friend bool operator==(const Web& lhs, const Web& rhs) {
			return lhs.chains == rhs.chains;
		}

		friend bool operator!=(const Web& lhs, const Web& rhs) {
			return !(lhs == rhs);
		}

		friend bool operator<(const Web& lhs, const Web& rhs) {
			return lhs.chains < rhs.chains;
		}

		friend bool operator<=(const Web& lhs, const Web& rhs) {
			return !(rhs < lhs);
		}

		friend bool operator>(const Web& lhs, const Web& rhs) {
			return rhs < lhs;
		}

		friend bool operator>=(const Web& lhs, const Web& rhs) {
			return !(lhs < rhs);
		}
	};

	
	
	class DefineUseChain {
		using Node = DefineUseNode;

		// using Node = string;
		
		DefineArrival define_arrival_;

		// map<string, set<Node>> def_nodes, use_nodes;
		
		map<string, set<Node>> ident_to_defs, ident_to_uses;
		set<string> idents;

		map<string, set<Web>> ident_to_webs;
		map<string, VarRange> ident_to_range;

	public:
		DefineUseChain() {
			
		}

		// using BlockLine = tuple<int, int>;
		
		map<string, VarRange> getRanges() {
			genRanges();
			return ident_to_range;
		}
		
		void genRanges() {
			for (auto ident: idents) {
				ident_to_range[ident] = getRange(ident);
			}
		}

		void generate(FlowGraph &flow_graph) {
			reAddDef();

			define_arrival_.generate(flow_graph);
			genWebs(flow_graph);
			// fixLoop();
		}


		void printChain() {
			define_arrival_.printInOut();

			for (auto i = idents.begin(); i != idents.end(); i++) {
				print("{}: ", *i);
				auto webs = ident_to_webs[*i];
				for (auto j = webs.begin(); j != webs.end(); j++) {
					println("W{}", *j);
				}
			}
		}

		
		void addDef(int block_index, string block_name, int line_number, string ident) {
			idents.insert(ident);
			Node new_node( block_index, block_name, line_number );
			
			auto define_blocks = ident_to_defs[ident];
			for (auto i = define_blocks.begin(); i != define_blocks.end(); i++) {
				if (*i != new_node) {
					define_arrival_.addKill(block_name, TOSTR(*i) );
				}
			}
			ident_to_defs[ident].insert(new_node);
			define_arrival_.addGen(block_name, new_node.toString());
			
		}

		// string getNodeName(string block, string line) {
		// 	return FORMAT("<{}, {}>", block, line);
		// }

		void addUse(int block_index, string block_name, int line_number, string ident) {
			idents.insert(ident);
			Node new_node{ block_index, block_name, line_number };
			
			ident_to_uses[ident].insert(new_node);
			define_arrival_.addName(block_name);
		}


		
		
	private:

		vector<NodeRange> getLoops(FlowGraph &flow_graph) {
			vector<NodeRange> loops;
			for (auto next_edge: flow_graph.getNextNodes()) {
				auto from = next_edge.first;
				for (auto to : next_edge.second) {
					int from_id = flow_graph.getId(from);
					int to_id = flow_graph.getId(to);
					if (to_id <= from_id) {
						loops.push_back(NodeRange(Node(to_id, to, 0), Node(from_id, from, INT32_MAX)));
					}
				}
			}
			return loops;
		}

		void fixChainLoop(Chain &chain, vector<NodeRange> &loops) {
			bool flag;
			do {
				flag = false;
				for (auto loop : loops) {
					for (auto use : chain.uses) {
						if (loop.in(use.block_index, use.line_number) && getFoundIndex(chain.uses, loop.last) == -1) {
							chain.uses.push_back(loop.last);
							flag = true;
							goto fixChainLoop_outside;
						}
					}
				}
			fixChainLoop_outside:;
			} while (flag);
		
			chain.sort();
		}

		VarRange getRange(string ident) {
			auto webs = ident_to_webs[ident];

			// BlockLine ans_min = make_tuple(INT32_MAX, INT32_MAX);
			// BlockLine ans_max = make_tuple(INT32_MIN, INT32_MIN);
			if (webs.empty()) {
				// TODO: fix this
				return VarRange(
					NodeRange(DefineUseNode(0, "Global", 0), DefineUseNode(INT32_MAX, "Global", INT32_MAX))
				);
			}
			auto first = webs.begin()->chains.begin()->def;
			
			VarRange range;

			for (auto web : webs) {
				NodeRange node_range = NodeRange(first);
				for (auto chain : web.chains) {
					node_range = NodeRange::merge(node_range, chain.def);
					for (auto use : chain.uses) {
						node_range = NodeRange::merge(node_range, use);
					}
				}
				range.addRange(node_range);
			}

			return range;
		}

		void reAddDef() {
			for (auto i = idents.begin(); i != idents.end(); i++) {
				auto def_nodes = ident_to_defs[*i];
				for (auto j = def_nodes.begin(); j != def_nodes.end(); j++) {
					for (auto k = def_nodes.begin(); k != def_nodes.end(); k++) {
						if (*j != *k) {
							define_arrival_.addKill(j->block_name, TOSTR(*k));
						}
					}
				}
			}
		}

		void genWebs(FlowGraph &flow_graph) {
			for (auto ident: idents) {
				genWeb(ident, flow_graph);
			}
		}

		void genWeb(string ident, FlowGraph &flow_graph) {
			vector<Chain> chains;
			auto defs = ident_to_defs[ident];
			auto uses = ident_to_uses[ident];

			for (auto i = defs.begin(); i != defs.end(); i++) {
				chains.push_back(Chain(*i));
			}

			for (auto use = uses.begin(); use != uses.end(); use++) {
				for (int chain_i = 0; chain_i < chains.size(); chain_i++) {
					// auto *x = *use;

					if (found(define_arrival_.getIn(use->block_name), chains[chain_i].def.toString())
						|| found(define_arrival_.getOut(use->block_name), chains[chain_i].def.toString())) {
						chains[chain_i].uses.push_back(*use);
					}
				}
			}

#ifndef FORSPEED
			if (!(starts_with(ident, string("__T")) && isdigit(ident[3]))) {
				auto loops = getLoops(flow_graph);
				for (auto& chain : chains) {
					fixChainLoop(chain, loops);
				}
			}
			
#endif

			vector<Chain> old_chains;
			do {
				old_chains = chains;
				for (int i = 0; i < chains.size(); i++) {
					for (int j = 0; j < chains.size(); j++) {
						if (i != j && chains[i].conflict(chains[j])) {
							if (chains[i].def < chains[j].def) {
								chains[i].merge(chains[j]);
								chains.erase(chains.begin() + j);
							} else {
								chains[j].merge(chains[i]);
								chains.erase(chains.begin() + i);
							}
							goto outside_merge_chains;
						}
					}
				}
			outside_merge_chains:;
			} while (old_chains != chains);

			vector<Web> webs;
			for (int i = 0; i < chains.size(); i++) {
				bool flag = true;
				for (int j = 0; j < webs.size(); j++) {
					if (webs[j].canAdd(chains[i])) {
						webs[j].chains.insert(chains[i]);
						flag = false;
						break;
					}
				}
				if (flag) {
					Web web;
					web.chains.insert(chains[i]);
					webs.push_back(web);
				}
			}

			vector<Web> old_webs;
			do {
				old_webs = webs;
				for (int i = 0; i < webs.size(); i++) {
					for (int j = 0; j < webs.size(); j++) {
						if (i != j && webs[i].canMerge(webs[j])) {
							webs[i].merge(webs[j]);
							webs.erase(webs.begin() + j);
							goto outside;
						}
					}
				}
			outside:;
			} while (old_webs != webs);

			for (int i = 0; i < webs.size(); i++) {
				webs[i].sort();
				ident_to_webs[ident].insert(webs[i]);
			}
		}

	};
}
