# pragma once
# include "geo.h"
# include <algorithm>
using namespace std;

class Cell {
public:
    Cell(const int& xx, const int& yy, const int& id) { location.x = xx; location.y = yy; cellId = id; }
    Cell(const int& xx, const int& yy, const vector<int>& netsOfCell) 
        : location(utils::PointT<int>(xx,yy)), netIds(netsOfCell) {}
    Cell() {}

    const int getX() { return location.x; }
    const int getY() { return location.y; }
    vector<int> getNetIds() { return netIds; }

    void moveCellTo(int xx, int yy) { location.x = xx; location.y = yy; }
    void moveCellTo(utils::PointT<int> location) { moveCellTo(location.x, location.y); }
    void assignToNet(int netId) { netIds.push_back(netId); }
    void removeNet(int netId) { 
        netIds.erase(remove(netIds.begin(), netIds.end(), netId), netIds.end());
    }
    const utils::PointT<int> getLocation() { return location; }


    int cellId = -1;
private:
    utils::PointT<int> location;
    vector<int> netIds;
};