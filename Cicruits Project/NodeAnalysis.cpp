#include "Components.h"


bool CheckEssential(Node* node) {
	//Please note : voltageSources should be zero  in node analysis , otherwise you should know what you are doing
	int totalSize = node->Resistors.size() + node->CurrentSource.size() + node->VoltageSource.size();
	if (totalSize < 2)
		return true;
	return false;

}
double CalculateG(Node* node) {
	double TotalG = 0;
	for (std::vector<Component>::iterator it = node->Resistors.begin(); it != node->Resistors.end(); ++it) 
		TotalG += 1/it->Magnitude;
	return TotalG;
}
//calculates G  for resistors  between two nodes
double CalculateMutualG(Node* node1,Node* node2) {
	if (node1 == node2)
		return CalculateG(node1);
	double TotalG = 0;
	for (std::vector<Component>::iterator it = node1->Resistors.begin(); it != node1->Resistors.end(); ++it)
		if( (it->Terminal1==node1->Number&&it->Terminal2 == node2->Number) || (it->Terminal2 == node1->Number&&it->Terminal1 == node2->Number) )
			TotalG += 1 / it->Magnitude;
	return TotalG;
}
double CalculateCurrent(Node* node) {
	double TotalI = 0;
	for (std::vector<Component>::iterator it = node->CurrentSource.begin(); it != node->CurrentSource.end(); ++it)
		TotalI += 1 / it->Magnitude;
	return TotalI;
}
//TODO: Function to distinguish the ref node 
bool isRef(Node* node) {
	return false;
}
void BindVoltageValues(vector<Node> nodes,MatrixXd matrixV) {
	int matIndex = 0;
	for (int i = 0; i < nodes.size(); i++)
		if (CheckEssential(&nodes[i])) {
			nodes[i].voltage = matrixV(matIndex, 0);
			nodes[i].voltageSet = true;
			matIndex++;
		}
		else
			nodes[i].voltageSet = false;
}
int CountEssentialNodes(vector<Node> nodes) {
	int EssentialsCount = 0;
	for (std::vector<Node>::iterator it = nodes.begin(); it != nodes.end(); ++it) {
		if (CheckEssential(&(*it)))
			EssentialsCount++;
	}
	return EssentialsCount;
}
void ConvertCircuit(vector<Node> nodes) {
	for (int i = 0; i < nodes.size(); i++) {
		if (!CheckEssential(&nodes[i])) {
			bool validToConvert = !(nodes[i].Resistors.empty() && nodes[i].VoltageSource.empty());
			if (validToConvert) {
				ConvertVStoCS(&nodes[i],nodes);
			}
		}
	}
}
void ConvertVStoCS(Node* node,vector<Node> nodes) {
	node->deprecated = true;
	Component CurrentSource;
	CurrentSource.Label = "JX";//to denote it's not original in the circuit
	CurrentSource.Magnitude = node->VoltageSource[0].Magnitude / node->Resistors[0].Magnitude;
	if (node->Resistors[0].Terminal1 == node->Number) {
		CurrentSource.Terminal2 = node->Resistors[0].Terminal2;
		if (node->VoltageSource[0].Terminal1 == node->Number) {
			node->Resistors[0].Terminal1 = node->VoltageSource[0].Terminal2;
		}
		else {
			node->Resistors[0].Terminal1 = node->VoltageSource[0].Terminal1;
		}
		CurrentSource.Terminal1 = node->Resistors[0].Terminal1;
		nodes[node->Resistors[0].Terminal1].Resistors.push_back(node->Resistors[0]);
	}


	else {
		CurrentSource.Terminal1 = node->Resistors[0].Terminal1;
		if (node->VoltageSource[0].Terminal1 == node->Number) {
			node->Resistors[0].Terminal2 = node->VoltageSource[0].Terminal2;
		}
		else {
			node->Resistors[0].Terminal2 = node->VoltageSource[0].Terminal1;
		}
		CurrentSource.Terminal2 = node->Resistors[0].Terminal2;
		nodes[node->Resistors[0].Terminal2].Resistors.push_back(node->Resistors[0]);
	}
	int node1 = CurrentSource.Terminal1;
	int node2 = CurrentSource.Terminal2;
	nodes[node1].CurrentSource.push_back(CurrentSource);
	nodes[node2].CurrentSource.push_back(CurrentSource);
}

//returns the vector of nodes with the voltages values
//attached to each node
void PerformNodeAnalysis(vector<Node> nodes) {
	//Should there be a call to convert circuit first 
	int EssetialCount = CountEssentialNodes(nodes);
	MatrixXd matG(EssetialCount, EssetialCount);
	MatrixXd matI(EssetialCount, 1);
	MatrixXd matV(EssetialCount, 1);
	matG= BuildMatrixG(nodes);
	matI = BuildMatrixI(nodes);
	matV = GetMatrixV(matG, matI);
	BindVoltageValues(nodes,matV);
}