import processing.serial.*;

Serial myPort;        // The serial port
ArrayList<int[]> vals = new ArrayList<int[]>();
int window = 32;

// Array of 10 classes, each of which is an array of samples, each of which is an object containing:
// "mean": an array of means (one for each analog input)
// "stddev": an array of std. dev. (one for each analog input)
JSONArray classes = new JSONArray();
boolean saveClass = false;
int classNum;

int minClass = -1;

void setup () {
  for (int i = 0; i < 10; i++) {
    classes.append(new JSONArray());
  }

  // set the window size:
  size(800, 600);

  // List all the available serial ports
  // if using Processing 2.1 or later, use Serial.printArray()
  println(Serial.list());

  // I know that the first port in the serial list on my mac
  // is always my  Arduino, so I open Serial.list()[0].
  // Open whatever port is the one you're using.
  myPort = new Serial(this, Serial.list()[1], 115200);

  // don't generate a serialEvent() unless you get a newline character:
  myPort.bufferUntil('\n');

  // set inital background:
  background(0);
  colorMode(HSB);
}

// TODO: need to allow retaining data that scrolls of the edge of the screen
synchronized void draw () {
  background(0);
  
  // Draw alternating-colored rectanges to show windows.
  noStroke();
  fill(32);
  for (int x = width; x >= 0; x -= window * 2) {
    rect(x - window, 0, window, height);
  }

  // Divide into three horizontal bands: raw values, mean, and std. dev.
  stroke(255);
  line(0, height / 3, width, height / 3);
  line(0, 2 * height / 3, width, 2 * height / 3);

  // Label each band.
  fill(255);
  text("values", 0, 20);
  text("mean", 0, height / 3 + 20);
  text("stddev", 0, 2 * height / 3 + 20);
  
  // Leave room at right to show stored values in each of 10 classes.
  line(width - window * 10, 0, width - window * 10, height);
  translate(-window * 10, 0);

  for (int i = vals.size() - 1; i > 0 && i > vals.size() - width; i--) {
    for (int j = 0; j < vals.get(i - 1).length && j < vals.get(i).length; j++) {
      float val1 = vals.get(i - 1)[j], val2 = vals.get(i)[j];
      val1 = map(val1, 0, 1023, 0, height / 3);
      val2 = map(val2, 0, 1023, 0, height / 3);

      // draw the line:
      stroke(255 / vals.get(i).length * j, 255, 255);
      line(width - vals.size() + i - 1, height / 3 - val1, width - vals.size() + i, height / 3 - val2);
    }
  }

  if (vals.size() > 0) {
    int index = vals.size() - 1;
    boolean firstWindow = true;

    while (index > 0 && index > vals.size() - width) {
      JSONObject sample = new JSONObject();
      
      ArrayList<Integer> totals = new ArrayList<Integer>();
      ArrayList<Integer> counts = new ArrayList<Integer>();
      int endIndex = index;
      
      // Calculate mean for the current time window.

      while (index >= 0 && index >= endIndex - window) {
        for (int j = 0; j < vals.get(index).length; j++) {
          if (totals.size() < j + 1) {
            totals.add(vals.get(index)[j]);
            counts.add(1);
          } else {
            totals.set(j, totals.get(j) + vals.get(index)[j]);
            counts.set(j, counts.get(j) + 1);
          }
        }
        index--;
      }

      ArrayList<Float> mean = new ArrayList<Float>();
      for (int j = 0; j < counts.size(); j++) {
        mean.add(float(totals.get(j)) / counts.get(j));
      }
      
      // Save mean as an example of the indicated class.
      // (Note that this will only happen for the first / most-recent / rightmost time window.)

      if (saveClass) {
        JSONArray means = new JSONArray();
        for (int j = 0; j < mean.size(); j++) {
          means.append(mean.get(j));
        }
        sample.setJSONArray("mean", means);
      }
      
      // Draw mean.

      for (int j = 0; j < totals.size(); j++) {
        float m = mean.get(j);
        m = map(m, 0, 1023, 0, height / 3);
        stroke(255 / totals.size() * j, 255, 255);
        line(width - vals.size() + endIndex, 2 * height / 3 - m, 
          width - vals.size() + endIndex - window, 2 * height / 3 - m); 
        fill(255 / totals.size() * j, 255, 255);
        //text("" + (totals.get(j) / counts.get(j)), width - (millis.get(millis.size() - 1) - startMillis), (j + 2) * 20);
      }
      
      // Calculate standard deviation for the current time window.

      index = endIndex;
      ArrayList<Float> stddev = new ArrayList<Float>();
      while (index >= 0 && index >= endIndex - window) {
        for (int j = 0; j < vals.get(index).length; j++) {
          if (stddev.size() < j + 1) stddev.add(float(0));
          stddev.set(j, stddev.get(j) + (vals.get(index)[j] - mean.get(j)) * (vals.get(index)[j] - mean.get(j)));
        }
        index--;
      }

      for (int j = 0; j < stddev.size(); j++) {
        stddev.set(j, sqrt(stddev.get(j) / counts.get(j)));
      }

      // Save std. dev. as an example of the indicated class.
      // (Note that this will only happen for the first / most-recent / rightmost time window.)

      if (saveClass) {
        JSONArray stddevs = new JSONArray();
        for (int j = 0; j < stddev.size(); j++) {
          stddevs.append(stddev.get(j));
        }
        sample.setJSONArray("stddev", stddevs);
      }
      
      // Draw std. dev.

      for (int j = 0; j < stddev.size(); j++) {
        float s = stddev.get(j);
        s = map(s, 0, 1023, 0, height / 3);
        stroke(255 / totals.size() * j, 255, 255);
        line(width - vals.size() + endIndex, height - s, 
          width - vals.size() + endIndex - window, height - s); 
        fill(255 / totals.size() * j, 255, 255);
        //text("" + (totals.get(j) / counts.get(j)), width - (millis.get(millis.size() - 1) - startMillis), (j + 2) * 20);
      }
      
      // Calculate class closest to the current time window.
      // (Note that this will only happen for the first / most-recent / rightmost time window.)
      
      if (firstWindow) {
        float minDistance = 9999999;
        for (int c = 0; c < classes.size(); c++) {
          for (int i = 0; i < classes.getJSONArray(c).size(); i++) {
            JSONObject s = classes.getJSONArray(c).getJSONObject(i);
            float distance = 0;
            for (int j = 0; j < s.getJSONArray("mean").size() && j < mean.size(); j++) {
              distance += (mean.get(j) - s.getJSONArray("mean").getFloat(j)) *
                          (mean.get(j) - s.getJSONArray("mean").getFloat(j));
            }
            for (int j = 0; j < s.getJSONArray("stddev").size() && j < stddev.size(); j++) {
              distance += (stddev.get(j) - s.getJSONArray("stddev").getFloat(j)) *
                          (stddev.get(j) - s.getJSONArray("stddev").getFloat(j));
            }
            distance = sqrt(distance); // not that it matters
            if (distance < minDistance) {
              minDistance = distance;
              minClass = c;
            }
          }
        }
      }
      
      if (saveClass) {
        classes.getJSONArray(classNum).append(sample);
      }
      
      saveClass = false; // Only save the first / most recent / rightmost time window.
      firstWindow = false;
    }
  }
  
  translate(window * 10, 0);
  
  for (int c = 0; c < 10; c++) {
    fill(255);
    text("" + char(c + '0'), width - window * (10 - c) + 10, 20);
    if (c == minClass) {
      fill(255);
      rect(width - window * (10 - c), 0, window, height);
    }
    for (int i = 0; i < classes.getJSONArray(c).size(); i++) {
      JSONObject sample = classes.getJSONArray(c).getJSONObject(i);
      for (int j = 0; j < sample.getJSONArray("mean").size(); j++) {
        float mean = sample.getJSONArray("mean").getFloat(j);
        mean = map(mean, 0, 1023, 0, height / 3);
        stroke(255 / sample.getJSONArray("mean").size() * j, 255, 255);
        line(width - window * (10 - c), 2 * height / 3 - mean, width - window * (10 - c - 1), 2 * height / 3 - mean); 
      }
      for (int j = 0; j < sample.getJSONArray("stddev").size(); j++) {
        float stddev = sample.getJSONArray("stddev").getFloat(j);
        stddev = map(stddev, 0, 1023, 0, height / 3);
        stroke(255 / sample.getJSONArray("stddev").size() * j, 255, 255);
        line(width - window * (10 - c), height - stddev, width - window * (10 - c - 1), height - stddev); 
      }
    }
  }
}

synchronized void serialEvent(Serial myPort) {
  // get the ASCII string:
  String inString = myPort.readStringUntil('\n');
  //print(inString);

  if (inString != null) {
    // sometimes garbage arrives in the first couple of seconds.
    if (millis() < 2000) return;

    // trim off any whitespace:
    inString = trim(inString);
    int[] v = int(splitTokens(inString));
    if (v.length < 1) return;
    vals.add(v);
  }
}

synchronized void keyPressed() {
  // mark the most recent time window as belonging to the class indicated by the key pressed
  if (key >= '0' && key <= '9') {
    saveClass = true;
    classNum = key - '0';
  }
  
  if (key == 's') {
    saveJSONArray(classes, "data/classes.json");
    println("Saved data/classes.json");
  }
  
  if (key == 'l') {
    classes = loadJSONArray("data/classes.json");
    println("Loaded data/classes.json");
  }
  
  if (key == 'a') {
    saveArduinoData(classes, "data/ArduinoKNN/data.h");
  }
}

void saveArduinoData(JSONArray data, String filename) {
  PrintWriter out = createWriter(filename);
  int dim = 0, num = 0;
  
  for (int c = 0; c < data.size(); c++) {
    num += data.getJSONArray(c).size();
    for (int i = 0; i < data.getJSONArray(c).size(); i++) {
      JSONObject sample = data.getJSONArray(c).getJSONObject(i);
      dim = sample.getJSONArray("mean").size() + sample.getJSONArray("stddev").size(); 
    }
  }
  
  out.println("const int NUM = " + num + ";");
  out.println("const int DIM = " + dim + ";");
  out.println();
  
  out.println("float data[NUM][DIM] = {");
  for (int c = 0; c < data.size(); c++) {
    for (int i = 0; i < data.getJSONArray(c).size(); i++) {
      JSONObject sample = data.getJSONArray(c).getJSONObject(i);
      out.print("  { ");
      for (int j = 0; j < sample.getJSONArray("mean").size(); j++) {
        out.print(sample.getJSONArray("mean").getFloat(j) + ", ");
      }
      for (int j = 0; j < sample.getJSONArray("stddev").size(); j++) {
        out.print(sample.getJSONArray("stddev").getFloat(j) + ", ");
      }
      out.println(" },");
    }
  }
  out.println("};"); out.println();

  out.println("float classes[NUM] = {");
  for (int c = 0; c < data.size(); c++) {
    for (int i = 0; i < data.getJSONArray(c).size(); i++) {
      out.println("  " + c + ",");
    }
  }
  out.println("};");
  out.flush();
  out.close();
}