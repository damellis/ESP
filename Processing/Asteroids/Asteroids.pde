//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                 ___   ___  ___  ___   ___        ___        __                                 ___   ___     //
//      .'|=|`.   |   |=|_.' `._|=|   |=|_.'   .'|=|_.'   .'|=|  |   .'|=|`.     .'|   .'|=|`.   |   |=|_.'     //
//    .'  | |  `. `.  |           |   |      .'  |  ___ .'  | |  | .'  | |  `. .'  | .'  | |  `. `.  |          //
//    |   |=|   |   `.|=|`.       |   |      |   |=|_.' |   |=|.'  |   | |   | |   | |   | |   |   `.|=|`.      //
//    |   | |   |  ___  |  `.     `.  |      |   |  ___ |   |  |`. `.  | |  .' |   | |   | |  .'  ___  |  `.    //
//    |___| |___|  `._|=|___|       `.|      |___|=|_.' |___|  |_|   `.|=|.'   |___| |___|=|.'    `._|=|___|    //
//                                                                                                              //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
 
// Lambert Wang 2013
 
// To be implemented:
// In no particular order
// 1. Acceleration Boost
// 3. Statistics
// 4. Web Leaderboards
// 5. Weapon slider
 
// Statistics
float[] accuracy;
int score, highscore;
 
// Asteroids and lasers and powerups
ArrayList<Asteroid> rocks;
ArrayList<Laser> pewpew;
ArrayList<JesusChrist> helpme;
 
// Enables guns
boolean[] bigFuckingGuns;
// 1. Standard
//    Firerate: 6
//    Index: -1
// 2. Twin Laser
//    Firerate: 4
//    Index: -2
// 3. Tri Laser
//    Firerate: 8
//    Index: -3
// 4. Burst Laser
//    Firerate: 5
//    Index: -5
// 5. Railgun
//    Firerate: 18
//    Index: -6
// 6. Seeker Missiles
//    Firerate: 10
//    Index: -7
// 7. Beam Laser
//    Firerate: 8
//    Index: -9
// 8. Seeker Missile Barrage
//    Index: -10
// 9. Rapidfire Weapon
//    Index: -11
 
// Different Game Settings
int[] options;
// 0. Drive type
//    1. Static Background
//    2. Static Ship
//    3. Ship Centered
// 1. Show Framerate
//    1. No
//    2. Yes
 
// Mercury
SpaceShip mercury;
 
// Game State
int thanksObama;
// 1. Playing
// 2. Paused
// 3. Game Over
// 4. Main menu
// 5. Options Menu
// 6. Stats
 
// Keyboard controls
//    uparrow : Accelerate
// rightarrow : Rotate Right
//  leftarrow : Rotate Left
//   spacebar : Shoot
//        1-7 : Select Weapon
//       v, b : Special Weapons
boolean keyup = false;
boolean keyright = false;
boolean keyleft = false;
boolean keyshoot = false;
 
// Button selection for menus
int selection;
 
// Menu Buttons
Button start;
Button option;
Button stat;
Button back;
// Options
Button mode;
Button showframes;
 
void setup() {
  // Basic screen setup
  size(768, 512);
  frameRate(30);
  colorMode(HSB);
 
  // Immeadiately starts up the game
  // thanksObama = 1;
 
  // Sets the object arrays
  rocks = new ArrayList<Asteroid>();
  pewpew = new ArrayList<Laser>();
  helpme = new ArrayList<JesusChrist>();
  bigFuckingGuns = new boolean[10];
  options = new int[2];
  options[0] = 1;
  options[1] = 1;
  accuracy = new float[14];
  for (int i = 0; i < accuracy.length; i++) {
    accuracy[i] = 0;
  }
  highscore = 0;
 
  // Launches the Mercury
  mercury = new SpaceShip();
  mercury.launch();
 
  start = new Button(width/4, height/2-32, 64, "Start", 1);
  option = new Button(width/4+64, height/2+32, 128, "Options", 2);
  stat = new Button(width/4+128, height/2+96, 192, "Stats", 3);
  back = new Button(width/4+128, height/2+96, 96, "Menu", 4);
  mode = new Button(width/4+64, height/2+32, 0, "Flight Mode", 5);
  showframes = new Button(width/4, height/2-32, 160, "Show Framerate", 6);
 
  // Starts game at menu
  gotoMenu();
}
 
void draw() {
  // Space, the final frontier
  background(0);
 
  if (thanksObama == 1) {
    playGame();
  }
 
  if (thanksObama == 4) {
    menu();
  }
 
  if (thanksObama == 5) {
    wtfjavascript();
  }
 
  if (thanksObama == 6) {
    omfgwhydoesntthiswork();
  }
 
  if (score > highscore) {
    highscore = score;
  }
   
  // Shows the active framerate
  if (options[1] == 2) {
    fill(75, 255, 255);
    textSize(12);
    text("FPS:" + frameRate, width-64, height-8);
  }
}
 
void keyPressed() {
  if (thanksObama < 4) {
    gameKeyPressed();
  }
 
  if (thanksObama > 3) {
    menuKeyPressed();
  }
}
 
void keyReleased() {
  if (thanksObama < 4) {
    gameKeyReleased();
  }
}
 
// What are these rocks doing in my face?!?!?!?!?!
void aaaaaaaaaugh(int i) {
  rocks.remove(i);
 
  // If no shield
  if (mercury.l == 0) {
    // Translation required to center the game over message
    // Merucury Centered and Static Mercury
    if (options[0] == 3 || options[0] == 2) {
      translate(-width/2, -height/2);
      translate(mercury.x, mercury.y);
    }
 
    // Merucury Centered
    if (options[0] == 3) {
      translate(32*cos(mercury.a)*mercury.v,
      32*sin(mercury.a)*mercury.v);
    }
 
    // The pilot is incompetent
    textSize(64);
    fill(255);
    text("YOU SUCK", width/2-160, height/2);
    textSize(32);
    text("'R' to retry, idiot", width/2-160, height/2+32);
    text("'M' to go to the menu, loser...", width/2-160, height/2+64);
 
    // Sets the correct game state
    thanksObama = 3;
 
 
    // Untranslates
    // Merucury Centered
    if (options[0] == 3) {
      translate(-32*cos(mercury.a)*mercury.v,
      -32*sin(mercury.a)*mercury.v);
    }
 
    // Merucury Centered and Static Mercury
    if (options[0] == 3 || options[0] == 2) {
      translate(-mercury.x, -mercury.y);
      translate(width/2, height/2);
    }
 
    // Stops the game
    noLoop();
  }
  // Removes 1 point from the mercury's shield if it has any
  else {
    mercury.l--;
  }
}
 
void weaponList() {
  // Displays your collection of weapons
  for (int i = 1; i < 10; i++) {
    if (bigFuckingGuns[i] == true) {
 
      // Displays the key that corresponds to the weapon
      textSize(24);
      fill(224, 255, 192);
      if (i < 8) {
        text(i, 16, 64+32*i);
      }
      else {
        switch(i) {
        case 8:
          text("V", 16, 64+32*i);
          break;
        case 9:
          text("B", 16, 64+32*i);
          break;
        }
      }
 
      // Renders the powerup next to the key
      JesusChrist temp = new JesusChrist(40, 52+32*i, -1);
      switch(i) {
      case 1:
        temp.c = -1;
        break;
      case 2:
        temp.c = -2;
        break;
      case 3:
        temp.c = -3;
        break;
      case 4:
        temp.c = -5;
        break;
      case 5:
        temp.c = -6;
        break;
      case 6:
        temp.c = -7;
        break;
      case 7:
        temp.c = -9;
        break;
      case 8:
        temp.c = -10;
        break;
      case 9:
        temp.c = -11;
        break;
      }
      temp.display();
    }
  }
}
 
// Displays your badassery level
public void ihatejavascript() {
  textSize(32);
  fill(150, 255, 255);
  text("Score:" + score, 8, 32);
}
 
// I got 100% acuraccy with the burst laser once... once
void sharpshooter() {
  textSize(16);
  fill(240, 255, 255);
  text("Accuracy:", 448, 160);
  text("Standard Laser", 448, 192);
  text("Twin Laser", 448, 224);
  text("Tri-Laser", 448, 256);
  text("Burst Laser", 448, 288);
  text("Railgun", 448, 320);
  text("Seeker Missile", 448, 352);
  text("Beam Laser", 448, 384);
   
  for (int i = 0; i < 7; i++) {
    // Renders the powerup next to the weapon
    JesusChrist temp = new JesusChrist(432, 184+32*i, -1);
    switch(i) {
    case 0:
      temp.c = -1;
      break;
    case 1:
      temp.c = -2;
      break;
    case 2:
      temp.c = -3;
      break;
    case 3:
      temp.c = -5;
      break;
    case 4:
      temp.c = -6;
      break;
    case 5:
      temp.c = -7;
      break;
    case 6:
      temp.c = -9;
      break;
    }
    temp.display();
    textSize(16);
    fill(240, 255, 255);
     
    // Displays the accuracy
    if (accuracy[i*2+1]!=0) {
      text(int(100*accuracy[i*2]/accuracy[i*2+1])+"%", 576, 192+32*i);
    }
    else {
      text("0%", 576, 192+32*i);
    }
  }
   
  // Displays the high score
  textSize(32);
  fill(150, 255, 255);
  text("High Score:" + highscore, 128, 160);
}
 
// I like rocks
class Asteroid extends Object {
 
  // The geometry of the asteroid
  float[] vertx = new float[16];
  float[] verty = new float[16];
 
  Asteroid(float x, float y, float a, float v, float w, float s, int c) {
    this.x = x;
    this.y = y;
    this.a = a;
    this.v = v;
    this.r = 0;
    this.w = w;
    this.s = s;
    // Asteroid types:
    // 1. Standard
    // 2. Seeker
    // 3. Breeder
    this.c = c;
 
    // Sets the vertices of the asteroid to an random 16 sided polygon
    for (int i = 0; i < 16; i++) {
      // The vertices are generated radially
      // A vertice is generated at an angle of 0 with a random distance from the center of the asteroid
      // Then the next vertice is PI/8 radians rotated from the previous one and generated with a new random distance from the center
      // Ask Lambert, it's a lot easier to explain in person
      vertx[i] = sin(i*PI/8)*(s + random(-s/4, s/4));
      verty[i] = cos(i*PI/8)*(s + random(-s/4, s/4));
    }
  }
 
  void show() {
    // The color of the asteroid is determined by the type
    fill(0);
    switch(c) {
    case 0:
      // White
      stroke(255);
      break;
    case 1:
      // Yellow
      stroke(32, 255, 255);
      break;
    case 2:
      // Purple
      stroke(192, 255, 255);
      break;
    case 3:
      // Blue
      stroke(128, 255, 255);
      break;
    }
 
    // Renders the asteroid
    strokeWeight(1);
    rotate(r);
    // Places the vertices of the polygon
    beginShape();
    for (int i = 0; i < 16; i++) {
      vertex(vertx[i], verty[i]);
    }
    vertex(vertx[0], verty[0]);
    endShape();
    rotate(-r);
  }
 
  void collision(Object thing, int i, int j) {
    // i is the index of rocks for this asteroid
    // j is the index of pewpew for the laser
 
    // Detects collision
    if (dist(this.x, this.y, thing.x, thing.y) < this.s+thing.s) {
      // If a laser hits an asteroid, you add 1 to self esteem
      score++;
 
      // Updates accuracy
      switch(thing.c) {
      case -1:
        accuracy[0]++;
        break;
      case -2:
        accuracy[2]++;
        break;
      case -3:
        accuracy[4]++;
        break;
      case -5:
        accuracy[6]++;
        break;
      case -6:
        accuracy[8]++;
        break;
      case -1001:
        accuracy[10]++;
        break;
      case -9:
        accuracy[12]++;
        break;
      }
 
      // If the laser isn't a railgun, delete the laser
      if (thing.c != -6) {
        pewpew.remove(j);
      }
 
      // Random chance to drop a powerup when destroyed
      // Chance is 3/64
      if (random(0, 64) < 3) {
        helpme.add(new JesusChrist(x, y, ceil(random(-12, -2))));
      }
 
      // Adds new asteroids if size is above a threshold
      if (s>7) {
 
        // Adds different asteroids for different asteroid types
        switch(c) {
        case 0:
          // For regular asteroids, divides size by 2 and multiplies velocity by 1.6
          rocks.add(new Asteroid(x, y, a-PI/8,
          v*1.6, w*2,
          s/2, c));
          rocks.add(new Asteroid(x, y, a+PI/8,
          v*1.6, w*2,
          s/2, c));
          break;
        case 1:
          // For seeker asteroids, divides size by 2 and multiplies velocity by 1.3
          rocks.add(new Asteroid(x, y, a-PI/4,
          v*1.3, w*2,
          s/2, c));
          rocks.add(new Asteroid(x, y, a+PI/4,
          v*1.3, w*2,
          s/2, c));
          break;
        case 2:
          // For breeker asteroids, divides size by 3
          // Sends out 4 asteroids at right angles with a random velocity and size divided by 2
          rocks.add(new Asteroid(x, y, w,
          random(1.2, 1.8), w*2,
          s/2, 0));
          rocks.add(new Asteroid(x, y, w+PI/2,
          random(1.2, 1.8), w*2,
          s/2, 0));
          rocks.add(new Asteroid(x, y, w+PI,
          random(1.2, 1.8), w*2,
          s/2, 0));
          rocks.add(new Asteroid(x, y, w-PI/2,
          random(1.2, 1.8), w*2,
          s/2, 0));
          rocks.add(new Asteroid(x, y, 0, 0,
          w*2,
          s/3, c));
          break;
        }
      }
 
      // Adds more asteroids based on the score
      if (score%12 == 0) {
        // Every 12 points, adds a regular asteroid
        // 32 size and random velocity
        rocks.add(new Asteroid(
        mercury.x+random(128, width-128),
        mercury.y+random(128, height-128),
        random(-PI, PI), random(0.4, 0.8),
        random(-PI/100, PI/100), 32, 0));
      }
 
      if (score%20 == 0) {
        // Every 20 points, adds a seeker asteroid
        // 24 size and random velocity
        rocks.add(new Asteroid(mercury.x+random(128, width-128),
        mercury.y+random(128, height-128),
        random(-PI, PI), random(0.6, 0.8),
        random(-PI/100, PI/100), 24, 1));
      }
 
      if (score%42 == 0) {
        // Every 42 points, adds a breeder asteroid
        // 48 size and zero velocity
        rocks.add(new Asteroid(
        mercury.x+random(128, width-128),
        mercury.y+random(128, height-128),
        0, 0, random(-PI/50, PI/50), 48, 2));
      }
 
      // Removes the rock that was hit by a laser
      rocks.remove(i);
    }
  }
}
 
// May our lord savior grant us the goddamn burst laser we need to get 500 points
class JesusChrist extends Object {
  JesusChrist(float x, float y, int c) {
    this.x = x;
    this.y = y;
    // Random trajectory
    this.a = random(0, 2*PI);
    // Velocity of 1
    this.v = 1;
    this.r = 0;
    this.w = 0;
    this.s = 8;
    // Powerup types:
    // -1. Basic Laser
    // -2. Twin Laser
    // -3. Tri Laser
    // -4. Explosion
    // -5. Burst Laser
    // -6. Railgun
    // -7. Seeker Missiles
    // -8. Shield
    // -9. Beam Laser
    // -10. Seeker Missile Barrage
    // -11. Rapid Fire
    this.c = c;
  }
 
  // Renders the powerup
  void show() {
    fill(0);
    stroke(160, 255, 255);
    strokeWeight(1);
    ellipse(0, 0, 16, 16);
    switch(c) {
    case -1:
      strokeWeight(2);
      stroke(0, 255, 255);
      line(0, 8, 0, -8);
      break;
    case -2:
      strokeWeight(2);
      stroke(32, 255, 255);
      line(0, 6, -6, -6);
      line(0, 6, 6, -6);
      break;
    case -3:
      strokeWeight(2);
      stroke(64, 255, 255);
      line(0, 6, -6, -6);
      line(0, 6, 6, -6);
      line(0, 6, 0, -6);
      break;
    case -4:
      stroke(128, 255, 255);
      line(0, 6, 0, -6);
      line(6, 0, -6, 0);
      line(-6, 6, 6, -6);
      line(-6, -6, 6, 6);
      break;
    case -5:
      strokeWeight(2);
      stroke(96, 255, 255);
      line(0, 6, -6, -6);
      line(0, 6, -3, -6);
      line(0, 6, 0, -6);
      line(0, 6, 3, -6);
      line(0, 6, 6, -6);
      break;
    case -6:
      strokeWeight(2);
      stroke(0, 0, 255);
      line(0, 8, 0, -8);
      break;
    case -7:
      strokeWeight(2);
      stroke(160, 255, 255);
      ellipse(0, 0, 2, 2);
      line(4, 0, 8, 0);
      line(-4, 0, -8, 0);
      line(0, 4, 0, 8);
      line(0, -4, 0, -8);
      break;
    case -8:
      stroke(150, 255, 255);
      ellipse(0, 0, 12, 12);
      stroke(140, 255, 255);
      ellipse(0, 0, 8, 8);
      break;
    case -9:
      stroke(192, 255, 255);
      line(2, 8, 2, -8);
      line(0, 8, 0, -8);
      line(-2, 8, -2, -8);
      break;
    case -10:
      stroke(180, 255, 255);
      line(4, 0, 10, 0);
      line(-4, 0, -10, 0);
      line(0, 4, 0, 10);
      line(0, -4, 0, -10);
      line(3, 3, 7, 7);
      line(3, -3, 7, -7);
      line(-3, 3, -7, 7);
      line(-3, -3, -7, -7);
      if (mercury.q != 0) {
        strokeWeight(2);
        stroke(0, 255, 255);
        line(10, 10, -10, -10);
        line(-10, 10, 10, -10);
      }
      break;
    case -11:
      stroke(0, 255, 255);
      line(0, 8, 0, 4);
      line(0, 8, 3, 3);
      line(0, 8, -3, 3);
      line(0, 2, 0, -2);
      line(0, 2, 3, -1);
      line(0, 2, -3, -1);
      line(0, -4, 0, -8);
      line(0, -4, 3, -7);
      line(0, -4, -3, -7);
      if (mercury.q != 0) {
        strokeWeight(2);
        stroke(0, 255, 255);
        line(10, 10, -10, -10);
        line(-10, 10, 10, -10);
      }
      break;
    }
  }
 
  void collision(SpaceShip thing, int i) {
    // Detects collision between powerup and the Mercury
    // i is the index of the powerup in helpme
    if (dist(this.x, this.y, thing.x, thing.y) < this.s+thing.s) {
      helpme.remove(i);
 
      // Enables the weapon of the corresponding powerup
      switch(c) {
      case -2:
        bigFuckingGuns[2] = true;
        break;
      case -3:
        bigFuckingGuns[3] = true;
        break;
      case -4:
        for (i = 0; i < 64; i++) {
          pewpew.add(new Laser(thing.x, thing.y, PI/2+i*PI/32, 24, i*PI/32, -4, 30));
        }
        break;
      case -5:
        bigFuckingGuns[4] = true;
        break;
      case -6:
        bigFuckingGuns[5] = true;
        break;
      case -7:
        bigFuckingGuns[6] = true;
        break;
      case -8:
        thing.l = 2;
        break;
      case -9:
        bigFuckingGuns[7] = true;
        break;
      case -10:
        bigFuckingGuns[8] = true;
        break;
      case -11:
        bigFuckingGuns[9] = true;
        break;
      }
    }
  }
}
 
// Dakka Dakka Dakka
class Laser extends Object {
  // f is the lifespan of the laser
  // e is the total runtime of the laser
  int e, f;
 
  Laser(float x, float y, float a, float v, float r, int c, int f) {
    this.x = x;
    this.y = y;
    this.a = a;
    this.v = v;
    this.r = r;
    this.w = 0;
    // The size of all lasers is 4
    this.s = 4;
    // Laser types:
    // -1. Basic Laser
    // -2. Twin Laser
    // -3. Tri Laser]
    // -5. Burst Laser
    // -6. Railgun
    // -9. Beam Laser
    // -1001. Seeker Missiles
    this.c = c;
    this.e = 0;
    this.f = f;
  }
 
  // Renders the laser
  void show() {
    switch(c) {
    case -1:
      stroke(0, 255, 255, 255 - e*3);
      break;
    case -2:
      stroke(32, 255, 255, 255 - e*6);
      break;
    case -3:
      stroke(64, 255, 255, 255 - e*3);
      break;
    case -4:
      stroke(128, 255, 255, 255 - e*3);
      break;
    case -5:
      stroke(96, 255, 255, 255 - e*12);
      break;
    case -6:
      stroke(0, 0, 255, 255 - e*2);
      break;
    case -1001:
      stroke(160, 255, 255, 255 - e*4);
      break;
    case -9:
      stroke(192, 255, 255, 255);
      break;
    }
    strokeWeight(3);
    rotate(r);
    beginShape();
    vertex(0, 4);
    vertex(0, -4);
    endShape();
    rotate(-r);
  }
 
  // When the laser runtime equals the laser lifespan, the laser dissipates into space
  void dissipate(int j) {
    // j is the index of this laser in pewpew
    e++;
    if (e == f) pewpew.remove(j);
  }
}
 
// There's a lot of empty space up here
abstract class Object {
 
  // x and y are the positions of the object
  // a is the angle of the velocity
  // v is the velocity of the object
  // r is the rotation of the object
  //   r and a are different because an object may be
  //   facing in a different direction from where it's moving
  // w is the rotation rate of the object
  // s is the size of the object
  float x, y, a, v, r, w, s;
  // c is the type of the object
  int c;
 
  // The gears and machinery that runs the entire program
  // Tick controls movement and rotation of objects
  void tick() {
    // If an object goes offscreen, it appears on the other side
 
    // Static Space
    if (options[0] == 1) {
      if (x<0) {
        x += width;
        y = height-y;
      }
      if (x>width) {
        x -= width;
        y = height-y;
      }
      if (y<0) {
        y += height;
        x = width-x;
      }
      if (y>height) {
        y -= height;
        x = width-x;
      }
    }
 
    // Mercury centered and static Mercury
    if (options[0] == 3 || options[0] == 2) {
      if (x<0) {
        x += width;
      }
      if (x>width) {
        x -= width;
      }
      if (y<0) {
        y += height;
      }
      if (y>height) {
        y -= height;
      }
    }
 
    // Changes the position and rotation of the ship
    r += w;
    // Newtonian integration works here
    x += cos(a)*v;
    y += sin(a)*v;
 
    // Does the tracking
    switch(c) {
      // Seeker Asteroid tracking
    case 1:
      if (abs(atan2(mercury.y-y, mercury.x-x) - a) < PI) {
        if (atan2(mercury.y-y, mercury.x-x) > a) {
          a += 0.04;
        }
        else {
          a -= 0.04;
        }
      }
      else {
        if (atan2(mercury.y-y, mercury.x-x) > a) {
          a -= 0.04;
        }
        else {
          a += 0.04;
        }
      }
      if (a < -PI) {
        a = PI+a;
      }
      if (a > PI) {
        a = -PI+a;
      }
      // Has weird edge cases and shit. I hate this code...
      break;
 
      // Seeker Missile tracking
    case -1001:
      r = a - PI/2;
      Asteroid track = rocks.get(0);
      for (int i = 1; i < rocks.size(); i++) {
        // Sets the tracked asteroid to the asteroid closest to the laser
        Asteroid rock = rocks.get(i);
        if (dist(x, y, rock.x, rock.y) < dist(x, y, track.x, track.y)) {
          track = rock;
        }
      }
      // Much simpler than Seeker Asteroid
      a = atan2(track.y-y, track.x-x);
      break;
    }
  }
 
  // All objects have a show method
  void show() {
  }
 
  // Displays the asteroid on the screen
  void display() {
    translate(x, y);
    show();
    translate(-x, -y);
 
    // Static Mercury and Mercury Centered
    if (options[0] == 3 || options[0] == 2) {
      translate(x+width, y);
      show();
      translate(-x-width, -y);
 
      translate(x-width, y);
      show();
      translate(-x+width, -y);
 
      translate(x, y+height);
      show();
      translate(-x, -y-height);
 
      translate(x, y-height);
      show();
      translate(-x, -y+height);
 
      translate(x+width, y+height);
      show();
      translate(-x-width, -y-height);
 
      translate(x-width, y-height);
      show();
      translate(-x+width, -y+height);
 
      translate(x+width, y-height);
      show();
      translate(-x-width, -y+height);
 
      translate(x-width, y+height);
      show();
      translate(-x+width, -y-height);
    }
 
    // Static Space
    if (options[0] == 1) {
      translate(x+width, height-y);
      show();
      translate(-x-width, y-height);
 
      translate(x-width, height-y);
      show();
      translate(-x+width, y-height);
 
      translate(width-x, y+height);
      show();
      translate(x-width, -y-height);
 
      translate(width-x, y-height);
      show();
      translate(x-width, -y+height);
    }
  }
}
 
// You must be at least 17 to acquire a space ship license
class SpaceShip extends Object {
 
  // d is weapon cooldown
  // f is weapon fire rate
  // l is shield points
  // p is special weapon type
  // q is special cooldown
  // t is weapon firerate final
  int d, f, l, p, q, t;
 
  SpaceShip() {
    this.launch();
  }
 
  // Renders the spaceship and its shield
  void show() {
    fill(0);
    stroke(75, 255, 255);
    strokeWeight(1);
    rotate(r);
    beginShape();
    vertex(0, 16);
    vertex(8, -16);
    vertex(-8, -16);
    vertex(0, 16);
    endShape();
    stroke(150, 255, 255, l*64);
    fill(150, 255, 255, l*24);
    ellipse(0, 0, 48, 48);
    rotate(-r);
  }
 
  // Ready for takeoff
  void launch() {
    loop();
    this.x = width/2;
    this.y = height/2;
    this.a = 0;
    this.v = 0;
    this.r = 0;
    this.w = 0;
    this.s = 12;
    // The space ship classes are the same as the powerup classes
    this.c = -1;
    this.f = 6;
    this.l = 1;
    this.p = 0;
    this.q = 0;
    this.t = 6;
 
    // Clears space
    rocks.clear();
    pewpew.clear();
    helpme.clear();
    score = 0;
 
    // Clears Controls
    keyup = false;
    keyright = false;
    keyleft = false;
    keyshoot = false;
 
    // Generates 4 starting asteroids
    rocks.add(new Asteroid(random(0, width), 0, random(-PI, PI), random(0.4, 0.8), random(-PI/100, PI/100), 32, 0));
    rocks.add(new Asteroid(random(0, width), height, random(-PI, PI), random(0.4, 0.8), random(-PI/100, PI/100), 32, 0));
    rocks.add(new Asteroid(0, random(0, height), random(-PI, PI), random(0.4, 0.8), random(-PI/100, PI/100), 32, 0));
    rocks.add(new Asteroid(width, random(0, height), random(-PI, PI), random(0.4, 0.8), random(-PI/100, PI/100), 32, 0));
 
    // Resets weapons
    for (int i = 0; i < 10; i++) {
      bigFuckingGuns[i] = false;
    }
 
    // Enables the starting weapon
    bigFuckingGuns[1] = true;
  }
 
  // Accelerates the ship in the direction it's facing
  void accelerate() {
    float xt = cos(a)*v+cos(r+PI/2)*.2;
    float yt = sin(a)*v+sin(r+PI/2)*.2;
    v=sqrt(sq(yt)+sq(xt));
    a=atan2(yt, xt);
  }
 
  // Exacts freedom on the asteroids
  void shoot() {
    if (d == 0) {
      switch(c) {
      case -1:
        accuracy[1]++;
        pewpew.add(new Laser(x, y, r+PI/2+random(-PI/32, PI/32), 10, r+random(-PI/32, PI/32), -1, 60));
        break;
      case -2:
        accuracy[3]+=2;
        pewpew.add(new Laser(x, y, r+PI/2-PI/16+random(-PI/16, PI/16), 12, r-PI/16+random(-PI/16, PI/16), -2, 30));
        pewpew.add(new Laser(x, y, r+PI/2+PI/16+random(-PI/16, PI/16), 12, r+PI/16+random(-PI/16, PI/16), -2, 30));
        break;
      case -3:
        accuracy[5]+=3;
        pewpew.add(new Laser(x, y, r+PI/2-PI/32, 18, r-PI/32, -3, 60));
        pewpew.add(new Laser(x, y, r+PI/2, 18, r, -3, 60));
        pewpew.add(new Laser(x, y, r+PI/2+PI/32, 18, r+PI/32, -3, 60));
        break;
      case -5:
        accuracy[7]+=4;
        pewpew.add(new Laser(x, y, r+PI/2-PI/8, 8, r-PI/8, -5, 10));
        pewpew.add(new Laser(x, y, r+PI/2-PI/16, 8, r-PI/16, -5, 10));
        pewpew.add(new Laser(x, y, r+PI/2, 8, r, -5, 10));
        pewpew.add(new Laser(x, y, r+PI/2+PI/16, 8, r+PI/16, -5, 10));
        pewpew.add(new Laser(x, y, r+PI/2+PI/8, 8, r+PI/8, -5, 10));
        break;
      case -6:
        accuracy[9]++;
        pewpew.add(new Laser(x, y, r+PI/2, 24, r, -6, 90));
        break;
      case -7:
        accuracy[11]++;
        pewpew.add(new Laser(x, y, r+PI/2, 16, r, -1001, 45));
        break;
      case -9:
        accuracy[13]++;
        for (int i = 1; i < 16; i++) {
          pewpew.add(new Laser(x+cos(r+PI/2)*16*i, y+sin(r+PI/2)*16*i, r+PI/2+PI/16, 16, r, -9, 3));
          pewpew.add(new Laser(x+cos(r+PI/2)*16*i, y+sin(r+PI/2)*16*i, r+PI/2-PI/16, 16, r, -9, 3));
        }
        break;
      }
      // Sets the cooldown to the fire rate
      d = f;
    }
  }
 
  // Activates the special weapon
  void wreckShit() {
    if (q == 0) {
      switch(p) {
      case 1:
        for (int i = 0; i < 16; i++) {
          accuracy[11]+=16;
          pewpew.add(new Laser(x+64*cos(i*PI/8), y+64*sin(i*PI/8), 0, 8, 0, -1001, 45));
        }
        break;
      case 2:
        f = 3;
        break;
      }
 
      // Sets the special cooldown to 3 seconds
      q = 90;
    }
  }
}
 
// I keep clicking but nothing happens!
class Button {
  // Position and hue
  float x, y, h;
  // Text
  String t;
  // Button index
  int c;
   
  Button(float x, float y, float h, String t, int c) {
    this.x = x;
    this.y = y;
    this.h = h;
    this.t = t;
    this.c = c;
  }
   
  // Renders the button
  void display() {
    textSize(40);
    if (selection == c) {
      stroke(h, 192, 192);
      fill(h, 128, 255);
    }
    else {
      stroke(h, 192, 96);
      fill(h, 128, 128);
    }
    strokeWeight(8);
    ellipse(x, y, 64, 64);
    fill((h+32)%256, 255, 255);
    text(t, x-16, y+16);
  }
}
 
// Yeahhhhh, I don't quite understand the code either
void playGame() {
  // Makes the grid
  strokeWeight(1);
  stroke(75, 64, 128);
 
  // Static space
  if (options[0] == 1) {
    for (int i = 0; i < height/32+1; i++) {
      line(-32, i*32, width+32, i*32);
    }
    for (int i = 0; i < width/32+1; i++) {
      line(i*32, -32, i*32, height+32);
    }
  }
 
  // Static Mercury
  if (options[0] == 2) {
    for (int i = 0; i < height/32+1; i++) {
      line(-32, i*32-(mercury.y%32), width+32, i*32-(mercury.y%32));
    }
    for (int i = 0; i < width/32+1; i++) {
      line(i*32-(mercury.x%32), -32, i*32-(mercury.x%32), height+32);
    }
  }
 
  // Mercury Centered
  // Mercury.x/y%32 translates the grid as the Mercury moves
  if (options[0] == 3) {
    for (int i = -8; i < height/32+8; i++) {
      line(-32, i*32-(mercury.y%32)-32*sin(mercury.a)*mercury.v,
      width+32, i*32-(mercury.y%32)-32*sin(mercury.a)*mercury.v);
    }
    for (int i = -8; i < width/32+8; i++) {
      line(i*32-(mercury.x%32)-32*cos(mercury.a)*mercury.v, -32,
      i*32-(mercury.x%32)-32*cos(mercury.a)*mercury.v, height+32);
    }
  }
 
  // Static Space
  if (options[0] == 1) {
    mercury.display();
    mercury.tick();
  }
 
  // Static Mercury
  if (options[0] == 2) {
    translate(width/2, height/2);
    mercury.show();
  }
 
  // Mercury Centered
  if (options[0] == 3) {
    translate(width/2-32*cos(mercury.a)*mercury.v,
    height/2-32*sin(mercury.a)*mercury.v);
    translate(-mercury.x, -mercury.y);
    mercury.display();
    translate(mercury.x, mercury.y);
  }
 
 
  // The Mercury, a result of the human instinct to set no limits
  mercury.tick();
 
  // Static Mercury and Mercury Centered
  if (options[0] == 3 || options[0] == 2) {
    translate(-mercury.x, -mercury.y);
  }
 
 
  for (int j = 0; j < pewpew.size(); j++) {
    // This is my laser. There are many like it, but this one is mine
    Laser pew = pewpew.get(j);
    pew.dissipate(j);
    pew.display();
    pew.tick();
  }
 
  for (int i = 0; i < helpme.size(); i++) {
    // Jesus, take the control surfaces
    JesusChrist omfg = helpme.get(i);
    omfg.display();
    omfg.tick();
    // Detects when the mercury picks up powerups
    omfg.collision(mercury, i);
  }
 
  for (int i = 0; i < rocks.size(); i++) {
 
    // We're all just objects in space
    Asteroid rock = rocks.get(i);
    rock.display();
    rock.tick();
 
    for (int j = 0; j < pewpew.size(); j++) {
      // It's your fault I keep missing
      Laser pew = pewpew.get(j);
      // Detects collision between rocks and lasers
      rock.collision(pew, i, j);
    }
  }
 
  for (int i = 0; i < rocks.size(); i++) {
    Asteroid rock = rocks.get(i);
    // Detects collision between the Mercury and Rocks
    if (dist(rock.x, rock.y, mercury.x, mercury.y) < rock.s+mercury.s) {
      // UI_game
      aaaaaaaaaugh(i);
    }
  }
 
  // Merucury Centered and Static Mercury
  if (options[0] == 3 || options[0] == 2) {
    translate(mercury.x, mercury.y);
    translate(-width/2, -height/2);
  }
 
  // Mercury Centerd
  if (options[0] == 3) {
 
    translate(+32*cos(mercury.a)*mercury.v,
    +32*sin(mercury.a)*mercury.v);
  }
 
  // UI_game
  ihatejavascript();
  weaponList();
 
  // Mercury Weapon Cooldowns
  if (mercury.d > 0) {
    mercury.d--;
  }
  if (mercury.q > 0) {
    mercury.q--;
  }
  if (mercury.q < 60) {
    mercury.f = mercury.t;
  }
 
  // Control Surfaces
  if (keyup == true) mercury.accelerate();
  if (keyleft == true) mercury.r -= 0.1;
  if (keyright == true) mercury.r += 0.1;
  if (keyshoot == true) mercury.shoot();
 
  // Space isn't a perfect vacuum
  mercury.v*=0.96;
}
 
// Is this a game? Does this look like a game to you?
// Oh wait, it is...
void gameKeyPressed() {
  // Toggles movement and shooting
  if (key == CODED) {
    if (keyCode == UP) keyup = true;
    if (keyCode == LEFT) keyleft = true;
    if (keyCode == RIGHT) keyright = true;
  }
  if (key == ' ') keyshoot = true;
 
  // Restarts the game
  if (key == 'r') {
    mercury.launch();
    thanksObama = 1;
    loop();
  }
 
  // Pauses and unpauses the game
  if (key == 'p') {
    if (thanksObama == 1) {
      textSize(64);
      fill(255);
      text("QQ MOAR", width/2-160, height/2-64);
      textSize(32);
      text("'P' to man the f*ck up", width/2-160, height/2-32);
      text("'M' to be a wimp", width/2-160, height/2);
      thanksObama = 2;
      noLoop();
    }
    else {
      if (thanksObama == 2) {
        thanksObama = 1;
        loop();
      }
    }
  }
 
  if (key == 'm') {
    if (thanksObama == 2 || thanksObama == 3) {
      gotoMenu();
    }
  }
 
  // Debug control, enables all weapons
  if (key == 'e') {
    for (int i = 0; i < 10; i++) {
      //bigFuckingGuns[i] = true;
    }
  }
 
  // Weapon switching
  switch(key) {
  case '1':
    if (bigFuckingGuns[1] == true) {
      mercury.c = -1;
      mercury.f = 6;
      mercury.t = 6;
    }
    break;
  case '2':
    if (bigFuckingGuns[2] == true) {
      mercury.c = -2;
      mercury.f = 4;
      mercury.t = 4;
    }
    break;
  case '3':
    if (bigFuckingGuns[3] == true) {
      mercury.c = -3;
      mercury.f = 8;
      mercury.t = 8;
    }
    break;
  case '4':
    if (bigFuckingGuns[4] == true) {
      mercury.c = -5;
      mercury.f = 5;
      mercury.t = 5;
    }
    break;
  case '5':
    if (bigFuckingGuns[5] == true) {
      mercury.c = -6;
      mercury.f = 18;
      mercury.t = 18;
    }
    break;
  case '6':
    if (bigFuckingGuns[6] == true) {
      mercury.c = -7;
      mercury.f = 10;
      mercury.t = 10;
    }
    break;
  case '7':
    if (bigFuckingGuns[7] == true) {
      mercury.c = -9;
      mercury.f = 8;
      mercury.t = 8;
    }
    break;
  case 'v':
    if (bigFuckingGuns[8] == true) {
      mercury.p = 1;
      mercury.wreckShit();
    }
    break;
  case 'b':
    if (bigFuckingGuns[9] == true) {
      mercury.p = 2;
      mercury.wreckShit();
    }
    break;
  }
}
 
void gameKeyReleased() {
  // Untoggles movement and shooting
  if (key == CODED) {
    if (keyCode == UP) keyup = false;
    if (keyCode == LEFT) keyleft = false;
    if (keyCode == RIGHT) keyright = false;
  }
  if (key == ' ') keyshoot = false;
}
 
// This was a good spenditure of time
void menu() {
 
  for (int i = 0; i < rocks.size(); i++) {
    Asteroid rock = rocks.get(i);
    rock.display();
    rock.tick();
  }
 
  start.display();
  stat.display();
  option.display();
 
  fill(75, 128, 255);
  textSize(96);
  text("ASTEROIDS", 124, 124);
 
  fill(75, 255, 128);
  text("ASTEROIDS", 132, 132);
 
  fill(75, 255, 255);
  text("ASTEROIDS", 128, 128);
  textSize(16);
  text("Created by Lambert Wang", 444, 148);
 
  fill(120, 255, 255);
  text("Instructions", 448, 192);
  text("Up and Down to navigate menu", 448, 224);
  text("Spacebar to select menu options", 448, 256);
  text("In game:", 448, 288);
  text("Up to Accelerate", 448, 320);
  text("Left and right to turn", 448, 352);
  text("Spacebar to shoot", 448, 384);
  text("1 - 7 to select weapons", 448, 416);
  text("V and B to use special weapons", 448, 448);
  text("P to pause", 448, 480);
}
 
void wtfjavascript() {
  for (int i = 0; i < rocks.size(); i++) {
    Asteroid rock = rocks.get(i);
    rock.display();
    rock.tick();
  }
 
  back.display();
  mode.display();
  showframes.display();
  textSize(40);
  text(":" + options[0], width/4+280, height/2+48);
  text(":" + options[1], width/4+300, height/2-16);
 
  fill(150, 128, 255);
  textSize(96);
  text("OPTIONS", 124, 124);
 
  fill(150, 255, 128);
  text("OPTIONS", 132, 132);
 
  fill(150, 255, 255);
  text("OPTIONS", 128, 128);
}
 
void omfgwhydoesntthiswork() {
  for (int i = 0; i < rocks.size(); i++) {
    Asteroid rock = rocks.get(i);
    rock.display();
    rock.tick();
  }
 
  back.display();
 
  fill(225, 128, 255);
  textSize(96);
  text("STATISTICS", 124, 124);
 
  fill(225, 255, 128);
  text("STATISTICS", 132, 132);
 
  fill(225, 255, 255);
  text("STATISTICS", 128, 128);
 
  sharpshooter();
}
 
void gotoMenu() {
  loop();
  thanksObama = 4;
  rocks.clear();
  selection = 1;
 
  for (int i = 0; i < 16; i++) {
    rocks.add(new Asteroid(random(0, width), random(0, height), random(-PI, PI), random(0.6, 1.2), random(-PI/100, PI/100), random(12, 32), 0));
  }
}
 
void gotoOption() {
  loop();
  thanksObama = 5;
  rocks.clear();
  selection = 4;
 
  for (int i = 0; i < 16; i++) {
    rocks.add(new Asteroid(random(0, width), random(0, height), random(-PI, PI), random(0.6, 1.2), random(-PI/100, PI/100), random(12, 32), 3));
  }
}
 
void gotoStat() {
  loop();
  thanksObama = 6;
  rocks.clear();
  selection = 4;
 
  for (int i = 0; i < 16; i++) {
    rocks.add(new Asteroid(random(0, width), random(0, height), random(-PI, PI), random(0.6, 1.2), random(-PI/100, PI/100), random(12, 32), 2));
  }
}
 
void menuKeyPressed() {
  if (key == CODED) {
    if (keyCode == UP) {
      switch(thanksObama) {
      case 4:
        switch(selection) {
        case 1:
          selection = 3;
          break;
        case 2:
          selection = 1;
          break;
        case 3:
          selection = 2;
          break;
        }
        break;
      case 5:
        switch(selection) {
        case 4:
          selection = 5;
          break;
        case 5:
          selection = 6;
          break;
        case 6:
          selection = 4;
          break;
        }
        break;
      case 6:
        switch(selection) {
        case 4:
          break;
        }
        break;
      }
    }
  }
  if (keyCode == DOWN) {
    switch(thanksObama) {
    case 4:
      switch(selection) {
      case 1:
        selection = 2;
        break;
      case 2:
        selection = 3;
        break;
      case 3:
        selection = 1;
        break;
      }
      break;
    case 5:
      switch(selection) {
      case 4:
        selection = 6;
        break;
      case 5:
        selection = 4;
        break;
      case 6:
        selection = 5;
        break;
      }
      break;
    case 6:
      switch(selection) {
      case 4:
        break;
      }
      break;
    }
  }
 
  if (key == ' ') {
    switch(thanksObama) {
    case 4:
      switch(selection) {
      case 1:
        mercury.launch();
        thanksObama = 1;
        break;
      case 2:
        gotoOption();
        break;
      case 3:
        gotoStat();
        break;
      }
      break;
    case 5:
      switch(selection) {
      case 4:
        gotoMenu();
        break;
      case 5:
        switch(options[0]) {
        case 1:
          options[0] = 2;
          break;
        case 2:
          options[0] = 3;
          break;
        case 3:
          options[0] = 1;
          break;
        }
        break;
      case 6:
        switch(options[1]) {
        case 1:
          options[1] = 2;
          break;
        case 2:
          options[1] = 1;
          break;
        }
        break;
      }
      break;
    case 6:
      switch(selection) {
      case 4:
        gotoMenu();
        break;
      }
      break;
    }
  }
}
