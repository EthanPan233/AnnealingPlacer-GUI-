#include "draw.h"
#include <stdio.h>
#include "graphics.h"
#include <iostream>

void drawInitCircuit(Circuit circuit) {
    init_graphics("Annealing Placement -- Ethan Pan");
    clearscreen();
    update_message("Annealing Placement");
    init_world (0.,0.,2500.,2500.);

    int xSize = circuit.getXSize();
    int ySize = circuit.getYSize();

    // draw grid
    for (int x=0; x<=xSize; x++) {
        setlinestyle (SOLID);
        setlinewidth (3);
        setcolor(BLACK);
        drawline(0.0, (x+2.)*100., ySize*100., (x+2)*100.);
    }
    for (int y=0; y<=ySize; y++) {
        drawline(y*100., 200., y*100., (xSize+2)*100.);
    }
    flushinput();
}

void drawGrids(Circuit circuit) {
    int xSize = circuit.getXSize();
    int ySize = circuit.getYSize();

    // draw grid
    for (int x=0; x<=xSize; x++) {
        setlinestyle (SOLID);
        setlinewidth (3);
        setcolor(BLACK);
        drawline(0.0, (x+2.)*100., ySize*100., (x+2)*100.);
    }
    for (int y=0; y<=ySize; y++) {
        drawline(y*100., 200., y*100., (xSize+2)*100.);
    }
    flushinput();
}

void drawCircuit(Circuit circuit) {
    init_graphics("Annealing Placement -- Ethan Pan");
    clearscreen();
    update_message("Annealing Placement");
    init_world (0.,0.,2500.,2500.);

    int xSize = circuit.getXSize();
    int ySize = circuit.getYSize();

    // draw cells and cell tiles
    for (int x=0; x<xSize; x++) {
        for (int y=0; y<ySize; y++) {
            if (circuit.isOccupied(x,y)) {
                drawTile(x, y, DARKGREY);
            } else {
                drawTile(x, y, WHITE);
            }
        }
    }

    // draw grid
    for (int x=0; x<=xSize; x++) {
        setlinestyle (SOLID);
        setlinewidth (3);
        setcolor(BLACK);
        drawline(0.0, (x+2.)*100., ySize*100., (x+2)*100.);
    }
    for (int y=0; y<=ySize; y++) {
        drawline(y*100., 200., y*100., (xSize+2)*100.);
    }
    flushinput();
}


void drawTile(int x, int y, enum color_types COLOR) {
    setcolor(COLOR);
    fillrect (y*100.,(x+2)*100.,(y+1)*100.,(x+3)*100.);
    // std::cout << "draw tile: " << x << " " << y << std::endl;
}

void drawCellLine(int x1, int y1, int x2, int y2, enum color_types COLOR) {
    setcolor(COLOR);
    drawline(y1*100.+50., (x1+2.)*100.+50., y2*100.+50., (x2+2.)*100.+50.);
    flushinput();
}