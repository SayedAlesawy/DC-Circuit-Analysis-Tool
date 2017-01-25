#include "Responses.h"

bool SearchElement(string element, vector<Node>nodes, int &x, int &y)
{
	if (element[0] == 'V') {
		for (int i = 0; i < nodes.size(); i++) {
			for (int k = 0; k < nodes[i].VoltageSource.size(); k++) {
				if (nodes[i].VoltageSource[k]->Label == element) {
					x = i, y = k;   //x= node number, y=index of the element in the vector
					return true;
				}
			}
		}
	}
	else if (element[0] == 'R') {
		for (int i = 0; i < nodes.size(); i++) {
			for (int k = 0; k < nodes[i].Resistors.size(); k++) {
				if (nodes[i].Resistors[k]->Label == element) {
					x = i, y = k;   //x= node number, y=index of the element in the vector
					return true;
				}
			}
		}
	}
	else if (element[0] == 'J') {
		for (int i = 0; i < nodes.size(); i++) {
			for (int k = 0; k < nodes[i].CurrentSource.size(); k++) {
				if (nodes[i].CurrentSource[k]->Label == element) {
					x = i, y = k;   //x= node number, y=index of the element in the vector
					return true;
				}
			}
		}
	}
	else
		return false;
}


double CalculateVD(int node1, int node2, vector<Node>nodes)
{
	double NodeVoltage[2];

	NodeVoltage[0] = nodes[node1 - 1].voltage;
	NodeVoltage[1] = nodes[node2 - 1].voltage;

	sort(NodeVoltage, NodeVoltage + 2);

	double VoltageDifference = NodeVoltage[1] - NodeVoltage[0];

	return VoltageDifference;
}


double ResistorCurrent(string resistor, vector<Node>nodes)
{
	int x, y;
	if (!SearchElement(resistor, nodes, x, y))
		cout << "Error, element wasn't found" << endl;

	double VD = CalculateVD(nodes[x].Resistors[y]->Terminal1+1,
		                    nodes[x].Resistors[y]->Terminal2+1, nodes);

	double Current = (double)VD / (double)nodes[x].Resistors[y]->Magnitude;

	return Current;
}


double CalculatePower(string element, vector<Node>nodes)
{
	int x, y; double Power;

	if (!SearchElement(element, nodes, x, y)) {
		cout << "Error, element wasn't found" << endl;
		return 0;
	}

	if (element[0] == 'V') {
		double Current = VoltageSourceCurrent(element, nodes,true);
		double Voltage = nodes[x].VoltageSource[y]->Magnitude;

		Power = Voltage*Current;
		return Power;
	}
	else if (element[0] == 'R') {
		int TR1 = nodes[x].Resistors[y]->Terminal1;
		int TR2 = nodes[x].Resistors[y]->Terminal2;

		double R = nodes[x].Resistors[y]->Magnitude;
		double VD = CalculateVD(TR1 + 1, TR2 + 1, nodes);

		Power = pow(VD, 2) / R;
		return Power;
	}
	else if (element[0] == 'J') {
		int TJ1 = nodes[x].CurrentSource[y]->Terminal1;
		int TJ2 = nodes[x].CurrentSource[y]->Terminal2;

		double I = nodes[x].CurrentSource[y]->Magnitude;
		double VD = CalculateVD(TJ1 + 1, TJ2 + 1, nodes);

		Power = VD*I;
		return Power;
	}
}


double VoltageSourceCurrent(string element, vector<Node>nodes, bool search)
{
	int x, y, TV1, TV2, minSize, TargetNode;

	if(!search)
		if (!SearchElement(element, nodes, x, y))
			cout << "Error, element wasn't found" << endl;

	TV1 = nodes[x].VoltageSource[y]->Terminal1;
	TV2 = nodes[x].VoltageSource[y]->Terminal2; 

	if (nodes[TV1].Resistors.size())
		TargetNode = TV1;
	else if (nodes[TV2].Resistors.size())
		TargetNode = TV2;
	else
		cout << "Error in connections" << endl;

	int TR1 = nodes[TargetNode].Resistors[0]->Terminal1;
	int TR2 = nodes[TargetNode].Resistors[0]->Terminal2;

	double VD = CalculateVD(TR1 + 1, TR2 + 1, nodes);
	double Current = (double)VD/(double)nodes[TargetNode].Resistors[0]->Magnitude;

	return Current;
}