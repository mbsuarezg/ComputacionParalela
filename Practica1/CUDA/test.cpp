#include <bits/stdc++.h>

using namespace std;

int main(){
	int H, W, h, w;	cin >> H >> W >> h >> w;

	int yoffset = (H + h - 1) / h;
	int xoffset = (W + w - 1) / w;

	printf("\nH:%d W:%d \nh:%d w:%d \nyoffset:%d xoffest:%d\n", H, W, h, w, yoffset, xoffset);
	cout << H*W*3 << '\n';
	ofstream fout;
	fout.open("mmmm.txt", ios_base::app);
	for(int i = 0; i < h*w; ++i){
		int a = (H * (i / w)) / h;
		int b = (W * (i % w)) / w;
		int x = a*W + b;
		fout << x << ' ';
		if(i && (i + 1) % w == 0) fout << "\n";
	}
	return 0;
}