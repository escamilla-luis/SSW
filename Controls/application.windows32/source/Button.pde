public class Button {
    // Button class, create a new button
    // Button button_name = new Button(x_position, y_position, width, height, shape(sides), "Label", font_size, color(0,0,0));
    
    int xpos, ypos, w, h, shape, font_size;
    String label;
    boolean click, over, in_focus = false, online = false;
    color clr, bright;

    Button(int x, int y, int w, int h, int s, String lbl, int sz, color clr) {
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

    void update() {
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
            text(label, xpos+(w/2)-(textWidth(label)/2), ypos+(h/2)+(textAscent()/2.5));
        }
    }
    
    void select(){
        if (_mouseX>xpos && _mouseX<xpos+w && ((_mouseY<ypos+h && _mouseY>ypos && h>0) || (_mouseY>ypos+h && _mouseY<ypos && h<0)))
            click = true; 
        else
            click = false;
    }
}