#include <algorithm>
#include "HyperEdge.h"
#include "Link.h"
#include "Variable.h"

HyperEdge::HyperEdge()
	: centerOfGravity(0)
{

}

double HyperEdge::getSpan() const
{ 
	if(links.size()>0) {
		double minPosition =
			std::min_element(this->links.begin(),this->links.end(),compare_positions)->variable->position();

		double maxPosition =
			std::max_element(this->links.begin(),this->links.end(),compare_positions)->variable->position();

		return maxPosition - minPosition;
	} else return 0;
}

bool compareSpan(const HyperEdge* e1, const HyperEdge* e2) {
	return e1->getSpan() < e2->getSpan();
};

void order_pre_post( std::vector<Variable*>& variables, std::vector<HyperEdge*>& edges ) {
	
	computeCOG(edges);
	std::sort(edges.begin(),edges.end(),compareSpan); // sort HyperEdges to have the smallest spans at the begin

	for(std::vector<HyperEdge*>::reverse_iterator it = edges.rbegin(); it != edges.rend(); ++it) // starts with the big span edges
	{
		HyperEdge& edge = **it;
		// insert condition to do the ordering such as "if(getSpan(edge)< 3% * totalSpan)"
		std::sort(edge.links.begin(),edge.links.end(),compare_relations);
		for(unsigned int i=0; i<edge.links.size(); i++) {
			Variable* v = edge.links[i].variable;
			v->position() = edge.centerOfGravity + i/double(edge.links.size());
		}
	}
	sortVariables(variables);
	refreshPositions(variables);
};
