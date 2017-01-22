void goToStation(int car, int station) {

    //startNode = gs[graphNo].getNodeAt(255, 460, 0, 16.0f);
    start = vehicle[car-1].on_track;

    switch(station) {
        case 0:
            endNode = gs[graphNo].getNode(101);
            break;
    
        case 1:
            endNode = gs[graphNo].getNode(201);
            break;
    
        case 2:
            endNode = gs[graphNo].getNode(301);
            break;
    
        case 3:
            endNode = gs[graphNo].getNode(401);
            break;
    }

    end = endNode.id();
    usePathFinder(pathFinder);
    
    String instructions_out = "" + (char)(car) + (char)0x00 + (char)0x10 + (char)0x00;
    print("Vehicle " + car + " route: ");
 
    for (int step=1; step < rNodes.length; step++) {
        int track_id = rNodes[step].id();
        int instruct = 0;

        char track[] = new char[2];
             track[0] = (char)track_id;
             track[1] = (char)(track_id >> 8);
             
        if(step < rNodes.length-1)
             instruct = gs[graphNo].getEdge(rNodes[step].id(),rNodes[step+1].id()).side();
        else
            instruct = 0;
            
        instructions_out += "" + (char)track[1] + (char)track[0] + (char)instruct;
        
        print(track_id + " " + instruct + "  ");
    }
    println();
    
    sendMessage(xbee_comm, instructions_out);
}

void drawNodes() {
    pushStyle();
    noStroke();
    fill(0);
    for (GraphNode node : gNodes)
        ellipse(node.xf(), node.yf(), nodeSize[graphNo], nodeSize[graphNo]);
    popStyle();
    fill(255, 0, 0, 100);
    for (GraphNode node2 : rNodes)
        ellipse(node2.xf(), node2.yf(), nodeSize[graphNo], nodeSize[graphNo]);
}

void usePathFinder(IGraphSearch pf) {
    time = System.nanoTime();
    pf.search(start, end, true);
    time = System.nanoTime() - time;
    rNodes = pf.getRoute();
    exploredEdges = pf.getExaminedEdges();
}

void makeGraphFromFile(Graph g, String fname) {
    String lines[];
    lines = loadStrings(fname);
    int mode = 0;
    int count = 0;
    while (count < lines.length) {
        lines[count].trim();
        
        if (!lines[count].startsWith("#") && lines[count].length() > 1) {
            switch(mode) {
                case 0:
                    if (lines[count].equalsIgnoreCase("<nodes>"))
                        mode = 1;
                    else if (lines[count].equalsIgnoreCase("<edges>"))
                        mode = 2;
                    break;
                case 1:
                    if (lines[count].equalsIgnoreCase("</nodes>"))
                        mode = 0;
                    else 
                        makeNode(lines[count], g);
                    break;
                case 2:
                    if (lines[count].equalsIgnoreCase("</edges>"))
                        mode = 0;
                    else
                        makeEdge(lines[count], g);
                    break;
            } // end switch
        } // end if
        count++;
    } // end while
}

void makeNode(String s, Graph g) {
    int nodeID;
    float x, y, z = 0;
    String part[] = split(s, " ");
    
    if (part.length >= 3) {
        nodeID = Integer.parseInt(part[0]);
        x = Float.parseFloat(part[1]);
        y = Float.parseFloat(part[2]);
        
        if (part.length >=4)
            z = Float.parseFloat(part[3]);
            
        g.addNode(new GraphNode(nodeID, x, y, z));
    }
}


void makeEdge(String s, Graph g) {
    int fromID, toID, side;
    float costOut = 0;
    String part[] = split(s, " ");
   
    if (part.length >= 4) {
        fromID = Integer.parseInt(part[0]);
        toID = Integer.parseInt(part[1]);
        costOut = Float.parseFloat(part[2]);
        side = Integer.parseInt(part[3]);

        g.addEdge(fromID, toID, costOut, side);
    }
}

void drawEdges(GraphEdge[] edges, int lineCol, float sWeight, boolean arrow) {
    if (edges != null) {
        pushStyle();
        noFill();
        stroke(lineCol);
        strokeWeight(sWeight);
        for (GraphEdge ge : edges) {
            if (arrow)
                drawArrow(ge.from(), ge.to(), nodeSize[graphNo] / 2.0f, 6);
            else {
                line(ge.from().xf(), ge.from().yf(), ge.to().xf(), ge.to().yf());
            }
        }
        popStyle();
    }
}

void drawArrow(GraphNode fromNode, GraphNode toNode, float nodeRad, float arrowSize) {
    float fx, fy, tx, ty;
    float ax, ay, sx, sy, ex, ey;
    float awidthx, awidthy;

    fx = fromNode.xf();
    fy = fromNode.yf();
    tx = toNode.xf();
    ty = toNode.yf();

    float deltaX = tx - fx;
    float deltaY = (ty - fy);
    float d = sqrt(deltaX * deltaX + deltaY * deltaY);

    sx = fx + (nodeRad * deltaX / d);
    sy = fy + (nodeRad * deltaY / d);
    ex = tx - (nodeRad * deltaX / d);
    ey = ty - (nodeRad * deltaY / d);
    ax = tx - (nodeRad + arrowSize) * deltaX / d;
    ay = ty - (nodeRad + arrowSize) * deltaY / d;

    awidthx = - (ey - ay);
    awidthy = ex - ax;

    noFill();
    strokeWeight(4.0f);
    stroke(160, 128);
    line(sx, sy, ax, ay);

    noStroke();
    fill(48, 128);
    beginShape(TRIANGLES);
    vertex(ex, ey);
    vertex(ax - awidthx, ay - awidthy);
    vertex(ax + awidthx, ay + awidthy);
    endShape();
}

void drawRoute(GraphNode[] r, int lineCol, float sWeight) {
    if (r.length >= 2) {
        pushStyle();
        stroke(lineCol);
        strokeWeight(sWeight);
        noFill();
        for (int i = 1; i < r.length; i++)
            line(r[i-1].xf(), r[i-1].yf(), r[i].xf(), r[i].yf());
        // Route start node
        strokeWeight(2.0f);
        stroke(0, 0, 160);
        fill(0, 0, 255);
        ellipse(r[0].xf(), r[0].yf(), nodeSize[graphNo], nodeSize[graphNo]);
        // Route end node
        stroke(160, 0, 0);
        fill(255, 0, 0);
        ellipse(r[r.length-1].xf(), r[r.length-1].yf(), nodeSize[graphNo], nodeSize[graphNo]); 
        popStyle();
    }
}