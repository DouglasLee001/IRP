#include "_Graph.h"
#include<iostream>
#include<sstream>
#include<math.h>
#include<string>
#include <fstream>
#include<vector>
#include<unistd.h>
#include<stdio.h>
using namespace szx;
void writeFile(Graph::CoordList<> &coordList,int round){//round代表当前在第几周期
        std::ostringstream vname1;
        std::ostringstream vname;
        std::ostringstream st;
        vname<<"./pr"<<round<<".tsp";///Users/douglaslee/Documents/codes/IRP2.0/pr
        vname1<<"./pr"<<round<<".par";///Users/douglaslee/Documents/codes/IRP2.0/pr
        std::fstream f(vname.str(), std::ios::out);
        std::fstream f1(vname1.str(),std::ios::out);
        if(f.bad())
        {
            std::cout << "打开文件出错" << std::endl;
        }
        int point=1;
        int Nretailers=coordList.size();
        for(int i=0;i<Nretailers;i++){
            st<<point++<<' '<<coordList[i].x<<' '<<coordList[i].y<<std::endl;
        }
        f<<"NAME : pr"<<std::endl<<"TYPE : TSP\nDIMENSION : "<<(point-1)<<"\nEDGE_WEIGHT_TYPE : EUC_2D\nNODE_COORD_SECTION\n"<<st.str()<<"EOF";
        f.close();
        f1<<"PROBLEM_FILE = "<<vname.str()<<"\nMOVE_TYPE = 5\nPATCHING_C = 3\nPATCHING_A = 2\nPOPULATION_SIZE=2\nRUNS = 10\nPOPULATION_SIZE=2\nOUTPUT_TOUR_FILE = ./ans"<<round<<".txt";///Users/douglaslee/Documents/codes/IRP2.0
        f1.close();
    
}
void lkh1(int round){
    int childpid;
    if (fork() == 0){
        //child process
        std::ostringstream str;
        str<<"./pr"<<round<<".par";//str<<"/Users/douglaslee/Documents/codes/IRP2.0/pr"<<i<<".par";
        char *p=(char *)str.str().data();
        char * execv_str[] = {"./lkh",p,NULL};
        if (execv("./LKH",execv_str) <0 ){///Users/douglaslee/Desktop/IRP2/LKH/LKH-3.0.5/LKH!!!!!此处要定位到LKH3所在文件位置，或者直接将LKH可执行文件移到当前目录下
            perror("error on exec");
            exit(0);
        }
    }else{
        //parent process
        wait(&childpid);
        printf("execv done\n\n");
    }
}



// interface.
int solve(Graph::Tour &sln, Graph::CoordList<> &coordList,int round,const Graph::Tour &hintSln = Graph::Tour()) {
    if (!hintSln.empty()) {
        // EXTEND[szx][0]: utilize initial solution.
    }
    writeFile(coordList,round);
    lkh1(round);
    std::ostringstream vname;
    vname<<"./ans"<<round<<".txt";
    std::freopen(vname.str().data(),"r",stdin);
    std::string s;
    int cost=0;
    std::vector<std::string> v;
    int flag=0;
    while(std::cin>>s){
        if(s=="=")
        {
            std::cin>>s;
            cost=atoi(s.c_str());
            v.push_back(s);
        }
        if(s=="TOUR_SECTION"){
            std::cin>>s;
            while(1){
                sln.push_back(atoi(s.c_str()));
                std::cin>>s;
                if(s=="-1")
                {
                    flag=1;break;
                }
            }
        }
        if(flag==1)
            break;
    }
    fclose(stdin);
    // TODO[szx][0]: retrieve solution path.
    return cost;
}

bool solve(Graph::Tour &sln, const Graph::CoordList<Graph::Coord3D<>> &coordList, const Graph::Tour &hintSln = Graph::Tour()) {
    return false;
}

bool solve(Graph::Tour &sln, const Graph::AdjMat<> &adjMat, const Graph::Tour &hintSln = Graph::Tour()) {
    return false;
}

bool solve(Graph::Tour &sln, const Graph::AdjList<Graph::WeightedAdjNode<>> &adjList, const Graph::Tour &hintSln = Graph::Tour()) {
    return false;
}

bool solve(Graph::Tour &sln, const Graph::EdgeList<Graph::WeightedEdge<>> &edgeList, Graph::ID nodeNum, const Graph::Tour &hintSln = Graph::Tour()) {
    return false;
}

// testing and sample code.
void testCoordList2D() {
    Graph::CoordList<> cl({
        { 0, 0 },
        { 100, 200 },
        { 200, 100 }
    });
    Graph::Tour sln;
    std::cout<<std::endl<<solve(sln, cl,1)<<std::endl;
    for(int i=0;i<sln.size();i++){
        std::cout<<sln[i]<<std::endl;
    }
   // if (solve(sln, cl,1)) {
        // EXTEND[szx][0]: record solution path.
    //}
}

void testCoordList3D() {
    Graph::CoordList<Graph::Coord3D<>> cl({
        Graph::Coord3D<>(0, 0, 0),
        Graph::Coord3D<>(100, 200, 300),
        Graph::Coord3D<>(300, 200, 100)
    });
    Graph::Tour sln;
    if (solve(sln, cl)) {

    }
}

void testAdjMat() {
    Graph::AdjMat<> am(3, 3);
    am.at(0, 0) = 0;
    am.at(0, 1) = 1;
    am.at(0, 2) = 2;
    am.at(1, 0) = 3;
    am.at(1, 1) = 0;
    am.at(1, 2) = 4;
    am.at(2, 0) = 5;
    am.at(2, 1) = 6;
    am.at(2, 2) = 0;
    Graph::Tour sln;
    if (solve(sln, am)) {

    }
}

void testWeightedAdjList() {
    Graph::AdjList<Graph::WeightedAdjNode<>> al({
        { Graph::WeightedAdjNode<>(1, 100), Graph::WeightedAdjNode<>(2, 200) },
        { Graph::WeightedAdjNode<>(0, 200), Graph::WeightedAdjNode<>(2, 100) },
        { Graph::WeightedAdjNode<>(0, 300) }
    });
    Graph::Tour sln;
    if (solve(sln, al)) {

    }
}

void testWeightedEdgeList() {
    Graph::EdgeList<Graph::WeightedEdge<>> el({
        Graph::WeightedEdge<>(0, 1, 100),
        Graph::WeightedEdge<>(1, 2, 200),
        Graph::WeightedEdge<>(2, 0, 300)
    });
    Graph::Tour sln;
    if (solve(sln, el, 3)) {

    }
}


int main(int argc, char *argv[]) {
    testCoordList2D();
    //testCoordList3D();
    //testAdjMat();
    //testWeightedAdjList();
    //testWeightedEdgeList();

    return 0;
}
