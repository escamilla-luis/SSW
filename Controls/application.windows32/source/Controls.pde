import processing.serial.*;
import processing.net.*;
import pathfinder.*;

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

void setup()
{
    // Initialize window for User Interface
    size(1440, 840);  
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

    smooth();
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
void drawinf()
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
void draw ()
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

void mousePressed()
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