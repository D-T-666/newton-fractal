debug:
	g++ -o out main.cpp

run:
	g++ -o out -O3 main.cpp
	./out
	# magick out.ppm -fuzz 0% -fill transparent -opaque black out.png
	magick out.ppm -scale 50% out.png
	rm out.ppm
