# include "Circuit.h"
# include <filesystem>
# include "draw.h"
# include <ctime>
# include "graphics.h"
namespace fs = std::filesystem;
using namespace std;

int main(int argc, char *argv[]) {
    srand(time(0));
    string path = "../test_files"; 
    string testFile = argv[1] + string(".txt");

    cout << "Start doing placement on " << testFile << "." << endl;

    Circuit circuit;
    circuit.loadCircuit(path + "/" + testFile);
    drawInitCircuit(circuit);

    circuit.updateAllBoundingBoxes();
    circuit.annealingPlacement();

    drawCircuit(circuit);

    do {
        cout << '\n' << "Press a key to continue...";
    } while (cin.get() != '\n');

    return 0;
}

