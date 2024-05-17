#include "ezdungeon.h"
#include <vector>
#include <iostream>
#include <stack>
#include <random>

class Node{
public:
    int corner0X;
    int corner0Y;

    int corner1X;
    int corner1Y;

    int roomCorner0X;
    int roomCorner0Y;

    int roomCorner1X;
    int roomCorner1Y;

    Node * child0;
    Node * child1;
    Node * parent;

    inline static int nNodes = 0;
    int nodeId;

    Node(int c0x,int c0y,int c1x,int c1y,Node * p){
        corner0X = c0x;
        corner0Y = c0y;

        corner1X = c1x;
        corner1Y = c1y;
        parent = p;
        nodeId = nNodes;
        nNodes++;
        std::cout << "node created " << nodeId;
        if(p!=nullptr) std::cout << " parent " << p->nodeId;
        std::cout << std::endl;
    }

    class iterator{
    private:
        Node * node;
    public:
        iterator(const iterator & it);
        bool operator!=(const iterator & it) const;
        bool operator==(const iterator & it) const;
        iterator & operator=(const iterator & it);
        Node & operator*();
        iterator & operator++();
    };
};

void develop(Node * node,int minWidthSpace,int minHeightSpace,int minRoomWidth,int minRoomHeight,int maxRoomWidth,int maxRoomHeight,std::vector<std::vector<int>> * dungeon,int * roomId){
    int width = node->corner1X - node->corner0X;
    int height = node->corner1Y - node->corner0Y;

    bool verticalCut = width>=minWidthSpace*2;
    bool horizontalCut = height>=minHeightSpace*2;

    int range;
    int cut;
    if(verticalCut&&!horizontalCut){
        range = width - (minWidthSpace*2);
        cut = range > 0 ? rand() % range : 0;
        node->child0 = new Node(node->corner0X,node->corner0Y,node->corner0X+minWidthSpace+cut,node->corner1Y,node);
        node->child1 = new Node(node->corner0X+minWidthSpace+cut,node->corner0Y,node->corner1X,node->corner1Y,node);
    }else if(!verticalCut&&horizontalCut){
        range = height - (minHeightSpace*2);
        cut = range > 0 ? rand() % range : 0;
        node->child0 = new Node(node->corner0X,node->corner0Y,node->corner1X,node->corner0Y+minHeightSpace+cut,node);
        node->child1 = new Node(node->corner0X,node->corner0Y+minHeightSpace+cut,node->corner1X,node->corner1Y,node);
    }else if(verticalCut&&horizontalCut){
        if(rand()%2==0){
            range = height - (minHeightSpace*2);
            cut = range > 0 ? rand() % range : 0;
            node->child0 = new Node(node->corner0X,node->corner0Y,node->corner1X,node->corner0Y+minHeightSpace+cut,node);
            node->child1 = new Node(node->corner0X,node->corner0Y+minHeightSpace+cut,node->corner1X,node->corner1Y,node);
        }else{
            range = width - (minWidthSpace*2);
            cut = range > 0 ? rand() % range : 0;
            node->child0 = new Node(node->corner0X,node->corner0Y,node->corner0X+minWidthSpace+cut,node->corner1Y,node);
            node->child1 = new Node(node->corner0X+minWidthSpace+cut,node->corner0Y,node->corner1X,node->corner1Y,node);
        }
    }else{
        int roomWidth = rand() % (maxRoomWidth-minRoomWidth) + minRoomWidth;
        int roomHeight = rand() % (maxRoomHeight-minRoomHeight) + minRoomHeight;
        roomWidth = roomWidth > width ? width : roomWidth;
        roomHeight = roomHeight > height ? height : roomHeight;

        node->roomCorner0X = (roomWidth>=width) ? node->corner0X : node->corner0X + rand() % (width-roomWidth);
        node->roomCorner0Y = (roomHeight>=height) ? node->corner0Y : node->corner0Y + rand() % (height-roomHeight);

        node->roomCorner1X = node->roomCorner0X + roomWidth;
        node->roomCorner1Y = node->roomCorner0Y + roomHeight;

        for(int i = node->roomCorner0X; i < node->roomCorner1X;i++){
            for(int j = node->roomCorner0Y; j < node->roomCorner1Y;j++){
                (*dungeon)[i][j] = *roomId;
                
            }
        }
        (*roomId)++;

        return;
    }
    develop(node->child0,minWidthSpace,minHeightSpace, minRoomWidth, minRoomHeight, maxRoomWidth, maxRoomHeight,dungeon,roomId);
    develop(node->child1,minWidthSpace,minHeightSpace, minRoomWidth, minRoomHeight, maxRoomWidth, maxRoomHeight,dungeon,roomId);
}

void joinHall(int startX,int startY,int endX,int endY, DungeonMatrix * dungeon){
    int posX = startX;
    int posY = startY;
    short dir = 0;
    // 0 up
    // 1 right
    // 2 down
    // 3 left
    while(posX!=endX||posY!=endY){
        if(posX<endX) dir = 3;
        else if(posX>endX) dir = 1;
        else if(posY<endY) dir = 2;
        else dir = 0;

        dungeon->AddPos(posX,0,posY,DUN_PXZ_WALL);
       
        if(posX>0&&(!dungeon->CheckPos(posX-1,0,posY,DUN_PXZ_WALL))) dungeon->AddPos(posX,0,posY,DUN_PYZ_WALL);
        if(posY>0&&(!dungeon->CheckPos(posX,0,posY-1,DUN_PXZ_WALL))) dungeon->AddPos(posX,0,posY,DUN_PXY_WALL);
        if((posX<(dungeon->GetSize().x-1))&&(!dungeon->CheckPos(posX+1,0,posY,DUN_PXZ_WALL))) dungeon->AddPos(posX+1,0,posY,DUN_PYZ_WALL);
        if((posY<(dungeon->GetSize().z-1))&&(!dungeon->CheckPos(posX,0,posY+1,DUN_PXZ_WALL))) dungeon->AddPos(posX,0,posY+1,DUN_PXY_WALL);

        if(dir==0) dungeon->RemovePos(posX,0,posY,DUN_PXY_WALL);
        if(dir==1) dungeon->RemovePos(posX,0,posY,DUN_PYZ_WALL);
        if(dir==2&&posY<(dungeon->GetSize().z-1)) dungeon->RemovePos(posX,0,posY+1,DUN_PXY_WALL);
        if(dir==3&&posX<(dungeon->GetSize().x-1)) dungeon->RemovePos(posX+1,0,posY,DUN_PYZ_WALL);

        switch(dir){
        case 0:
            posY--;
            break;
        case 1:
            posX--;
            break;
        case 2:
            posY++;
            break;
        case 3:
            posX++;
            break;
        }
    }
}

void postOrderJoiner(Node * root,DungeonMatrix * dungeon){
    Node * previousNode = nullptr;
    Node * node = root;
    while(node->child0!=nullptr){
        node = node->child0;
    }
    std::cout << "starting at " << node->nodeId << std::endl;
 
    std::stack<Node*> toExplore;
    toExplore.push(root);

    while(!toExplore.empty()){
        Node * node = toExplore.top();
        toExplore.pop();
        if(node->child0!=nullptr){
            toExplore.push(node->child0);
            toExplore.push(node->child1);
        }else{
            if(previousNode!=nullptr){
                int startXrange = previousNode->roomCorner0X-previousNode->roomCorner1X;
                int startYrange = previousNode->roomCorner0Y-previousNode->roomCorner1Y;
                int endXrange = node->roomCorner0X-node->roomCorner1X;
                int endYrange = node->roomCorner0Y-node->roomCorner1Y;
                int startX = previousNode->roomCorner0X + rand() % startXrange;
                int startY = previousNode->roomCorner0Y + rand() % startYrange;
                int endX = node->roomCorner0X + rand() % endXrange;
                int endY = node->roomCorner0Y + rand() % endYrange;

                joinHall(startX,startY,endX,endY,dungeon);
            }
            previousNode = node;

        }
    }
}

DungeonMatrix * generateDungeon(int width,int height,int minWidthSpace,int minHeightSpace,int minRoomWidth,int minRoomHeight,int maxRoomWidth,int maxRoomHeight,int seed){
    srand(seed);
    std::vector<std::vector<int>> dungeon;

    for(int i = 0; i < width;i++){
        dungeon.push_back(std::vector<int>(height,-1));
    }

    int * roomId = new int (1);
    Node * emptyNode = new Node(0,0,width,height,nullptr);
    Node * root = new Node(0,0,width,height,emptyNode);
    develop(root,minWidthSpace,minHeightSpace, minRoomWidth, minRoomHeight, maxRoomWidth, maxRoomHeight, &dungeon,roomId);

    DungeonMatrix * matrix = new DungeonMatrix(width,1,height);

    for(unsigned int i = 0; i < width;i++){
        for(unsigned int j = 0; j < height;j++){
            if( (dungeon)[i][j]>=0) matrix->SetPos({i,0,j},DUN_PXZ_WALL);

            if(i>0&&dungeon[i-1][j]!=(dungeon)[i][j]) matrix->SetPos({i,0,j},matrix->GetPos({i,0,j}) | DUN_PYZ_WALL);
            if(j>0&&dungeon[i][j-1]!=(dungeon)[i][j]) matrix->SetPos({i,0,j},matrix->GetPos({i,0,j}) | DUN_PXY_WALL);
        }
        std::cout << std::endl;
    }

    postOrderJoiner(root, matrix);

    return matrix;
}

int main(int args,char ** argv){
    ArgHandler argHandler;
    std::random_device rd;
    int seed = rd();
    argHandler.AddArg("width",20,"dungeon's max width");
    argHandler.AddArg("height",20,"dungeon's max height");
    argHandler.AddArg("minWidthSpace",8,"minimun reserved width space for a room");
    argHandler.AddArg("minHeightSpace",8,"minimun reserved height space for a room");

    argHandler.AddArg("minRoomHeight",8,"minimun room height");
    argHandler.AddArg("minRoomWidth",8,"minimun room width");

    argHandler.AddArg("maxRoomHeight",15,"maximun room height");
    argHandler.AddArg("maxRoomWidth",15,"maximun room width");

    argHandler.AddStringArg("o","a.dun","output file name");

    argHandler.AddArg("seed",seed,"dungeon's generation seed");

    if(!argHandler.ReadArgs(args,argv)){
        argHandler.PrintHelp("width");
        argHandler.PrintHelp("height");
        argHandler.PrintHelp("minWidthSpace");
        argHandler.PrintHelp("minHeightSpace");
        argHandler.PrintHelp("maxRoomHeight");
        argHandler.PrintHelp("maxRoomWidth");
        argHandler.PrintHelp("minRoomHeight");
        argHandler.PrintHelp("minRoomWidth");
        argHandler.PrintHelp("seed");
        argHandler.PrintHelp("o");
        return 0;
    }

    std::cout << "WIDTH: " << argHandler.GetIntArg("width") << std::endl;
    std::cout << "DEPTH: " << argHandler.GetIntArg("height") << std::endl;
    std::cout << "----------------" << std::endl;
    std::cout << "MIN WIDTH SPACE: " << argHandler.GetIntArg("minWidthSpace") << std::endl;
    std::cout << "MIN HEIGHT SPACE: " << argHandler.GetIntArg("minHeightSpace") << std::endl;
    std::cout << "MIN ROOM WIDTH: " << argHandler.GetIntArg("minRoomWidth") << std::endl;
    std::cout << "MIN ROOM HEIGHT: " << argHandler.GetIntArg("minRoomHeight") << std::endl;
    std::cout << "MAX ROOM WIDTH: " << argHandler.GetIntArg("maxRoomWidth") << std::endl;
    std::cout << "MAX ROOM HEIGHT: " << argHandler.GetIntArg("maxRoomHeight") << std::endl;
    std::cout << "----------------" << std::endl;
    std::cout << "SEED:" << argHandler.GetIntArg("seed") << std::endl;

    DungeonMatrix * dungeon = generateDungeon(
        argHandler.GetIntArg("width"),
        argHandler.GetIntArg("height"),
        argHandler.GetIntArg("minWidthSpace"),
        argHandler.GetIntArg("minHeightSpace"),
        argHandler.GetIntArg("minRoomWidth"),
        argHandler.GetIntArg("minRoomHeight"),
        argHandler.GetIntArg("maxRoomWidth"),
        argHandler.GetIntArg("maxRoomHeight"),
        argHandler.GetIntArg("seed"));


    std::string filename = argHandler.GetStringArg("o");

    if(dungeon->Dun2File(filename))
    {
        std::cout << "archivo generado" << std::endl;
    }
    else
    {
        std::cout << "el archivo no se ha podido generar" << std::endl;
    }

    return 0;
}