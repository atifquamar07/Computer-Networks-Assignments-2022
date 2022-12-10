#include "node.h"
#include <iostream>
#include <queue>
#include <unordered_map>

using namespace std;

void printRT(vector<RoutingNode *> nd)
{
    /*Print routing table entries*/
    for (int i = 0; i < nd.size(); i++)
    {
        nd[i]->printTable();
    }
}


void routingAlgo(vector<RoutingNode *> nd){

    int n = nd.size();

    for (int i = 0; i < n; ++i)
    {
        for (RoutingNode *node : nd)
        {
            node->sendMsg(nd, node);
        }
    }

    printf("Printing the routing tables after the convergence \n");
    printRT(nd);

}

void RoutingNode::recvMsg(priority_queue<pair<int, RoutingNode*>, vector<pair<int, RoutingNode*>>, greater<pair<int, RoutingNode*>>> pq,
unordered_map<RoutingNode *, int> distance, unordered_map<RoutingNode *, RoutingNode *> parent, vector<RoutingNode *> nd, 
RoutingNode *node)
{
    // your code here

    int n = nd.size();
    while(!pq.empty()){

            int dist = pq.top().first;
            RoutingNode *prev = pq.top().second;
            pq.pop();

            vector<pair<NetInterface, Node *>> adjList = prev->interfaces;

            for(auto it : adjList){
                Node *temp = it.second;
                string name = temp->getName();

                RoutingNode *next;
                for (auto x : nd){
                    if(x->getName() == name){
                        next = x;
                        break;
                    }
                }

                if(distance[next] > dist + 1){
                    distance[next] = dist + 1;
                    pq.push({distance[next], next});
                    parent[next] = prev;
                }
            }
        }


        for (int j = 0; j < n; j++){
            
            RoutingNode *curr = nd[j];

            vector<RoutingNode *> path;

            while (parent[curr] != curr){
                path.push_back(curr);
                curr = parent[curr];
            }

            path.push_back(node);
            reverse(path.begin(), path.end());

            RoutingNode *nextNode = path[0];

            if(path.size() > 1){
                nextNode = path[1];
            }
            string nextNodeName = nextNode->getName();

            string nexthop;
            string myIPinterface;

            vector<pair<NetInterface, Node *>> adjList = node->interfaces;

            for(auto x : adjList){
                if(x.second->getName() == nextNodeName){
                    nexthop = x.first.getConnectedIp();
                    myIPinterface = x.first.getip();
                    break;
                }
            }

            // Add Table Entry

            vector<pair<NetInterface, Node *>> destIPs = nd[j]->interfaces;

            for(auto k : destIPs){
                string destip = k.first.getip();
                
                if(nexthop == ""){
                    continue;
                }
                else {
                    addTblEntry(destip, nexthop, myIPinterface, distance[nd[j]]); 
                }
            }
        }

}
