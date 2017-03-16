import processing.core.*; 
import processing.data.*; 
import processing.event.*; 
import processing.opengl.*; 

import processing.serial.*; 
import processing.net.*; 
import pathfinder.*; 

import java.util.HashMap; 
import java.util.ArrayList; 
import java.io.File; 
import java.io.BufferedReader; 
import java.io.PrintWriter; 
import java.io.InputStream; 
import java.io.OutputStream; 
import java.io.IOException; 

public class Controls extends PApplet {





Client server_socket;
Serial xbee_comm;
Car[] vehicle;

// Create button for vehicles, stations, and controls
//   Button button_name = new Button(x_position, y_position, width, height, shape(sides), "Label", font_size, color(0,0,0));
    //Button[] vehicle_button = new Button[10];
    Button[] station_button = new Button[8];

// Contorl Buttons
    Button up    = new Button(1218, 370, 30, 30, 3);
    Button down  = new Button(1218, 535, 30, -30, 3);
    Button go    = new Button(1056, 560, 150, 50, 4, "Go", 35, color(50, 180, 50));
    Button stop  = new Button(1256, 560, 150, 50, 4, "Stop", 35, color(180, 50, 50));
    Button servo = new Button(1056, 620, 150, 50, 4, "Switch", 35, color(50, 180, 200));
    Button state = new Button(1256, 620, 150, 50, 4, "Status", 35, color(200, 200, 50));
    Button all = new Button(1365, 290, 36, 36, 1, "A", 24, color(235, 0, 0));


// Controls variables
    int v_focus = 0;
    int set_speed = 0;
    PImage logo;
    int _mouseX, _mouseY;


// Control panel positioing variable
    int control_x = 1020; 
    
// Debug mode flag
    boolean debug_mode = true;


// Pathfinding Library setup
    Graph[] gs = new Graph[1];
    PImage[] graphImage = new PImage[4];
    int start = 0;
    int end = 0;
    float nodeSize[] = new float[4];
    
    GraphNode[] gNodes, rNodes;
    GraphEdge[] gEdges, exploredEdges;
    
    // Pathfinder algorithm
    IGraphSearch pathFinder;
    
    // Used to indicate the start and end nodes as selected by the user.
    GraphNode startNode, endNode;
    boolean selectMode = false;
    long time;
    int graphNo = 0;
    int algorithm = 1;
    int overAlgorithm, overOption, overGraph;
    int offX = 10, offY = 10;
    PrintWriter output;

public void setup()
{
    // Initialize window for User Interface
      
    background(230);
    surface.setTitle("1/12 Scale Control");

    // Print available serial ports
    printArray(Serial.list());

    // Try to connect to serial port, if unavailable throw error
    try { 
        xbee_comm = new Serial(this, Serial.list()[0], 57600);
        xbee_comm.bufferUntil('\n');
    } 
    catch (IndexOutOfBoundsException e) {
        System.err.println("IndexOutOfBoundsException: " + e.getMessage());
    }

    // Initialize array for 10 vehicles
    vehicle = new Car[10];

    // Draw initial control panel
    drawinf();

    // Add button for vehicle selection & initialize vehicles
    for (int veh = 0; veh < vehicle.length; veh++)
    {
        vehicle[veh] = new Car(veh);
        vehicle[veh].button = new Button(veh*(width / 16)+35, 65, 70, 70, 1, str(veh+1), 35, color(0x03, 0xA9, 0xF4));
        vehicle[veh].dest = -1;
    }

    // Add button for station
    int row = 0, col = 0;
    for (int sta = 0; sta < 8; sta++) {
        col = sta;
        if (sta >= 4) {
            row = 1; 
            col = sta-4;
        }
        station_button[sta] = new Button((col*(width / 16))+1065, row*(width/16)+65, 70, 70, 1, str(sta+1), 35, color(0xEF, 0x6C, 0x00));
    }

    
    ellipseMode(CENTER);

    graphNo = 0;
    nodeSize[graphNo] = 12.0f;
    graphImage[graphNo] = loadImage("Track.JPG");
    gs[graphNo] = new Graph();
    makeGraphFromFile(gs[graphNo], "Superway_track.txt");
    gs[graphNo].compact();

    gNodes =  gs[graphNo].getNodeArray();
    gEdges = gs[graphNo].getAllEdgeArray();
    // Create a path finder object based on the algorithm
    pathFinder = new GraphSearch_BFS(gs[graphNo]);
    usePathFinder(pathFinder);
}

// Draw user interface and button
public void drawinf()
{
    // Draw controls box
    stroke(0); 
    strokeWeight(3); 
    fill(255);
    rect(control_x, 260, width - control_x, height);

    // Draw labels for vehicle and station selection
    textSize(72);
    fill(255, 255, 255, 120);
    text("Vehicles", 50, 70);
    text("Stations", control_x+50, 70);

    // Draw components for control panel
    textSize(52);
    fill(0, 0, 0, 120);
    text("Controls", control_x+100, 330);
    fill(0, 0, 0, 30);
    rect(control_x+60, 412, 300, 80, 20);
    line(control_x, 0, control_x, height);
    line(0, 150, control_x, 150);

    // Draw Superway logo
    logo = loadImage("super_logo.png");
    image (logo, 1070, 720);
    line(control_x, 700, width, 700);

    try {
        text(vehicle[v_focus].speed, 1040, 160);
    } 
    catch(NullPointerException npe) {
        println("Error: No vehicles active.");
    }
}

// Looping block
public void draw ()
{
    // Update vehicle and station button graphics
    for (int veh = 0; veh < 10; veh++) {
        if     ((veh == vehicle[v_focus].dest && veh >= 10) || (veh == v_focus))
            vehicle[veh].button.in_focus = true;
        else if ((veh != vehicle[v_focus].dest && veh >= 10) || (veh != v_focus))
            vehicle[veh].button.in_focus = false;

        vehicle[veh].button.update();
        vehicle[veh].checkState();
    }

    for (int sta = 0; sta < 8; sta++) {
        station_button[sta].update();
    }

    // Update control button graphics
    up.update(); 
    down.update(); 
    stop.update(); 
    go.update();
    servo.update(); 
    state.update(); 
    all.update();
    
    // Update pathfinding graphics
    image(graphImage[0], 200, 200);
    drawNodes();
    drawEdges(gEdges, color(192, 192, 192, 128), 1.0f, true);
    drawRoute(rNodes, color(200, 0, 0), 5.0f);

    // Update Set speed window
    fill(220); 
    stroke(0);
    rect(control_x + 60, 410, 300, 85, 20);
    textSize(72);
    fill(180);
    text(set_speed, control_x + 145, 480);
}

public void mousePressed()
{   
    _mouseX = mouseX; 
    _mouseY = mouseY;

    // Vehicle/Station button Select
    for (int veh = 0; veh < 10; veh++) {
        vehicle[veh].button.select();

        if (vehicle[veh].button.click)
            v_focus = veh;
    }

    for (int sta = 0; sta < 8; sta++) {
        station_button[sta].select();

        if (station_button[sta].click){ // && vehicle[v_focus].status == 1) {
            vehicle[v_focus].dest = sta;
            goToStation(v_focus+1, sta);
        }
    }

    //Check for control button clicks
    up.select(); 
    down.select(); 
    stop.select(); 
    go.select();
    servo.select(); 
    state.select(); 
    all.select();


    // Vehicle control commands
    if (go.click) {
        // See goCommand function under communication
        goCommand(v_focus+1);
    } else if (stop.click) {
        String output ; 
        output = "" + (char)(v_focus+1) + (char)0x00 + (char)0x02 + (char)0x00;
        sendMessage(xbee_comm, output);
    } else if (servo.click) {
        String output ; 
        output = "" + (char)(v_focus+1) + (char)0x00 + (char)0x07 + (char)0x00;
        sendMessage(xbee_comm, output);
    } else if (state.click) {
        requestState(v_focus+1);
    } else if (up.click) {
        set_speed += 15;
        if (set_speed > 255)
            set_speed = 255;
    } else if (down.click) {
        set_speed -= 15;
        if (set_speed < 0)
            set_speed = 0;
    }

    _mouseX = -1; 
    _mouseY = -1;
}
public class Button {
    // Button class, create a new button
    // Button button_name = new Button(x_position, y_position, width, height, shape(sides), "Label", font_size, color(0,0,0));
    
    int xpos, ypos, w, h, shape, font_size;
    String label;
    boolean click, over, in_focus = false, online = false;
    int clr, bright;

    Button(int x, int y, int w, int h, int s, String lbl, int sz, int clr) {
        this.xpos = x;       this.ypos = y;
        this.w = w;          this.h = h;
        this.label = lbl;    this.font_size = sz;
        this.clr = clr;
        this.shape = s;

    }

    Button(int x, int y, int w, int h, int s) {
        this.xpos = x;    this.ypos = y;
        this.w = w;       this.h = h;
        this.shape = s;
        this.label = "";
        this.clr = color(180, 180, 180);
    }

    public void update() {
        if (mouseX>xpos && mouseX<xpos+w && ((mouseY<ypos+h && mouseY>ypos && h>0) || (mouseY>ypos+h && mouseY<ypos && h<0))) {
            over = true; 
        } else {
            over = false;
        }

        fill(clr); 
        
        if(online == true)
            stroke(255);
        else
            stroke(60);

        if (over && !in_focus)
            fill(color(red(clr)+30, green(clr)+30, blue(clr)+30));
        else if (in_focus)
            fill(color(red(clr)+65, green(clr)+65, blue(clr)+65));

        if (this.shape == 1) {
            ellipse(xpos+(w/2), ypos+(h/2), w, h);
        } else if (this.shape == 3) {
            triangle(xpos+(w/2), ypos, xpos, ypos+h, xpos+w, ypos+h);
        } else if (this.shape == 4) {
            rect(xpos, ypos, w, h, 10);
        }

        if (label != "") {
            fill(0); 
            textSize(font_size); 
            textAlign(LEFT);
            text(label, xpos+(w/2)-(textWidth(label)/2), ypos+(h/2)+(textAscent()/2.5f));
        }
    }
    
    public void select(){
        if (_mouseX>xpos && _mouseX<xpos+w && ((_mouseY<ypos+h && _mouseY>ypos && h>0) || (_mouseY>ypos+h && _mouseY<ypos && h<0)))
            click = true; 
        else
            click = false;
    }
}
public class Car
{
    int id;
    int status = 0;    // 0 = Not connected, 1 = Connected/Idle, 2 = Active
    int dest = -1;     // Station number
    int on_track, speed;
    long last_ping;
    double distance;
    Button button;
    String last_message;
    

    public Car(int id) {
        on_track    = 101;    distance    = 0;
        status      = 0;      speed       = 0;
        this.id     = id;
    }
    
    public void checkState() {
        if(millis() - last_ping > 3000){
            status = 0;
            button.online = false;
        }
    }
}
// Read input from serial communications
public void serialEvent(Serial xbee_comm) 
{
    byte[] xbee_in = new byte[25];
    boolean string_enable = false;
    byte checksum = 0x00;
    int msg_pos = 0;
    
    while(xbee_comm.available() > 0){
        // Get new byte:
        byte char_in = (byte)xbee_comm.read();

        // End message
        if (char_in == (byte)0x7E && string_enable == true) {
            string_enable = false;
            xbee_comm.clear();
            
            // Check for errors
            if(checksum == 0)
                process_data(xbee_in);
            else
                requestState((int)xbee_in[1]);
        }
    
        // Record incomming message
        if (string_enable == true){
            xbee_in[msg_pos] = char_in;
            checksum += char_in;
            msg_pos++;
        }
        
        // Start Message
        if (char_in == (byte)0x81 && string_enable == false)
            string_enable = true;
            
        if (debug_mode == true && string_enable == false && char_in != (byte)0x81 && char_in != (byte)0x7E)
            System.out.print(PApplet.parseChar(char_in));
    }
}

// Write message out to vehicle(s) 
public void sendMessage(Serial port, String message){
    byte checksum = 0x00;
    vehicle[message.charAt(0)-1].last_message = message;
    
    // Write out message start character
    port.write(0x81);
    
    // Write message
    for(int ltr = 0; ltr < message.length(); ltr++){
        port.write(message.charAt(ltr));
        checksum += message.charAt(ltr);
    }
    
    // Write out checksum and message end character
    port.write(0-checksum);
    port.write(0x7E);
}

// Process input data
public void process_data(byte[] input){
    if(input[2] == 0x30){        // Vehicle connected
        vehicle[(int)input[1]-1].button.online = true;
        vehicle[(int)input[1]-1].last_ping = millis();
        System.out.println("Vehicle " + (int)input[1] + " connected.");
    }
    
    else if(input[2] == 0x05){    // Vehicle status update
        vehicle[(int)input[1]-1].status = (int)input[3];
        vehicle[(int)input[1]-1].speed = (input[6]<<8) | (input[7] & 0xFF);
        vehicle[(int)input[1]-1].on_track = (input[4]<<8) | (input[5] & 0xFF);
        vehicle[(int)input[1]-1].last_ping = millis();
        
        String message_out = "Vehicle " + (int)input[1] + " State: " + (int)input[3] + " Speed: " + vehicle[(int)input[1]-1].speed + " TrackID: " + vehicle[(int)input[1]-1].on_track;
        
        System.out.println(message_out);
    }
    else if(input[2] == 0x08){    // Vehicle ping/heartbeat
        vehicle[(int)input[1]-1].last_ping = millis();
        vehicle[input[1]-1].button.online = true;
    }
    else if(input[2] == 0x20){    // Vehicle received instructions
        String message_out = "Vehicle " + (int)input[1] + " instructions received.";
        System.out.println(message_out);
    }
    else if(input[2] == 0xF0){    // Message not received/resend message request
        sendMessage(xbee_comm, vehicle[input[1]-1].last_message);
    }
}

// Request state from vehicle
public void requestState(int vehicle){
    String output ; 
    output = "" + (char)(vehicle) + (char)0x00 + (char)0x05 + (char)0x00;
    sendMessage(xbee_comm, output);
}

// Command vehicle to go
public void goCommand(int vehicle){
        char speed[] = new char[2];
            speed[0] = (char)set_speed;
            speed[1] = (char)(set_speed >> 8);
            
        String output ; 
            output = "" + (char)(vehicle) + (char)0x00 + (char)0x01 + (char)0x00 + (char)speed[0] + (char)speed[1];
            sendMessage(xbee_comm, output);
}
public void goToStation(int car, int station) {

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

public void drawNodes() {
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

public void usePathFinder(IGraphSearch pf) {
    time = System.nanoTime();
    pf.search(start, end, true);
    time = System.nanoTime() - time;
    rNodes = pf.getRoute();
    exploredEdges = pf.getExaminedEdges();
}

public void makeGraphFromFile(Graph g, String fname) {
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

public void makeNode(String s, Graph g) {
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


public void makeEdge(String s, Graph g) {
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

public void drawEdges(GraphEdge[] edges, int lineCol, float sWeight, boolean arrow) {
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

public void drawArrow(GraphNode fromNode, GraphNode toNode, float nodeRad, float arrowSize) {
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

public void drawRoute(GraphNode[] r, int lineCol, float sWeight) {
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
  public void settings() {  size(1440, 840);  smooth(); }
  static public void main(String[] passedArgs) {
    String[] appletArgs = new String[] { "--present", "--window-color=#666666", "--stop-color=#cccccc", "Controls" };
    if (passedArgs != null) {
      PApplet.main(concat(appletArgs, passedArgs));
    } else {
      PApplet.main(appletArgs);
    }
  }
}
