#include "ezdungeon.h"
#include <vector>
#include <iostream>
#include <stack>

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
    // std::cout << "node " << node->corner0X << "," << node->corner0Y << " " << node->corner1X << "," << node->corner1Y << std::endl;
    if(verticalCut&&!horizontalCut){
        // std::cout << "Vcut" << std::endl;
        range = width - (minWidthSpace*2);
        cut = range > 0 ? rand() % range : 0;
        node->child0 = new Node(node->corner0X,node->corner0Y,node->corner0X+minWidthSpace+cut,node->corner1Y,node);
        node->child1 = new Node(node->corner0X+minWidthSpace+cut,node->corner0Y,node->corner1X,node->corner1Y,node);
    }else if(!verticalCut&&horizontalCut){
        // std::cout << "Hcut" << std::endl;
        range = height - (minHeightSpace*2);
        cut = range > 0 ? rand() % range : 0;
        node->child0 = new Node(node->corner0X,node->corner0Y,node->corner1X,node->corner0Y+minHeightSpace+cut,node);
        node->child1 = new Node(node->corner0X,node->corner0Y+minHeightSpace+cut,node->corner1X,node->corner1Y,node);
    }else if(verticalCut&&horizontalCut){
        if(rand()%2==0){
            // std::cout << "Hcut rand" << std::endl;
            range = height - (minHeightSpace*2);
            cut = range > 0 ? rand() % range : 0;
            node->child0 = new Node(node->corner0X,node->corner0Y,node->corner1X,node->corner0Y+minHeightSpace+cut,node);
            node->child1 = new Node(node->corner0X,node->corner0Y+minHeightSpace+cut,node->corner1X,node->corner1Y,node);
        }else{
            // std::cout << "Vcut rand" << std::endl;
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

        // node->roomCorner0X = node->corner0X + rand() % (width-roomWidth);
        // node->roomCorner0Y = node->corner0Y + rand() % (height-roomHeight);

        node->roomCorner1X = node->roomCorner0X + roomWidth;
        node->roomCorner1Y = node->roomCorner0Y + roomHeight;
        // std::cout << "corner0X " << node->corner0X << " corner0Y " << node->corner0Y << " corner1X " << node->corner1X << " corner1Y " << node->corner1Y << std::endl;
        // std::cout << "roomCorner0X " << node->roomCorner0X << " roomCorner0Y " << node->roomCorner0Y << " roomCorner1X " << node->roomCorner1X << " roomCorner1Y " << node->roomCorner1Y << std::endl;

        for(int i = node->roomCorner0X; i < node->roomCorner1X;i++){
            for(int j = node->roomCorner0Y; j < node->roomCorner1Y;j++){
                (*dungeon)[i][j] = *roomId;
                
            }
        }
        (*roomId)++;
        // for(int i = node->corner0X; i < node->corner1X;i++){
        //     for(int j = node->corner0Y; j < node->corner1Y;j++){
        //         (*dungeon)[i][j] = 1;
        //     }
        // }

        return;
    }
    // std::cout << "child0 " << node->child0->corner0X << "," << node->child0->corner0Y << " " << node->child0->corner1X << "," << node->child0->corner1Y << std::endl;
    // std::cout << "child1 " << node->child1->corner0X << "," << node->child1->corner0Y << " " << node->child1->corner1X << "," << node->child1->corner1Y << std::endl;
    develop(node->child0,minWidthSpace,minHeightSpace, minRoomWidth, minRoomHeight, maxRoomWidth, maxRoomHeight,dungeon,roomId);
    develop(node->child1,minWidthSpace,minHeightSpace, minRoomWidth, minRoomHeight, maxRoomWidth, maxRoomHeight,dungeon,roomId);
}

void joinHall(int startX,int startY,int endX,int endY, dungeonMatrix * dungeon){
    int posX = startX;
    int posY = startY;
    int dir = 0;
    // 0 up
    // 1 right
    // 2 down
    // 3 left
    while(posX!=endX||posY!=endY){
        if(posX<endX) dir = 3;
        else if(posX>endX) dir = 1;
        else if(posY<endY) dir = 2;
        else dir = 0;
        // if(posX!=endX){
        //     posX = posX < endX ? posX + 1 : posX - 1;
        //     dungeon->data[posX][0][posY] |= DUN_PXZ_WALL; //Add floor tiles
        //     dungeon->data[posX][0][posY] &= ~DUN_PYZ_WALL; //Remove perpendicular walls
        //     if(posY<=0||(!(dungeon->data[posX][0][posY-1]&DUN_PXZ_WALL))) dungeon->data[posX][0][posY] |= DUN_PXY_WALL; //Add top walls
        //     if((posY<(dungeon->size_z-1))&&(!(dungeon->data[posX][0][posY+1]&DUN_PXZ_WALL))) dungeon->data[posX][0][posY+1] |= DUN_PXY_WALL; // Add bottom walls
        //     if(posX==endX&&(posX<(dungeon->size_x-1))&&(!(dungeon->data[posX+1][0][posY]&DUN_PXZ_WALL))) dungeon->data[posX+1][0][posY] |= DUN_PYZ_WALL; // Fix the corner wall
        // }
        // else{
        //     posY = posY < endY ? posY + 1 : posY - 1;
        //     dungeon->data[posX][0][posY] |= DUN_PXZ_WALL; //Add floor tiles
        //     dungeon->data[posX][0][posY] &= ~DUN_PXY_WALL; //Remove perpendicular walls
        //     if(posX<=0||(!(dungeon->data[posX-1][0][posY]&DUN_PXZ_WALL))) dungeon->data[posX][0][posY] |= DUN_PYZ_WALL; //Add top walls
        //     if((posX<(dungeon->size_x-1))&&(!(dungeon->data[posX+1][0][posY]&DUN_PXZ_WALL))) dungeon->data[posX+1][0][posY] |= DUN_PYZ_WALL; // Add bottom walls
        // }
        dungeon->data[posX][0][posY] |= DUN_PXZ_WALL;

        

        if(posX>0&&(!(dungeon->data[posX-1][0][posY]&DUN_PXZ_WALL))) dungeon->data[posX][0][posY] |= DUN_PYZ_WALL;
        if(posY>0&&(!(dungeon->data[posX][0][posY-1]&DUN_PXZ_WALL))) dungeon->data[posX][0][posY] |= DUN_PXY_WALL;
        if((posX<(dungeon->size_x-1))&&(!(dungeon->data[posX+1][0][posY]&DUN_PXZ_WALL))) dungeon->data[posX+1][0][posY] |= DUN_PYZ_WALL;
        if((posY<(dungeon->size_z-1))&&(!(dungeon->data[posX][0][posY+1]&DUN_PXZ_WALL))) dungeon->data[posX][0][posY+1] |= DUN_PXY_WALL;

        if(dir==0) dungeon->data[posX][0][posY] &= ~DUN_PXY_WALL;
        if(dir==1) dungeon->data[posX][0][posY] &= ~DUN_PYZ_WALL;
        if(dir==2&&posY<(dungeon->size_z-1)) dungeon->data[posX][0][posY+1] &= ~DUN_PXY_WALL;
        if(dir==3&&posX<(dungeon->size_x-1)) dungeon->data[posX+1][0][posY] &= ~DUN_PYZ_WALL;

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

void postOrderJoiner(Node * root,dungeonMatrix * dungeon){
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

dungeonMatrix * generateDungeon(int width,int height,int minWidthSpace,int minHeightSpace,int minRoomWidth,int minRoomHeight,int maxRoomWidth,int maxRoomHeight,int seed){
    std::vector<std::vector<int>> dungeon;

    for(int i = 0; i < width;i++){
        dungeon.push_back(std::vector<int>(height,-1));
    }

    int * roomId = new int (1);
    Node * emptyNode = new Node(0,0,width,height,nullptr);
    Node * root = new Node(0,0,width,height,emptyNode);
    develop(root,minWidthSpace,minHeightSpace, minRoomWidth, minRoomHeight, maxRoomWidth, maxRoomHeight, &dungeon,roomId);

    dungeonMatrix * matrix;

    allocDungeonMatrix(&matrix,width,1,height);

    for(int i = 0; i < width;i++){
        for(int j = 0; j < height;j++){
            // if((dungeon)[i][j]>0)
            //     std::cout << (dungeon)[i][j]%9+1;
            // else
            //     std::cout << " ";
            if( (dungeon)[i][j]>=0) matrix->data[i][0][j] = DUN_PXZ_WALL;

            if(i>0&&dungeon[i-1][j]!=(dungeon)[i][j]) matrix->data[i][0][j] |= DUN_PYZ_WALL;
            if(j>0&&dungeon[i][j-1]!=(dungeon)[i][j]) matrix->data[i][0][j] |= DUN_PXY_WALL;
        }
        std::cout << std::endl;
    }

    // joinHall(10,10,20,20,matrix);
    postOrderJoiner(root, matrix);

    return matrix;
}

int main(int args,char ** argv){
    ArgHandler argHadler;
    srand(time(NULL));
    int seed = rand();
    argHadler.AddArg("width",20,"dungeon's max width");
    argHadler.AddArg("height",20,"dungeon's max height");
    argHadler.AddArg("minWidthSpace",8,"minimun reserved width space for a room");
    argHadler.AddArg("minHeightSpace",8,"minimun reserved height space for a room");

    argHadler.AddArg("minRoomHeight",8,"minimun room height");
    argHadler.AddArg("minRoomWidth",8,"minimun room width");

    argHadler.AddArg("maxRoomHeight",15,"maximun room height");
    argHadler.AddArg("maxRoomWidth",15,"maximun room width");

    argHadler.AddStringArg("o","a.dun","output file name");

    argHadler.AddArg("seed",seed,"dungeon's generation seed");

    if(!argHadler.ReadArgs(args,argv)){
        argHadler.PrintHelp("width");
        argHadler.PrintHelp("height");
        argHadler.PrintHelp("minWidthSpace");
        argHadler.PrintHelp("minHeightSpace");
        argHadler.PrintHelp("maxRoomHeight");
        argHadler.PrintHelp("maxRoomWidth");
        argHadler.PrintHelp("minRoomHeight");
        argHadler.PrintHelp("minRoomWidth");
        argHadler.PrintHelp("seed");
        argHadler.PrintHelp("o");
        return 0;
    }

    std::cout << "WIDTH: " << argHadler.GetIntArg("width") << std::endl;
    std::cout << "DEPTH: " << argHadler.GetIntArg("height") << std::endl;
    std::cout << "----------------" << std::endl;
    std::cout << "MIN WIDTH SPACE: " << argHadler.GetIntArg("minWidthSpace") << std::endl;
    std::cout << "MIN HEIGHT SPACE: " << argHadler.GetIntArg("minHeightSpace") << std::endl;
    std::cout << "MIN ROOM WIDTH: " << argHadler.GetIntArg("minRoomWidth") << std::endl;
    std::cout << "MIN ROOM HEIGHT: " << argHadler.GetIntArg("minRoomHeight") << std::endl;
    std::cout << "MAX ROOM WIDTH: " << argHadler.GetIntArg("maxRoomWidth") << std::endl;
    std::cout << "MAX ROOM HEIGHT: " << argHadler.GetIntArg("maxRoomHeight") << std::endl;
    std::cout << "----------------" << std::endl;
    std::cout << "SEED:" << argHadler.GetIntArg("seed") << std::endl;

    dungeonMatrix * dungeon = generateDungeon(
        argHadler.GetIntArg("width"),
        argHadler.GetIntArg("height"),
        argHadler.GetIntArg("minWidthSpace"),
        argHadler.GetIntArg("minHeightSpace"),
        argHadler.GetIntArg("minRoomWidth"),
        argHadler.GetIntArg("minRoomHeight"),
        argHadler.GetIntArg("maxRoomWidth"),
        argHadler.GetIntArg("maxRoomHeight"),
        argHadler.GetIntArg("seed"));


    std::string filename = argHadler.GetStringArg("o");

    if(dun2File(dungeon,filename))
    {
        std::cout << "archivo generado" << std::endl;
    }
    else
    {
        std::cout << "el archivo no se ha podido generar" << std::endl;
    }

    return 0;
}