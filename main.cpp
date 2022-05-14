#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>
#include <set>

class Node{
public:
	int depth = -1, value = -1;
	std::vector<char> state;
	Node(std::vector<char> s) : state(s) { }
	void PrintNode(){
		for(int i = 0; i < state.size() - 1; i++){
			std::cout << state.at(i) << " ";
		}
		std::cout << std::endl << "  ";
		for(int i = 1; i < state.size() - 1; i+= 2){
			std::cout << "|   ";
		}
		std::cout << std::endl << "center: " << state.at(state.size() - 1) << std::endl;
	}
};

class Comparator{
public:
	bool operator()(Node *a, Node *b){
		return a->value + a->depth > b->value + b->depth;
	}
};

class GeneralSearch{
private:
	// FLANDERS spelt around the edges, last index is the center, first index
	// is not connected to the center.
	std::vector<char> solved_state = {'F', 'L', 'A', 'N','D','E','R','S','_'};
	Node *start;
public:
	GeneralSearch(Node *n) : start(n) { }
	
	// Method 1 = Uniform Cost, 2 = Misplaced Tile, 3 = Manhattan Distance
	int Evaluate(std::vector<char> state, int method){
		// Uniform cost returns heuristic of 0 every time
		if(method == 1) return 0;
		std::vector<char>::iterator it;
		int index, dist, ret = 0, size = state.size();
		
		// Misplaced Tile counts the number of incorrect edge nodes
		if(method == 2){
			for(int i = 0; i < size; i++){
				if(state.at(i) != solved_state.at(i) && state.at(i) != '_') ret++;
			}
			return ret;
		}
		
		// Manhattan Distance calculates how many moves are required for each
		// piece to go to its solved state equivalent. Assumes letters in the
		// word are unique and every state has valid letters, which is why I
		// use find().
		for(int i = 0; i < size; i++){
			if(state.at(i) == solved_state.at(i)) continue;
			it = find(solved_state.begin(), solved_state.end(), state.at(i));
			index = it - solved_state.begin();
			
			// Distance travelling across the edges
			dist = abs(index - i);
			dist = std::min(dist, abs(size - dist - 1));
			
			// Edge case for calculating from the center node
			if(i == size - 1) dist = 1 + ((index + 1) % 2);
			
			// Distance travelling through the center
			ret += std::min(dist, 2 + ((index + 1) % 2) + ((i + 1) % 2));
		}
		return ret;
	}
	
	std::vector<Node*> Expand(Node *n){
		std::vector<Node*> ret;
		std::vector<char>::iterator it = find(n->state.begin(), n->state.end(), '_');
		int index = it - n->state.begin();
		std::vector<char> s = n->state;
		
		if(index == n->state.size() - 1){
			// Moving the empty space from the center
			for(int i = 1; i < n->state.size() - 1; i += 2){
				s = n->state;
				std::swap(s.at(i), s.at(s.size() - 1));
				ret.push_back(new Node(s));
			}
		}
		else{
			// Moving the empty space from the edge
			int i = index - 1;
			if(i < 0) i = s.size() - 2;
			std::swap(s.at(index), s.at(i));
			ret.push_back(new Node(s));
			
			s = n->state;
			i = index + 1;
			if(i >= s.size() - 1) i = 0;
			std::swap(s.at(index), s.at(i));
			ret.push_back(new Node(s));
			
			if(index % 2){
				s = n->state;
				i = s.size() - 1;
				std::swap(s.at(index), s.at(i));
				ret.push_back(new Node(s));
			}
		}
		
		//n->leaves = ret;
		return ret;
	}
	
	void Solve(int method){
		std::priority_queue<Node*, std::vector<Node*>, Comparator> frontier;
		std::set<std::vector<char>> repeated_states;
		Node *curr;
		int exp = 0, max_size = 1;
		
		// Evaluate the start node with the selected method, then push it
		// onto the queue.
		start->value = Evaluate(start->state, method);
		start->depth = 0;
		frontier.push(start);
		while(!frontier.empty()){
			curr = frontier.top();
			frontier.pop();
			
			std::cout << "The best state to expand with a g(n) = " << curr->depth << " and h(n) = " << curr->value << " is.." << std::endl;
			curr->PrintNode();
			
			if(curr->state == solved_state){
				std::cout << "Goal state achieved after " << exp << " expansions" << std::endl;
				std::cout << "Max queue size: " << max_size << std::endl;
				std::cout << "Solution depth: " << curr->depth << std::endl;
				return;
			}
			
			//Expand and add children to the queue
			std::vector<Node*> nodes = Expand(curr);
			for(int i = 0; i < nodes.size(); i++){
				if(repeated_states.count(nodes.at(i)->state)){
					continue;
				}
				nodes.at(i)->depth = curr->depth + 1;
				nodes.at(i)->value = Evaluate(nodes.at(i)->state, method);
				frontier.push(nodes.at(i));
				repeated_states.insert(nodes.at(i)->state);
			}
			if(frontier.size() > max_size) max_size = frontier.size();
			exp++;
			delete curr;
		}
		
		std::cout << "Failed to find a solution after " << exp << " expansions" << std::endl;
		
	}
};

int main(){
	std::string input, c;
	std::vector<char> v;
	int num;
	std::cout << "Type 1 to use a default puzzle, or 2 to create a custom" << std::endl;
	std::cin >> num;
	if(num == 1){
		std::cout << "Choose a difficulty from 1 to 4" << std::endl;
		std::cin >> num;
		if(num == 1) input = "FLAN_DRSE";
		else if(num == 2) input = "FLAENDRS_";
		else if(num == 3) input = "FLAENRSD_";
		else if(num == 4) input = "DFLANERS_";
	}
	else{
		std::cout << "Enter the capital letters as they appear around the wheel, let '_' denote the empty space" << std::endl;
		std::cin >> input;
		std::cout << "Enter the letter in the center" << std::endl;
		std::cin >> c;
		input += c;
	}
	
	for(int i = 0; i < input.length(); i++){
		v.push_back(input[i]);
	}
	
	Node *start = new Node(v);
	GeneralSearch s(start);
	
	std::cout << "Select algorithm\n1) Uniform Cost Search\n2) Misplaced Tile Heuristic\n3) Manhattan Distance Heuristic" << std::endl;
	std::cin >> num;
	
	s.Solve(num);
	
	return 0;
}