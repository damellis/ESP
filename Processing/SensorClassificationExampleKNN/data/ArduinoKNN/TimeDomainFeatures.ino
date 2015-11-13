float mean(int vals[], int num)
{
  float total = 0;
  for (int i = 0; i < num; i++) total += vals[i];
  return total / num;
}

float stddev(int vals[], int num)
{
  float m = mean(vals, num);
  float s = 0;

  for (int i = 0; i < num; i++) s += (vals[i] - m) * (vals[i] - m);

  return sqrt(s / num);
}

