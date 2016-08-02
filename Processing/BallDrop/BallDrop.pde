// Processing example for receiving predictions from ESP
// Draws a small ball (circle) dropping from the top of the window
// to the bottom. ESP predictions make the ball move left (class 1)
// and right (class 2).

import processing.net.*; // include the networking library

Server server; // will receive predictions from ESP
int x, y;

void setup()
{
  size(400, 800);
  server = new Server(this, 5204); // listen on port 5204
  
  // start at the middle of the top of the window
  x = width / 2;
  y = 0;
}

void draw()
{
  // check for incoming data
  Client client = server.available();
  if (client != null) {
    // check for a full line of incoming data
    String line = client.readStringUntil('\n');
    if (line != null) {
      println(line);
      int val = int(trim(line)); // extract the predicted class
      if (val == 1) x -= 20; // move left when class 1 occurs
      if (val == 2) x += 20; // move right when class 2 occurs
    }
  }
  
  // draw
  background(0);
  fill(0, 255, 0);
  ellipse(x, y, 20, 20); // draw the ball at position (x, y)
  
  y += 3; // drop the ball
  if (y > height) y = 0; // if it hits the botton, wrap around to top
}