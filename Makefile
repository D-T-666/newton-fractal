run:
	g++ -o out -Ofast main.cpp -std=c++20
	./out
	magick out.ppm -fuzz 0% -fill transparent -opaque black out.png
