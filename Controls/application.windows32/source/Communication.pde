// Read input from serial communications
void serialEvent(Serial xbee_comm) 
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
            System.out.print(char(char_in));
    }
}

// Write message out to vehicle(s) 
void sendMessage(Serial port, String message){
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
void process_data(byte[] input){
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
void requestState(int vehicle){
    String output ; 
    output = "" + (char)(vehicle) + (char)0x00 + (char)0x05 + (char)0x00;
    sendMessage(xbee_comm, output);
}

// Command vehicle to go
void goCommand(int vehicle){
        char speed[] = new char[2];
            speed[0] = (char)set_speed;
            speed[1] = (char)(set_speed >> 8);
            
        String output ; 
            output = "" + (char)(vehicle) + (char)0x00 + (char)0x01 + (char)0x00 + (char)speed[0] + (char)speed[1];
            sendMessage(xbee_comm, output);
}