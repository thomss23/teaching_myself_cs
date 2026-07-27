int sum(int *nums, int n) {
  int total1 = 0;
  int total2 = 0;

  for (int i = 0; i + 1 < n; i = i + 2) {
    total1 += nums[i];
    total2 += nums[i + 1];
  }

  return total1 + total2;
}
