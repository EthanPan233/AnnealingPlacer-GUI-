# include "Circuit.h"
# include <fstream>
# include <sstream>
# include <iostream>
# include <limits>
# include <cassert>
# include <stdexcept>
# include "math.h"
# include <numeric>
# include "draw.h"
using namespace std;


template<typename T>
T variance(const std::vector<T> &vec) {
    const size_t sz = vec.size();
    if (sz == 1) {
        return 0.0;
    }
    // Calculate the mean
    const T mean = std::accumulate(vec.begin(), vec.end(), 0.0) / sz;

    // Now calculate the variance
    auto variance_func = [&mean, &sz](T accumulator, const T& val) {
        return accumulator + ((val - mean)*(val - mean) / (sz - 1));
    };
    return std::accumulate(vec.begin(), vec.end(), 0.0, variance_func);
}

void Circuit::loadCircuit(const string& filename) {
    // spdlog::info("Start reading file: {}", filename); 
    cout << "Start reading file: " << filename << endl;
    ifstream infile(filename);
    string line;
    getline(infile, line);
    // cout << "line: " << line << endl;
    istringstream issFirstLine(line);
    int numCells, numNets, xxSize, yySize;
    issFirstLine >> numCells >> numNets >> xxSize >> yySize;
    // cout <<"numCells, numWires, xSize, ySize: " << numCells << " " << numWires << " " << xxSize << " " << yySize << endl;
    *this = Circuit(xxSize, yySize, numNets);
    // cout << "Finish reading the first line" << endl;

    // generate random non-repeating locations
    // first generate a sequantial array, then shuffle it
    vector<int> coor(xSize*ySize);
    for (int i = 0; i < xSize*ySize; i++) { coor[i]=i; }
    random_shuffle(coor.begin(), coor.end());
    coor.erase(coor.begin()+numCells, coor.end());
    vector<pair<int, int>> coorPairs(numCells);
    for (int i=0; i<numCells; i++) {
        coorPairs[i].first = int(coor[i] / ySize);
        coorPairs[i].second = coor[i] % ySize;
    }
    for (int i = 0; i < numCells; i++) {
        addCell(coorPairs[i].first, coorPairs[i].second, i);
    }

    for (int i = 0; i < numNets; i++) {
        // cout << "Start reading a new line!" << endl;
        getline(infile, line);
        istringstream issCellAndNets(line);
        int numCellsOnNet;
        issCellAndNets >> numCellsOnNet;
        vector<int> cellIds(numCellsOnNet, -1);
        for (int j = 0; j < numCellsOnNet; j++) {
            int cellId;
            issCellAndNets >> cellId;
            cells[cellId].assignToNet(i);
            nets[i].push_back(cellId);
        }
        // cout << "location: "<< coorPairs[i].first << " " << coorPairs[i].second << endl;
    }
    cout << "Finish reading file: " << filename << endl; 
}

const void Circuit::printCircuit() {
    cout << "***********************************************************" << endl;
    for (int x = 0; x < xSize; x++) {
        for (int y = 0; y < ySize; y++) {
            if (cellIds[x][y] >= 0) cout << "x";
            else cout << "o";
            cout << " ";
        }
        cout << endl;
    }
    cout << "***********************************************************" << endl;
    cout << "xSize: " << xSize << ", ySize: " << ySize << endl;
    cout << "All cells are listed below: " << endl;
    for (auto cell : cells) {
        cout << "cell " << cell.cellId << ": (" << cell.getLocation().x << " " << cell.getLocation().y << "): ";
        for (auto net : cell.getNetIds()) {
            cout << net << " ";
        }
        cout << endl;
    }
}

const void Circuit::printBoundingBoxes() {
    cout << "All bounding boxes are listed below: " << endl;
    for (int netId = 0; netId < boundingBoxes.size(); netId++) {
        cout << "box " << netId << ": (" << boundingBoxes[netId].lx() << ", " << boundingBoxes[netId].ly() << ") (" << boundingBoxes[netId].hx() << ", " << boundingBoxes[netId].hy() << ")" << endl;
    }
}

const utils::PointT<int> Circuit::getCellLocation(const int& cellId) {
    for (auto cell : cells) {
        if (cell.cellId == cellId) {
            return getCellLocation(cell);
        }
    }
    std::cerr << "Can't find the location of target cell" << cellId << endl;
    assert(false);
}

void Circuit::updateAllBoundingBoxes() {
    int numNets = nets.size();
    wireLength = 0;
    for (int netId = 0; netId < numNets; netId++) {
        int minX = numeric_limits<int>::max();
        utils::IntervalT<int> xInterval;
        utils::IntervalT<int> yInterval;
        for (int pinId = 0; pinId < nets[netId].size(); pinId++) {
            int cellId = nets[netId][pinId];
            int x = getCellLocation(cellId).x;
            int y = getCellLocation(cellId).y;
            xInterval.Update(x);
            yInterval.Update(y);
        }
        boundingBoxes[netId] = utils::BoxT<int>(xInterval, yInterval);
        wireLength += boundingBoxes[netId].hp();
    }
}

void Circuit::updateTargetBoundingBox(const vector<int>& netIds) {
    for (auto netId : netIds) {
        wireLength -= boundingBoxes[netId].hp();
        int minX = numeric_limits<int>::max();
        utils::IntervalT<int> xInterval;
        utils::IntervalT<int> yInterval;
        for (int pinId = 0; pinId < nets[netId].size(); pinId++) {
            int cellId = nets[netId][pinId];
            int x = getCellLocation(cellId).x;
            int y = getCellLocation(cellId).y;
            xInterval.Update(x);
            yInterval.Update(y);
        }
        boundingBoxes[netId] = utils::BoxT<int>(xInterval, yInterval);
        wireLength += boundingBoxes[netId].hp();
    }
}

double Circuit::updateWL() {
    wireLength = 0;
    for (auto box : boundingBoxes) {
        wireLength += box.hp();
    }
    return wireLength;
}

inline void Circuit::swapCells(Cell& cell1, Cell& cell2) {
    swapCells(cell1.cellId, cell2.cellId);
}

void Circuit::swapCells(const int& cellId1, const int& cellId2) {
    assert(cellId1<=cells.size() && cellId2<=cells.size());
    if (cellId1 == cellId2) return;
    int idx1 = 0;
    int idx2 = 0;
    utils::PointT<int> location1;
    utils::PointT<int> location2;
    vector<int> netsToUpdate;
    for (int i=0; i<cells.size(); i++) {
        if (cells[i].cellId == cellId1) {
            location1 = cells[i].getLocation();
            idx1 = i;
            // get the netIds of swapped cells
            vector<int> tempNetList = cells[i].getNetIds();
            for (auto netId : tempNetList) {
                if (find(netsToUpdate.begin(), netsToUpdate.end(), netId) == netsToUpdate.end()) netsToUpdate.push_back(netId);
            }
        } else if (cells[i].cellId == cellId2) {
            location2 = cells[i].getLocation();
            idx2 = i;
            // get the netIds of swapped cells
            vector<int> tempNetList = cells[i].getNetIds();
            for (auto netId : tempNetList) {
                if (find(netsToUpdate.begin(), netsToUpdate.end(), netId) == netsToUpdate.end()) netsToUpdate.push_back(netId);
            }
        }
    }
    assert(idx1>=0 && idx2>=0 && location1.IsValid() && location2.IsValid());
    cells[idx1].moveCellTo(location2);
    cells[idx2].moveCellTo(location1);

    // wire length is updated here, only two nets are re-computed
    updateTargetBoundingBox(netsToUpdate);
    int temp = cellIds[location1.x][location1.y];
    cellIds[location1.x][location1.y] = cellIds[location2.x][location2.y];
    cellIds[location2.x][location2.y] = temp;
}

void Circuit::moveCellTo(Cell& cell, const int& x, const int& y) { 
    if (cell.getLocation().x == x && cell.getLocation().y == y) return;
    if (cellIds[x][y] >= 0) swapCells(cell.cellId, cellIds[x][y]);
    else {
        utils::PointT<int> location = cell.getLocation();
        cellIds[location.x][location.y] = -1; // remove cell from previous location
        cell.moveCellTo(x, y);
        cellIds[x][y] = cell.cellId; // move cell to the new location
        updateTargetBoundingBox(cell.getNetIds());
    }
}

void Circuit::randomMove() {
    int cellToMove = rand() % cells.size(); // random cell
    int x = rand() % xSize; // randon coordinate
    int y = rand() % ySize; 
    moveCellTo(cells[cellToMove], x, y); // move cell to there
}

void Circuit::annealingPlacement() {
    // perform 50 random moves to get initial tempeture
    vector<double> costs50(50);
    for (int i = 0; i < 50; i++) {
        double oldCost = getCost();
        randomMove();
        double newCost = getCost();
        costs50[i] = newCost;
    }
    double temp = 20 * sqrt(variance(costs50)); // initial tempture

    int counts = 0;
    int numIts = static_cast<int>(10 * pow(cells.size(), 4/3));
    cout << "Initial tempture is " << temp << ". Number of iterations for each tempture is " << numIts << endl;
    while(true) {
        vector<double> costs;
        for (int i=0; i<numIts; i++) { // for some numbers of its
            int cellToMove = rand() % cells.size(); // random cell
            utils::PointT<int> oldLocation = cells[cellToMove].getLocation();
            double oldCost = getCost();
            int x = rand() % xSize; // randon coordinate
            int y = rand() % ySize; 
            moveCellTo(cells[cellToMove], x, y); // move cell to there
            double newCost = getCost();
            double r = static_cast<double> (rand()) / static_cast<double> (RAND_MAX); // random number between 0 and 1
            if (r > exp((oldCost-newCost)/temp)) { // randomly revoke move
                moveCellTo(cells[cellToMove], oldLocation.x, oldLocation.y);
            } else { // take move
                costs.push_back(newCost);
                enum color_types color = DARKGREY;
                if (temp > 200) color = RED;
                else if (temp > 100) color = MAGENTA;
                else if (temp > 25) color = YELLOW;
                else if (temp > 5) color = GREEN;
                else if (temp > 1) color = BLUE;
                else if (temp > 0.1) color = CYAN;
                else if (temp > 0.01) color = DARKGREEN;
                else color = LIGHTGREY;
                drawCellLine(oldLocation.x, oldLocation.y, x, y, color);
            }
            counts++;
            if (counts % 100 == 0) {
                cout << "Iteration " << counts << ", current T " << temp << ", cost " << getCost() << endl;
                drawGrids(*this);
            }
        }
        double devCost = sqrt(variance(costs));
        if (devCost < 0.001) {
            cout << "Iteration " << counts << ", placement finished! Final cost: " << getCost() << endl;
            break;
        } else if (counts > 10000000) {
            cout << "Iteration " << counts << ", optimal placement couldn't be found!" << endl;
            break;
        }
        if (counts % 1 == 0) {
            cout << "Iteration " << counts << ", current T " << temp << ", deviation of costs " << devCost << ", cost " << getCost() << endl;
        }
        // Update tempture
        if (temp < 0.001) temp = 0; // use greedy
        else{
            temp = temp * exp(-0.7*temp/devCost);
        }
    }
}
