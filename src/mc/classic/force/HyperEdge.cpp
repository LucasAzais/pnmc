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

void order_pre_post( std::vector<Variable*>& variables, std::vector<HyperEdge*>& edges ) {
	for(std::vector<HyperEdge*>::iterator it = edges.begin(); it != edges.end(); ++it)
	{
		HyperEdge& edge = **it;
		// insert condition to do the ordering such as "if(getSpan(edge)< 3% * totalSpan)"
				
	}
};
