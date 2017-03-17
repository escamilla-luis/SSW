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
    
    void checkState() {
        if(millis() - last_ping > 3000){
            status = 0;
            button.online = false;
        }
    }
}