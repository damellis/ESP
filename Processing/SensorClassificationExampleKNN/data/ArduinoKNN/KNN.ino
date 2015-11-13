float distance(float a[], float b[], int num) {
  float total = 0;
  for (int i = 0; i < num; i++) total += (a[i] - b[i]) * (a[i] - b[i]);
  return sqrt(total);
}

int predict(float sample[]) {
  float mindist = 9999999.0;
  int minclass = -1;
  for (int i = 0; i < NUM; i++) {
    float dist = distance(sample, data[i], DIM);
    if (dist < mindist) {
      mindist = dist;
      minclass = classes[i];
    }
  }

  return minclass;
}

